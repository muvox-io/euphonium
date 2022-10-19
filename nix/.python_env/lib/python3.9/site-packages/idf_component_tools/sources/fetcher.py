"""Small class that manages getting components to right path using system-wide cache"""

import os
from io import open

from ..build_system_tools import build_name
from ..errors import ComponentModifiedError, InvalidComponentHashError
from ..hash_tools import (
    HASH_FILENAME, HashDoesNotExistError, HashNotEqualError, HashNotSHA256Error, validate_dir_with_hash_file)
from ..manifest import SolvedComponent

try:
    from typing import TYPE_CHECKING, List

    if TYPE_CHECKING:
        from . import BaseSource
except ImportError:
    pass


class ComponentFetcher(object):
    def __init__(
        self,
        solved_component,
        components_path,
        source=None,
    ):  # type: (SolvedComponent, str, BaseSource) -> None
        self.source = source if source else solved_component.source
        self.component = solved_component
        self.components_path = components_path
        self.managed_path = os.path.join(self.components_path, build_name(self.component.name))

    def download(self):  # type: () -> List[str]
        """If necessary, it downloads component and returns local path to component directory"""
        try:
            validate_dir_with_hash_file(self.managed_path)
        except HashNotEqualError:
            raise ComponentModifiedError(
                'Component directory was modified on the disk since the last run of '
                'the CMake')
        except HashNotSHA256Error:
            raise InvalidComponentHashError(
                'File .component_hash for component "{}" in the managed '
                'components directory cannot be parsed. This file is used by the '
                'component manager for component integrity checks. If this file '
                'exists in the component source, please ask the component '
                'maintainer to remove it.'.format(self.component.name))
        except HashDoesNotExistError:
            pass

        return self.source.download(self.component, self.managed_path)

    def create_hash(self, paths, component_hash):  # type: (list[str], None | str) -> None
        if self.component.source.downloadable:
            hash_file = os.path.join(paths[0], HASH_FILENAME)

            if not os.path.isfile(hash_file):
                with open(hash_file, mode='w+', encoding='utf-8') as f:
                    f.write(u'{}'.format(component_hash))
