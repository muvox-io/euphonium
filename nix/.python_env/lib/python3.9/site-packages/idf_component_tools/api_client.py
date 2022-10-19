"""Classes to work with Espressif Component Web Service"""
import os
import platform
from collections import namedtuple
from functools import wraps
from io import open

import requests
from requests.adapters import HTTPAdapter
from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
from schema import Schema, SchemaError
from tqdm import tqdm

# Import whole module to avoid circular dependencies
import idf_component_tools as tools
from idf_component_tools.__version__ import __version__
from idf_component_tools.semver import SimpleSpec, Version

from .api_client_errors import KNOWN_API_ERRORS, APIClientError, ComponentNotFound
from .api_schemas import COMPONENT_SCHEMA, ERROR_SCHEMA, TASK_STATUS_SCHEMA, VERSION_UPLOAD_SCHEMA
from .manifest import Manifest

try:
    from typing import TYPE_CHECKING, Dict, List, Optional, Tuple, Union

    if TYPE_CHECKING:
        from idf_component_tools.sources import BaseSource
except ImportError:
    pass

TaskStatus = namedtuple('TaskStatus', ['message', 'status', 'progress'])

DEFAULT_TIMEOUT = (
    6.05,  # Connect timeout
    30.1,  # Read timeout
)


class ComponentDetails(Manifest):
    def __init__(
            self,
            download_url=None,  # type: str | None # Direct url for tarball download
            documents=None,  # type: list[dict[str, str]] | None # List of documents of the component
            license=None,  # type: dict[str, str] | None # Information about license
            examples=None,  # type: list[dict[str, str]] | None # List of examples of the component
            *args,
            **kwargs):
        super(ComponentDetails, self).__init__(*args, **kwargs)
        self.download_url = download_url
        self.documents = documents
        self.license = license
        self.examples = examples


def handle_4xx_error(error):  # type: (requests.Response) -> str
    try:
        json = ERROR_SCHEMA.validate(error.json())
        name = json['error']
        messages = json['messages']
    except SchemaError as e:
        raise APIClientError('API Endpoint "{}: returned unexpected error description:\n{}'.format(error.url, str(e)))
    except ValueError:
        raise APIClientError('Server returned an error in unexpected format')

    exception = KNOWN_API_ERRORS.get(name, APIClientError)
    if isinstance(messages, list):
        raise exception('\n'.join(messages))
    else:
        raise exception(
            'Error during request:\n{}\nStatus code: {} Error code: {}'.format(str(messages), error.status_code, name))


def join_url(*args):  # type: (*str) -> str
    """
    Joins given arguments into an url and add trailing slash
    """
    parts = [part[:-1] if part and part[-1] == '/' else part for part in args]
    parts.append('')
    return '/'.join(parts)


def auth_required(f):
    @wraps(f)
    def wrapper(self, *args, **kwargs):
        if not self.session.auth.token:
            raise APIClientError('API token is required')
        return f(self, *args, **kwargs)

    return wrapper


class TokenAuth(requests.auth.AuthBase):
    def __init__(self, token):  # type: (Optional[str]) -> None
        self.token = token

    def __call__(self, request):
        if self.token:
            request.headers['Authorization'] = 'Bearer %s' % self.token
        return request


def user_agent():  # type: () -> str
    return 'idf-component-manager/{version} ({os}/{release} {arch}; python/{py_version})'.format(
        version=__version__,
        os=platform.system(),
        release=platform.release(),
        arch=platform.machine(),
        py_version=platform.python_version(),
    )


