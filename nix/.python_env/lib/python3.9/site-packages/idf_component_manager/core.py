"""Core module of component manager"""
from __future__ import print_function

import os
import re
import shutil
import tarfile
import tempfile
import time
from datetime import datetime, timedelta
from io import open
from pathlib import Path

import requests

from idf_component_manager.utils import info, warn
from idf_component_tools.api_client_errors import APIClientError
from idf_component_tools.archive_tools import pack_archive, unpack_archive
from idf_component_tools.build_system_tools import build_name
from idf_component_tools.errors import FatalError, GitError, ManifestError, NothingToDoError
from idf_component_tools.file_tools import copy_filtered_directory, create_directory
from idf_component_tools.git_client import GitClient
from idf_component_tools.hash_tools import (
    HashDoesNotExistError, HashNotEqualError, HashNotSHA256Error, validate_dir_with_hash_file)
from idf_component_tools.manifest import (
    MANIFEST_FILENAME, WEB_DEPENDENCY_REGEX, Manifest, ManifestManager, ProjectRequirements)
from idf_component_tools.semver import SimpleSpec, Version
from idf_component_tools.sources import WebServiceSource

from .cmake_component_requirements import CMakeRequirementsManager, ComponentName, handle_project_requirements
from .context_manager import make_ctx
from .core_utils import ProgressBar, archive_filename, dist_name, raise_component_modified_error
from .dependencies import download_project_dependencies
from .local_component_list import parse_component_list
from .service_details import create_api_client, get_namespace, get_profile, service_details

try:
    from typing import Optional, Tuple
except ImportError:
    pass

try:
    PROCESSING_TIMEOUT = int(os.getenv('COMPONENT_MANAGER_JOB_TIMEOUT', 300))
except TypeError:
    warn(
        'Cannot parse value of COMPONENT_MANAGER_JOB_TIMEOUT.'
        ' It should be number of seconds to wait for job result.')
    PROCESSING_TIMEOUT = 300

CHECK_INTERVAL = 3
MAX_PROGRESS = 100  # Expected progress is in percent


