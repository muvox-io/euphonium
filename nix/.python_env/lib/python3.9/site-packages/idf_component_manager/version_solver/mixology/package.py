try:
    from typing import Optional
except ImportError:
    pass

from idf_component_tools.sources import BaseSource


class Package(object):
    """
    A project's package.
    """
    ROOT_PACKAGE_NAME = '_root_'

    def __init__(self, name, source=None):  # type: (str, Optional[BaseSource]) -> None
        self._name = name
        self._source = source

    @classmethod
    def root(cls):  # type: () -> Package
        return Package(cls.ROOT_PACKAGE_NAME)

    @property
    def name(self):  # type: () -> str
        return self._name

    @property
    def source(self):  # type: () -> BaseSource
        return self._source

    def __eq__(self, other):  # type: (Package) -> bool
        if not isinstance(other, Package):
            return NotImplemented

        return self.name == other.name and self.source == other.source

    def __ne__(self, other):  # type: (Package) -> bool
        if not isinstance(other, Package):
            return NotImplemented

        return not (self == other)

    def __str__(self):  # type: () -> str
        return self._name

    def __repr__(self):  # type: () -> str
        return 'Package("{}" {})'.format(self.name, self.source)

    def __hash__(self):
        return hash(self.name)