class APIClient(object):
    def __init__(self, base_url, source=None, auth_token=None):
        # type: (str, Optional[BaseSource], Optional[str]) -> None
        self.base_url = base_url
        self.source = source

        api_adapter = HTTPAdapter(max_retries=3)

        session = requests.Session()
        session.headers['User-Agent'] = user_agent()
        session.auth = TokenAuth(auth_token)
        session.mount(base_url, api_adapter)
        self.session = session

    def _version_dependencies(self, version):
        dependencies = []
        for dependency in version.get('dependencies', []):
            # Support only idf and service sources
            if dependency['source'] == 'idf':
                source = tools.sources.IDFSource({})
            else:
                source = self.source or tools.sources.WebServiceSource({})

            dependencies.append(
                tools.manifest.ComponentRequirement(
                    name='{}/{}'.format(dependency['namespace'], dependency['name']),
                    version_spec=dependency['spec'],
                    public=dependency['is_public'],
                    source=source,
                ))

        return dependencies

    def _base_request(self, method, path, data=None, headers=None, schema=None):
        # type: (str, List[str], Optional[Dict], Optional[Dict], Schema) -> Dict
        endpoint = join_url(self.base_url, *path)

        timeout = DEFAULT_TIMEOUT  # type: Union[float, Tuple[float, float]]
        try:
            timeout = float(os.environ['IDF_COMPONENT_SERVICE_TIMEOUT'])
        except ValueError:
            raise APIClientError('Cannot parse IDF_COMPONENT_SERVICE_TIMEOUT. It should be a number in seconds.')
        except KeyError:
            pass

        try:
            response = self.session.request(
                method,
                endpoint,
                data=data,
                headers=headers,
                timeout=timeout,
            )

            if response.status_code == 204:  # NO CONTENT
                return {}
            elif 400 <= response.status_code < 500:
                handle_4xx_error(response)

            elif 500 <= response.status_code < 600:
                raise APIClientError(
                    'Internal server error happended while processing requrest to:\n{}\nStatus code: {}'.format(
                        endpoint, response.status_code))

            json = response.json()
        except requests.exceptions.RequestException:
            raise APIClientError('HTTP request error')

        try:
            if schema is not None:
                schema.validate(json)
        except SchemaError as e:
            raise APIClientError('API Endpoint "{}: returned unexpected JSON:\n{}'.format(endpoint, str(e)))

        except (ValueError, KeyError, IndexError):
            raise APIClientError('Unexpected component server response')

        return json

    def versions(self, component_name, spec='*', target=None):
        """List of versions for given component with required spec"""
        semantic_spec = SimpleSpec(spec or '*')
        component_name = component_name.lower()
        try:
            body = self._base_request(
                'get',
                ['components', component_name],
                schema=COMPONENT_SCHEMA,
            )
        except ComponentNotFound:
            versions = []
        else:
            versions = []
            for version in body['versions']:
                if semantic_spec.match(Version(version['version'])):
                    if target and version['targets'] and target not in version['targets']:
                        continue
                    versions.append(version)

        return tools.manifest.ComponentWithVersions(
            name=component_name,
            versions=[
                tools.manifest.HashedComponentVersion(
                    version_string=version['version'],
                    component_hash=version['component_hash'],
                    dependencies=self._version_dependencies(version),
                    targets=version['targets'],
                ) for version in versions
            ],
        )

    def component(self, component_name, version=None):
        """Manifest for given version of component, if version is None most recent version returned"""
        response = self._base_request(
            'get',
            ['components', component_name.lower()],
            schema=COMPONENT_SCHEMA,
        )
        versions = response['versions']

        if version:
            requested_version = tools.manifest.ComponentVersion(str(version))
            best_version = [v for v in versions
                            if tools.manifest.ComponentVersion(v['version']) == requested_version][0]
        else:
            best_version = max(versions, key=lambda v: Version(v['version']))

        return ComponentDetails(
            name=('%s/%s' % (response['namespace'], response['name'])),
            version=tools.manifest.ComponentVersion(best_version['version']),
            dependencies=self._version_dependencies(best_version),
            maintainers=None,
            download_url=best_version['url'],
            documents=best_version['docs'],
            license=best_version['license'],
            examples=best_version['examples'])

    @auth_required
    def upload_version(self, component_name, file_path):
        with open(file_path, 'rb') as file:
            filename = os.path.basename(file_path)

            encoder = MultipartEncoder({'file': (filename, file, 'application/octet-stream')})
            headers = {'Content-Type': encoder.content_type}

            progress_bar = tqdm(total=encoder.len, unit_scale=True, unit='B', disable=None)

            def callback(monitor, memo={'progress': 0}):  # type: (MultipartEncoderMonitor, dict) -> None
                progress_bar.update(monitor.bytes_read - memo['progress'])
                memo['progress'] = monitor.bytes_read

            data = MultipartEncoderMonitor(encoder, callback)

            try:
                return self._base_request(
                    'post',
                    ['components', component_name.lower(), 'versions'],
                    data=data,
                    headers=headers,
                    schema=VERSION_UPLOAD_SCHEMA,
                )['job_id']
            finally:
                progress_bar.close()

    @auth_required
    def delete_version(self, component_name, component_version):
        self._base_request('delete', ['components', component_name.lower(), component_version])

    def task_status(self, job_id):  # type: (str) -> TaskStatus
        body = self._base_request('get', ['tasks', job_id], schema=TASK_STATUS_SCHEMA)
        return TaskStatus(body['message'], body['status'], body['progress'])
