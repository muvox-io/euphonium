import os
import re
import shutil
import tempfile
from hashlib import sha256

from ..errors import FetchingError
from ..file_tools import copy_filtered_directory
from ..git_client import GitClient
from ..hash_tools import hash_dir
from ..manifest import (
    MANIFEST_FILENAME, ComponentVersion, ComponentWithVersions, HashedComponentVersion, ManifestManager)
from .base import BaseSource

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

BRANCH_TAG_RE = re.compile(r'^(?!.*/\.)(?!.*\.\.)(?!/)(?!.*//)(?!.*@\{)(?!.*\\)[^\177\s~^:?*\[]+[^.]$')


class GitSource(BaseSource):
    NAME = 'git'

    def __init__(self, source_details=None, **kwargs):
        super(GitSource, self).__init__(source_details=source_details, **kwargs)
        self.git_repo = source_details['git']
        self.component_path = source_details.get('path') or '.'

        self._client = GitClient()

    def _checkout_git_source(
            self,
            version,  # type: str | ComponentVersion | None
            path,  # type: str
            selected_paths=None  # type: list[str]
    ):  # type: (...) -> str
        if version is not None:
            version = None if version == '*' else str(version)
        return self._client.prepare_ref(
            repo=self.git_repo,
            bare_path=self.cache_path(),
            checkout_path=path,
            ref=version,
            with_submodules=True,
            selected_paths=selected_paths)

    @staticmethod
    def is_me(name, details):  # type: (str, dict) -> bool
        return bool(details.get('git', None))

    @classmethod
    def required_keys(cls):
        return ['git']

    @classmethod
    def optional_keys(cls):
        return ['path']

    @property
    def component_hash_required(self):  # type: () -> bool
        return True

    @property
    def downloadable(self):  # type: () -> bool
        return True

    @property
    def hash_key(self):
        if self._hash_key is None:
            url = urlparse(self.git_repo)
            netloc = url.netloc
            path = '/'.join(filter(None, url.path.split('/')))
            normalized_path = '/'.join([netloc, path])
            self._hash_key = sha256(normalized_path.encode('utf-8')).hexdigest()
        return self._hash_key

    def cache_path(self):
        # Using `b_` prefix for bare git repos in cache
        path = os.path.join(self.system_cache_path, 'b_{}_{}'.format(self.NAME, self.hash_key[:8]))
        return path

    def download(self, component, download_path):  # type: (SolvedComponent, str) -> List[str]
        # Check for required components
        if not component.component_hash:
            raise FetchingError('Component hash is required for components from git repositories')

        if not component.version:
            raise FetchingError('Version should provided for %s' % component.name)

        if self.up_to_date(component, download_path):
            return [download_path]

        temp_dir = tempfile.mkdtemp()
        try:
            self._checkout_git_source(component.version, temp_dir, selected_paths=[self.component_path])
            source_path = os.path.join(str(temp_dir), self.component_path)
            if not os.path.isdir(source_path):
                raise FetchingError(
                    'Directory {} wasn\'t found for the commit id "{}" of the git repository "{}"'.format(
                        self.component_path, component.version, self.git_repo))

            if os.path.isdir(download_path):
                shutil.rmtree(download_path)

            possible_manifest_filepath = os.path.join(source_path, MANIFEST_FILENAME)
            include, exclude = set(), set()
            if os.path.isfile(possible_manifest_filepath):
                manifest = ManifestManager(possible_manifest_filepath, component.name).load()
                include.update(manifest.files['include'])
                exclude.update(manifest.files['exclude'])

            copy_filtered_directory(source_path, download_path, include=include, exclude=exclude)
        finally:
            shutil.rmtree(temp_dir)

        return [download_path]

    def versions(self, name, details=None, spec='*', target=None):
        """For git returns hash of locked commit, ignoring manifest"""
        version = None if spec == '*' else spec
        temp_dir = tempfile.mkdtemp()
        try:
            commit_id = self._checkout_git_source(version, temp_dir, selected_paths=[self.component_path])
            source_path = os.path.join(str(temp_dir), self.component_path)

            if not os.path.isdir(source_path):
                dependency_description = 'commit id "{}"'.format(commit_id)
                if version:
                    dependency_description = 'version "{}" ({})'.format(version, dependency_description)
                raise FetchingError(
                    'Directory {} wasn\'t found for the {} of the git repository "{}"'.format(
                        self.component_path, dependency_description, self.git_repo))

            manifest_path = os.path.join(source_path, MANIFEST_FILENAME)
            targets = []
            dependencies = []

            if os.path.isfile(manifest_path):
                manifest = ManifestManager(manifest_path, name=name).load()
                dependencies = manifest.dependencies

                if manifest.targets:  # only check when exists
                    if target and target not in manifest.targets:
                        raise FetchingError(
                            'Version "{}" (commit id "{}") of the component "{}" does not support target "{}"'.format(
                                version, commit_id, name, target))

                    targets = manifest.targets

            component_hash = hash_dir(source_path)
        finally:
            shutil.rmtree(temp_dir)

        return ComponentWithVersions(
            name=name,
            versions=[
                HashedComponentVersion(
                    commit_id, targets=targets, component_hash=component_hash, dependencies=dependencies)
            ],
        )

    def serialize(self):  # type: () -> Dict
        source = {
            'git': self.git_repo,
            'type': self.name,
        }

        if self.component_path:
            source['path'] = self.component_path

        return source

    def validate_version_spec(self, spec):  # type: (str) -> bool
        if not spec or spec == '*':
            return True

        return bool(BRANCH_TAG_RE.match(spec))

    def normalize_spec(self, spec):  # type: (str) -> str
        if not spec:
            return '*'
        ref = None if spec == '*' else spec
        commit_id = self._client.get_commit_id_by_ref(self.git_repo, self.cache_path(), ref)
        return commit_id