class ComponentManager(object):
    def __init__(self, path, lock_path=None, manifest_path=None):
        # type: (str, Optional[str], Optional[str]) -> None

        # Working directory
        self.path = os.path.abspath(path if os.path.isdir(path) else os.path.dirname(path))

        # Set path of the project's main component
        self.main_component_path = os.path.join(self.path, 'main')

        # Set path of the manifest file for the project's main component
        self.main_manifest_path = manifest_path or (
            os.path.join(path, 'main', MANIFEST_FILENAME) if os.path.isdir(path) else path)

        # Lock path
        self.lock_path = lock_path or (os.path.join(path, 'dependencies.lock') if os.path.isdir(path) else path)

        # Components directories
        self.components_path = os.path.join(self.path, 'components')
        self.managed_components_path = os.path.join(self.path, 'managed_components')

        # Dist directory
        self.dist_path = os.path.join(self.path, 'dist')

    def _get_manifest(self, component='main'):  # type: (str) -> Tuple[str, bool]
        base_dir = self.path if component == 'main' else self.components_path
        manifest_dir = os.path.join(base_dir, component)

        if not os.path.isdir(manifest_dir):
            raise FatalError(
                'Directory "{}" does not exist! '
                'Please specify a valid component under {}'.format(manifest_dir, self.path))

        manifest_filepath = os.path.join(manifest_dir, MANIFEST_FILENAME)
        created = False
        # Create manifest file if it doesn't exist in work directory
        if not os.path.exists(manifest_filepath):
            example_path = os.path.join(
                os.path.dirname(os.path.realpath(__file__)), 'templates', 'idf_component_template.yml')
            create_directory(manifest_dir)
            shutil.copyfile(example_path, manifest_filepath)
            info('Created "{}"'.format(manifest_filepath))
            created = True
        return manifest_filepath, created

    def create_manifest(self, args):
        manifest_filepath, created = self._get_manifest(args.get('component', 'main'))
        if not created:
            info('"{}" already exists, skipping...'.format(manifest_filepath))

    def create_project_from_example(self, args):
        profile = get_profile(args.get('service_details'))
        namespace = get_namespace(profile, args.get('namespace') or 'espressif')
        component_name = '/'.join([namespace, args.get('name')])
        version = args.get('version') or '*'
        example_full_name = args.get('example')
        if not example_full_name:
            raise FatalError('Failed to get example name. Please use --example flag to select example name')
        example_name = os.path.basename(example_full_name)
        project_path = os.path.join(args.get('path'), example_name)

        if os.path.isfile(project_path):
            raise FatalError(
                'Your target path is not a directory. Please remove the {} or use different target path.'.format(
                    os.path.abspath(project_path)),
                exit_code=4)
        if os.path.isdir(project_path) and os.listdir(project_path):
            raise FatalError(
                'The directory {} is not empty. To create an example you must empty the directory or '
                'choose a different path.'.format(project_path),
                exit_code=3)

        client = create_api_client()
        try:
            component_details = client.component(component_name, version)
        except APIClientError:
            raise FatalError(
                'Selected component "{}" with selected version "{}" doesn\'t exist.'.format(component_name, version))

        try:
            example_url = [example for example in component_details.examples
                           if example_full_name == example['name']][-1]
        except IndexError:
            raise FatalError(
                'Cannot find example "{}" for {} version {}'.format(example_full_name, component_name, version),
                exit_code=2)

        response = requests.get(example_url['url'], stream=True)
        with tarfile.open(fileobj=response.raw, mode='r|gz') as tar:
            tar.extractall(project_path)
        info('Example {} successfully downloaded to {}'.format(example_full_name, os.path.abspath(project_path)))

    def add_dependency(self, args):
        dependency = args.get('dependency')
        manifest_filepath, _ = self._get_manifest(args.get('component', 'main'))

        match = re.match(WEB_DEPENDENCY_REGEX, dependency)
        if match:
            name, spec = match.groups()
        else:
            raise FatalError('Invalid dependency: "{}". Please use format "namespace/name".'.format(dependency))

        if not spec:
            spec = '*'

        try:
            SimpleSpec(spec)
        except ValueError:
            raise FatalError(
                'Invalid dependency version requirement: {}. Please use format like ">=1" or "*".'.format(spec))

        name = WebServiceSource().normalized_name(name)
        manifest_manager = ManifestManager(manifest_filepath, args.get('component'))
        manifest = manifest_manager.load()

        for dependency in manifest.dependencies:
            if dependency.name == name:
                raise FatalError('Dependency "{}" already exists for in manifest "{}"'.format(name, manifest_filepath))

        with open(manifest_filepath, 'r', encoding='utf-8') as file:
            file_lines = file.readlines()

        index = 0
        if 'dependencies' in manifest_manager.manifest_tree.keys():
            for i, line in enumerate(file_lines):
                if line.startswith('dependencies:'):
                    index = i + 1
                    break
        else:
            file_lines.append('\ndependencies:\n')
            index = len(file_lines) + 1

        file_lines.insert(index, '  {}: "{}"\n'.format(name, spec))

        # Check result for correctness
        with tempfile.NamedTemporaryFile(delete=False) as temp_manifest_file:
            temp_manifest_file.writelines(line.encode('utf-8') for line in file_lines)

        try:
            ManifestManager(temp_manifest_file.name, name).load()
        except ManifestError:
            raise ManifestError(
                'Cannot update manifest file. '
                "It's likely due to the 4 spaces used for indentation we recommend using 2 spaces indent. "
                'Please check the manifest file:\n{}'.format(manifest_filepath))

        shutil.move(temp_manifest_file.name, manifest_filepath)
        info('Successfully added dependency "{}{}" for component "{}"'.format(name, spec, manifest_manager.name))

    def pack_component(self, args):  # type: (dict) -> Tuple[str, Manifest]
        version = args.get('version')

        if version == 'git':
            try:
                version = GitClient().get_tag_version()
            except GitError:
                raise FatalError('An error happend while getting version from git tag')
        elif version:
            try:
                Version.parse(version)
            except ValueError:
                raise FatalError('Version parameter must be either "git" or a valid semantic version')

        manifest_manager = ManifestManager(self.path, args['name'], check_required_fields=True, version=version)
        manifest = manifest_manager.load()
        dist_temp_dir = os.path.join(self.dist_path, dist_name(manifest))
        copy_filtered_directory(
            self.path, dist_temp_dir, include=set(manifest.files['include']), exclude=set(manifest.files['exclude']))
        manifest_manager.dump(dist_temp_dir)
        archive_filepath = os.path.join(self.dist_path, archive_filename(manifest))
        info('Saving archive to "{}"'.format(archive_filepath))
        pack_archive(dist_temp_dir, archive_filepath)
        return archive_filepath, manifest

    def delete_version(self, args):
        client, namespace = service_details(args.get('namespace'), args.get('service_profile'))
        name = args.get('name')
        version = args.get('version')

        if not version:
            raise FatalError('Argument "version" is required')

        component_name = '/'.join([namespace, name])
        # Checking if current version already uploaded
        versions = client.versions(component_name).versions

        if version not in versions:
            raise NothingToDoError(
                'Version {} of the component "{}" is not on the service'.format(version, component_name))

        try:
            client.delete_version(component_name=component_name, component_version=version)
            info('Deleted version {} of the component {}'.format(component_name, version))
        except APIClientError as e:
            raise FatalError(e)

    def remove_managed_components(self, args):
        managed_components_dir = Path(self.path, 'managed_components')

        if not managed_components_dir.is_dir():
            return

        undeleted_components = []
        for component_dir in managed_components_dir.glob('*/'):

            if not (managed_components_dir / component_dir).is_dir():
                continue

            try:
                validate_dir_with_hash_file(str(managed_components_dir / component_dir))
                shutil.rmtree(str(managed_components_dir / component_dir))
            except (HashNotEqualError, HashNotSHA256Error):
                undeleted_components.append(component_dir.name)
            except HashDoesNotExistError:
                pass

        if undeleted_components:
            raise_component_modified_error(str(managed_components_dir), undeleted_components)

        elif any(managed_components_dir.iterdir()) == 0:
            shutil.rmtree(str(managed_components_dir))

    def upload_component(self, args):
        client, namespace = service_details(args.get('namespace'), args.get('service_profile'))
        version = args.get('version')
        archive_file = args.get('archive')
        if archive_file:
            if not os.path.isfile(archive_file):
                raise FatalError('Cannot find archive to upload: {}'.format(archive_file))

            if version:
                raise FatalError('Parameters "version" and "archive" are not supported at the same time')

            tempdir = tempfile.mkdtemp()
            try:
                unpack_archive(archive_file, tempdir)
                manifest = ManifestManager(tempdir, args['name'], check_required_fields=True).load()
            finally:
                shutil.rmtree(tempdir)
        else:
            archive_file, manifest = self.pack_component(args)

        if not manifest.version.is_semver:
            raise FatalError('Only components with semantic versions are allowed on the service')

        if manifest.version.semver.prerelease and args.get('skip_pre_release'):
            raise NothingToDoError('Skipping pre-release version {}'.format(manifest.version))

        try:
            component_name = '/'.join([namespace, manifest.name])
            # Checking if current version already uploaded
            versions = client.versions(component_name, spec='*').versions
            if manifest.version in versions:
                if args.get('allow_existing'):
                    return

                raise NothingToDoError(
                    'Version {} of the component "{}" is already on the service'.format(
                        manifest.version, component_name))

            # Exit if check flag was set
            if args.get('check_only'):
                return

            # Uploading the component
            info('Uploading archive: %s' % archive_file)
            job_id = client.upload_version(component_name=component_name, file_path=archive_file)

            # Wait for processing
            info(
                'Wait for processing, it is safe to press CTRL+C and exit\n'
                'You can check the state of processing by running subcommand '
                '"upload-component-status --job=%s"' % job_id)

            timeout_at = datetime.now() + timedelta(seconds=PROCESSING_TIMEOUT)

            try:
                with ProgressBar(total=MAX_PROGRESS, unit='%') as progress_bar:
                    while True:
                        if datetime.now() > timeout_at:
                            raise TimeoutError()
                        status = client.task_status(job_id)
                        progress_bar.set_description(status.message)
                        progress_bar.update_to(status.progress)

                        if status.status == 'failure':
                            raise FatalError("Uploaded version wasn't processed successfully.\n%s" % status.message)
                        elif status.status == 'success':
                            return

                        time.sleep(CHECK_INTERVAL)
            except TimeoutError:
                raise FatalError(
                    "Component wasn't processed in {} seconds. Check processing status later.".format(
                        PROCESSING_TIMEOUT))

        except APIClientError as e:
            raise FatalError(e)

    def upload_component_status(self, args):
        job_id = args.get('job')

        if not job_id:
            raise FatalError('Job ID is required')

        client, _ = service_details(None, args.get('service_profile'))
        try:
            status = client.task_status(job_id)
            if status.status == 'failure':
                raise FatalError("Uploaded version wasn't processed successfully.\n%s" % status.message)
            else:
                info('Status: %s. %s' % (status.status, status.message))

        except APIClientError as e:
            raise FatalError(e)

    def prepare_dep_dirs(self, managed_components_list_file, component_list_file, local_components_list_file=None):
        '''Process all manifests and download all dependencies'''
        # Find all components
        local_components = []
        if local_components_list_file and os.path.isfile(local_components_list_file):
            local_components = parse_component_list(local_components_list_file)
        else:
            local_components.append({'name': 'main', 'path': self.main_component_path})

            if os.path.isdir(self.components_path):
                local_components.extend(
                    {
                        'name': item,
                        'path': os.path.join(self.components_path, item)
                    } for item in os.listdir(self.components_path)
                    if os.path.isdir(os.path.join(self.components_path, item)))

        # Check that CMakeLists.txt and idf_component.yml exists for all component dirs
        local_components = [
            component for component in local_components
            if os.path.isfile(os.path.join(component['path'], 'CMakeLists.txt'))
            and os.path.isfile(os.path.join(component['path'], MANIFEST_FILENAME))
        ]

        downloaded_component_paths = set()
        if local_components:
            manifests = []

            for component in local_components:
                manifest_filepath = os.path.join(component['path'], MANIFEST_FILENAME)
                with make_ctx('manifest', manifest_path=manifest_filepath):
                    manifests.append(ManifestManager(component['path'], component['name']).load())

            project_requirements = ProjectRequirements(manifests)
            downloaded_component_paths = download_project_dependencies(
                project_requirements, self.lock_path, self.managed_components_path)

        # Exclude requirements paths
        downloaded_component_paths -= {component['path'] for component in local_components}
        # Change relative paths to absolute paths
        downloaded_component_paths = {os.path.abspath(path) for path in list(downloaded_component_paths)}
        # Include managed components in project directory
        with open(managed_components_list_file, mode='w', encoding='utf-8') as file:
            for component_path in downloaded_component_paths:
                file.write(u'idf_build_component("%s")\n' % Path(component_path).as_posix())

            component_names = ';'.join(os.path.basename(path) for path in downloaded_component_paths)
            file.write(u'set(managed_components "%s")\n' % component_names)

        # Saving list of all components with manifests for use on requirements injection step
        all_components = downloaded_component_paths.union(component['path'] for component in local_components)
        with open(component_list_file, mode='w', encoding='utf-8') as file:
            file.write(u'\n'.join(all_components))

    def inject_requirements(
            self,
            interface_version,  # type: int
            component_requires_file,  # type: Path | str
            component_list_file,  # type: Path | str
    ):
        '''Set build dependencies for components with manifests'''
        requirements_manager = CMakeRequirementsManager(component_requires_file)
        requirements = requirements_manager.load()

        try:
            with open(component_list_file, mode='r', encoding='utf-8') as f:
                components_with_manifests = f.readlines()
            os.remove(component_list_file)
        except FileNotFoundError:
            raise FatalError('Cannot find component list file. Please make sure this script is executed from CMake')

        add_all_components_to_main = False
        for component in components_with_manifests:
            component = component.strip()
            name = os.path.basename(component)
            manifest = ManifestManager(component, name).load()
            name_key = ComponentName('idf', name)

            for dependency in manifest.dependencies:
                # Meta dependencies, like 'idf' are not used directly
                if dependency.meta:
                    continue

                dependency_name = build_name(dependency.name)
                requirement_key = 'REQUIRES' if dependency.public else 'PRIV_REQUIRES'

                def add_req(key):  # type: (str) -> None
                    if key not in requirements[name_key]:
                        requirements[name_key][key] = []

                    req = requirements[name_key][key]
                    if isinstance(req, list) and dependency_name not in req:
                        req.append(dependency_name)

                add_req(requirement_key)

                managed_requirement_key = 'MANAGED_{}'.format(requirement_key)
                add_req(managed_requirement_key)

                # In interface v0, component_requires_file contains also common requirements
                if interface_version == 0 and name_key == ComponentName('idf', 'main'):
                    add_all_components_to_main = True

        # If there are dependencies added to the `main` component,
        # and common components were included to the requirements file
        # then add every other component to it dependencies
        # to reproduce convenience behavior for the standard project defined in IDF's `project.cmake`
        # For ESP-IDF < 5.0 (Remove after ESP-IDF 4.4 EOL)
        if add_all_components_to_main:
            main_reqs = requirements[ComponentName('idf', 'main')]['REQUIRES']
            for requirement in requirements.keys():
                name = requirement.name
                if name not in main_reqs and name != 'main' and isinstance(main_reqs, list):
                    main_reqs.append(name)

        handle_project_requirements(requirements)
        requirements_manager.dump(requirements)
