import os
import re
import sys

from schema import And, Optional, Or, Regex, Schema, SchemaError, Use
from six import string_types

import idf_component_tools as tools

from ..errors import SourceError
from ..semver import Version
from .constants import FULL_SLUG_REGEX, TAGS_REGEX
from .if_parser import parse_if_clause

try:
    from typing import Iterable, List
except ImportError:
    pass

KNOWN_ROOT_KEYS = [
    'name',  # Name key is ignored
    'maintainers',
    'dependencies',
    'targets',
    'version',
    'description',
    'files',
    'url',
    'tags',
]

DEFAULT_KNOWN_TARGETS = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'linux', 'esp32h2', 'esp32c2']

KNOWN_FILES_KEYS = [
    'include',
    'exclude',
]

KNOWN_IF_CLAUSE_KEYWORDS = ['IDF_TARGET', 'IDF_VERSION']

NONEMPTY_STRING = And(Or(*string_types), len, error='Non-empty string is required here')
SLUG_REGEX_COMPILED = re.compile(FULL_SLUG_REGEX)


def known_targets():  # type () -> list[str]
    try:
        targets = os.environ['IDF_COMPONENT_MANAGER_KNOWN_TARGETS'].split(',')
        if any(targets):
            return targets
    except KeyError:
        pass

    try:
        idf_path = os.environ['IDF_PATH']
    except KeyError:
        return DEFAULT_KNOWN_TARGETS

    try:
        sys.path.append(os.path.join(idf_path, 'tools'))
        from idf_py_actions.constants import PREVIEW_TARGETS, SUPPORTED_TARGETS
        return SUPPORTED_TARGETS + PREVIEW_TARGETS
    except ImportError:
        return DEFAULT_KNOWN_TARGETS


def known_component_keys():
    from idf_component_tools.sources import KNOWN_SOURCES
    return set(key for source in KNOWN_SOURCES for key in source.known_keys())


def dependency_schema():  # type () -> Schema
    return Or(
        Or(None, *string_types, error='Dependency version spec format is invalid'),
        {
            Optional('version'): Or(None, *string_types, error='Dependency version spec format is invalid'),
            Optional('public'): Use(bool, error='Invalid format of dependency public flag'),
            Optional('path'): NONEMPTY_STRING,
            Optional('git'): NONEMPTY_STRING,
            Optional('service_url'): NONEMPTY_STRING,
            Optional('rules'): [{
                'if': Use(parse_if_clause)
            }],
            Optional('override_path'): NONEMPTY_STRING,
        },
        error='Invalid dependency format',
    )


def manifest_schema():  # type () -> Schema
    return Schema(
        {
            Optional('name'): Or(*string_types),
            Optional('version'): Or(Version.parse, error='Component version should be valid semantic version'),
            Optional('targets'): known_targets(),
            Optional('maintainers'): [NONEMPTY_STRING],
            Optional('description'): NONEMPTY_STRING,
            Optional('url'): NONEMPTY_STRING,
            Optional('tags'): [
                Regex(
                    TAGS_REGEX,
                    error='Invalid tag. Tags may be between 3 and 32 symbols long and may contain '
                    'letters, numbers, _ and -')
            ],
            Optional('dependencies'): {
                Optional(Regex(FULL_SLUG_REGEX, error='Invalid dependency name')): dependency_schema()
            },
            Optional('files'): {Optional(key): [NONEMPTY_STRING]
                                for key in KNOWN_FILES_KEYS},
        },
        error='Invalid manifest format',
    )


