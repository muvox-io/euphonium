"""Set of tools and constants to work with files and directories """
import os
import shutil
from pathlib import Path
from shutil import copytree, rmtree

try:
    from typing import Callable, Iterable, Optional, Set, Text, Union
except ImportError:
    pass

DEFAULT_EXCLUDE = [
    # Python files
    '**/__pycache__',
    '**/*.pyc',
    '**/*.pyd',
    '**/*.pyo',
    # macOS files
    '**/.DS_Store',
    # Git
    '**/.git/**/*',
    # dist and build artefacts
    './dist/**/*',
    'build/**/*',
    # CI files
    '.github/**/*',
    '.gitlab-ci.yml',
    # IDE files
    '.idea/**/*',
    '.vscode/**/*',
    # Configs
    '.settings/**/*',
    '**/sdkconfig',
    # Hash file
    '.component_hash'
]


def filtered_paths(path, include=None, exclude=None):
    # type: (Union[Text, Path], Optional[Iterable[str]], Optional[Iterable[str]]) -> Set[Path]
    if include is None:
        include = set()

    if exclude is None:
        exclude = set()

    base_path = Path(path)
    paths = set()  # type: Set[Path]

    def include_paths(pattern):
        paths.update(base_path.glob(pattern))

    def exclude_paths(pattern):
        paths.difference_update(base_path.glob(pattern))

    # First include everything
    include_paths('**/*')

    # Exclude all defaults, including directories
    for pattern in DEFAULT_EXCLUDE:
        exclude_paths(pattern)
        if pattern.endswith('/**/*'):
            exclude_paths(pattern[:pattern.rindex('/**/*')])

    # Exclude user patterns
    for pattern in exclude:
        exclude_paths(pattern)

    # Include everything that was explicitly added
    for pattern in include:
        include_paths(pattern)

    return paths


def filter_builder(paths):  # type: (Iterable[Path]) -> Callable[[Path], bool]
    def filter_path(path):  # type: (Path) -> bool
        '''Returns True if path should be included, False otherwise'''
        return path.resolve() in paths

    return filter_path


def create_directory(directory):  # type: (str) -> None
    """Create directory, if doesn't exist yet"""
    if not os.path.exists(directory):
        os.makedirs(directory)


def prepare_empty_directory(directory):  # type: (str) -> None
    """Prepare directory empty"""
    dir_exist = os.path.exists(directory)

    # Delete path if it's not empty
    if dir_exist and os.listdir(directory):
        rmtree(directory)
        dir_exist = False

    if not dir_exist:
        os.makedirs(directory)


def copy_directory(source_directory, destination_directory):  # type: (str, str) -> None
    if os.path.exists(destination_directory):
        rmtree(destination_directory)
    copytree(source_directory, destination_directory)


def copy_filtered_directory(source_directory, destination_directory, include=None, exclude=None):
    # type: (str, str, Optional[Iterable[str]], Optional[Iterable[str]]) -> None
    paths = filtered_paths(source_directory, include=include, exclude=exclude)
    prepare_empty_directory(destination_directory)

    for path in sorted(paths):
        path = str(path)  # type: ignore # Path backward compatibility
        rel_path = os.path.relpath(path, source_directory)
        dest_path = os.path.join(destination_directory, rel_path)

        if os.path.isfile(path):
            dest_dir = os.path.dirname(dest_path)

            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)

            shutil.copy2(path, dest_path)
        else:
            os.makedirs(dest_path)
