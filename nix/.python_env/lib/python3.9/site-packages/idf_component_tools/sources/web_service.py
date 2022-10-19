"""Component source that downloads components from web service"""

import os
import re
import shutil
import tempfile
from hashlib import sha256
from io import open

import requests

import idf_component_tools.api_client as api_client

from ..archive_tools import ArchiveError, get_format_from_path, unpack_archive
from ..config import ConfigManager
from ..errors import FetchingError
from ..file_tools import copy_directory
from ..hash_tools import validate_dir
from . import utils
from .base import BaseSource
from .constants import DEFAULT_COMPONENT_SERVICE_URL

try:
    from urllib.parse import urlparse  # type: ignore
except ImportError:
    from urlparse import urlparse  # type: ignore

try:
    from typing import TYPE_CHECKING, Dict, List

    if TYPE_CHECKING:
        from ..manifest import SolvedComponent
except ImportError:
    pass


def default_component_service_url(service_profile=None):
    env_service_url = os.getenv('DEFAULT_COMPONENT_SERVICE_URL')
    if env_service_url:
        return env_service_url

    env_service_profile_name = os.getenv('IDF_COMPONENT_SERVICE_PROFILE')
    if env_service_profile_name:
        service_profile = ConfigManager().load().profiles.get(env_service_profile_name, {})
    elif service_profile is None:
        service_profile = {}

    service_url = service_profile.get('url')
    if service_url and service_url != 'default':
        return service_url

    return DEFAULT_COMPONENT_SERVICE_URL


class WebServiceSource(BaseSource):
    NAME = 'service'

    def __init__(self, source_details=None, **kwargs):
        super(WebServiceSource, self).__init__(source_details=source_details, **kwargs)

        self.base_url = str(self.source_details.get('service_url', default_component_service_url()))
        self.api_client = self.source_details.get(
            'api_client', api_client.APIClient(base_url=self.base_url, source=self))

    @classmethod
    def required_keys(cls):
        return ['service_url']

    @property
    def hash_key(self):
        if self._hash_key is None:
            url = urlparse(self.base_url)
            netloc = url.netloc
            path = '/'.join(filter(None, url.path.split('/')))
            normalized_path = '/'.join([netloc, path])
            self._hash_key = sha256(normalized_path.encode('utf-8')).hexdigest()
        return self._hash_key

    @staticmethod
    def is_me(name, details):
        # This should be run last
        return True

    def component_cache_path(self, component):  # type: (SolvedComponent) -> str
        component_dir_name = '_'.join(
            [
                self.normalized_name(component.name).replace('/', '__'),
                str(component.version),
                str(component.component_hash),
            ])
        path = os.path.join(self.cache_path(), component_dir_name)
        return path

    def versions(self, name, details=None, spec='*', target=None):
        cmp_with_versions = self.api_client.versions(name, spec, target)

        if not cmp_with_versions:
            raise FetchingError('Cannot get versions of "%s"' % name)

        return cmp_with_versions

    @property
    def component_hash_required(self):  # type: () -> bool
        return True

    @property
    def downloadable(self):  # type: () -> bool
        return True

    def normalized_name(self, name):
        return utils.normalized_name(name)

    def download(self, component, download_path):  # type: (SolvedComponent, str) -> List[str]
        # Check for required components
        if not component.component_hash:
            raise FetchingError('Component hash is required for componets from web service')

        if not component.version:
            raise FetchingError('Version should be provided for %s' % component.name)

        if self.up_to_date(component, download_path):
            return [download_path]

        # Check if component is in the cache
        if validate_dir(self.component_cache_path(component), component.component_hash):
            copy_directory(self.component_cache_path(component), download_path)
            return [download_path]

        component_manifest = self.api_client.component(component.name, component.version)
        url = component_manifest.download_url

        if not url:
            raise FetchingError(
                'Unexpected response: URL wasn\'t found for version %s of "%s"',
                component.version,
                component.name,
            )

        with requests.get(url, stream=True, allow_redirects=True) as r:
            # Trying to get extension from url
            original_filename = url.split('/')[-1]

            try:
                extension = get_format_from_path(original_filename)[1]
            except ArchiveError:
                extension = None

            if r.status_code != 200:
                raise FetchingError(
                    'Cannot download component %s@%s. Server returned HTTP code %s' %
                    (component.name, component.version, r.status_code))

            # If didn't find anything useful, trying content disposition
            content_disposition = r.headers.get('content-disposition')
            if not extension and content_disposition:
                filenames = re.findall('filename=(.+)', content_disposition)
                try:
                    extension = get_format_from_path(filenames[0])[1]
                except IndexError:
                    raise FetchingError('Web Service returned invalid download url')

            tempdir = tempfile.mkdtemp()

            try:
                filename = 'component.%s' % extension
                file_path = os.path.join(tempdir, filename)

                with open(file_path, 'wb') as f:
                    for chunk in r.iter_content(chunk_size=65536):
                        if chunk:
                            f.write(chunk)

                unpack_archive(file_path, self.component_cache_path(component))
                copy_directory(self.component_cache_path(component), download_path)
            finally:
                shutil.rmtree(tempdir)

        return [download_path]

    @property
    def service_url(self):
        return self.base_url

    def serialize(self):  # type: () -> Dict
        return {
            'service_url': self.base_url,
            'type': self.name,
        }