class ManifestValidator(object):
    """Validator for manifest object, checks for structure, known fields and valid values"""
    def __init__(
            self, parsed_manifest, check_required_fields=False, version=None):  # type: (dict, bool, str | None) -> None
        self.manifest_tree = parsed_manifest
        self.version = version
        self._errors = []  # type: List[str]
        # Check for required fields when upload to the registry
        self.check_required_fields = check_required_fields

    @staticmethod
    def _validate_keys(manifest, known_keys):  # type: (dict, Iterable[str]) ->  List[str]
        unknown_keys = []
        for key in manifest.keys():
            if key not in known_keys:
                unknown_keys.append(key)
        return unknown_keys

    def add_error(self, message):
        self._errors.append(message)

    def validate_root_keys(self):  # type: () -> None
        unknown = sorted(self._validate_keys(self.manifest_tree, KNOWN_ROOT_KEYS))
        if unknown:
            self.add_error('Unknown keys: %s' % ', '.join(unknown))

    def _check_name(self, component):  # type: (str) -> None
        if not SLUG_REGEX_COMPILED.match(component):
            self.add_error(
                'Component\'s name is not valid "%s", should contain only letters, numbers, /, _ and -.' % component)

        if '__' in component:
            self.add_error('Component\'s name "%s" should not contain two consecutive underscores.' % component)

    def validate_normalize_dependencies(self):  # type: () -> None
        if 'dependencies' not in self.manifest_tree.keys() or not self.manifest_tree['dependencies']:
            return

        dependencies = self.manifest_tree['dependencies']

        # List of components should be a dictionary.
        if not isinstance(dependencies, dict):
            self.add_error(
                'List of dependencies should be a dictionary.'
                ' For example:\ndependencies:\n  some-component: ">=1.2.3,!=1.2.5"')

            return

        for component, details in dependencies.items():
            self._check_name(component)

            if isinstance(details, str):
                dependencies[component] = details = {'version': details}

            if isinstance(details, dict):
                try:
                    source = tools.sources.BaseSource.fromdict(component, details)

                    if not source.validate_version_spec(str(details.get('version', ''))):
                        self.add_error('Version specifications for "%s" are invalid.' % component)
                except SourceError as unknown_keys_error:
                    self.add_error(str(unknown_keys_error))
            else:
                self.add_error(
                    '"%s" version have unknown format. Should be either version string or dictionary with details' %
                    component)
                continue

    def validate_normalize_required_keys(self):  # type: () -> None
        '''Check for required keys in the manifest, if necessary'''
        if self.version:
            manifest_version = self.manifest_tree.get('version')
            if manifest_version and manifest_version != self.version:
                self.add_error(
                    'Manifest version ({}) does not match the version specified in the command line ({}). '
                    'Please either remove `--version` CLI parameter or update verson in the manifest.'.format(
                        manifest_version, self.version))
            else:
                self.manifest_tree['version'] = str(self.version)

        if not self.check_required_fields:
            return

        if not self.manifest_tree.get('version'):
            self.add_error(
                '"version" field is required in the "idf_component.yml" manifest when uploading to the registry.')

    def validate_targets(self):  # type: () -> None
        targets = self.manifest_tree.get('targets', [])

        if isinstance(targets, str):
            targets = [targets]

        if not isinstance(targets, list):
            self.add_error(
                'Unknown format for list of supported targets. '
                'It should be a list of targets, like [esp32, esp32s2]')
            return

        unknown_targets = []
        for target in targets:
            if target not in known_targets():
                unknown_targets.append(target)

        if unknown_targets:
            self.add_error('Unknown targets: %s' % ', '.join(unknown_targets))

    def validate_files(self):  # type: () -> None
        '''Check include/exclude patterns'''
        files = self.manifest_tree.get('files', {})
        for key in files:
            if key not in KNOWN_FILES_KEYS:
                self.add_error('"files" section contains unknown key: %s' % key)

    def validate_normalize_schema(self):
        try:
            self.manifest_tree = manifest_schema().validate(self.manifest_tree)
        except SchemaError as e:
            # Some format errors may not have detailed description, so avoid duplications
            errors = list(filter(None, e.errors))
            self._errors.extend(sorted(set(errors), key=errors.index))

    def validate_tags(self):
        '''Validate tags for duplicates'''
        tags = self.manifest_tree.get('tags', [])
        tags = [str(tag).lower() for tag in tags]
        dupes = set([tag for tag in tags if tags.count(tag) > 1])
        if dupes:
            self.add_error('Some tags are more than once in the manifest: %s' % ', '.join(dupes))

    def validate_normalize(self):
        self.validate_normalize_schema()
        self.validate_root_keys()
        self.validate_normalize_dependencies()
        self.validate_targets()
        self.validate_normalize_required_keys()
        self.validate_files()
        self.validate_tags()
        return self._errors
