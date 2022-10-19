"""Results of the solver"""

from idf_component_tools.serialization import serializable

from ..errors import LockError
from ..manifest import ComponentRequirement, ComponentVersion
from ..sources.base import BaseSource

try:
    from typing import Iterable, Optional
except ImportError:
    pass


@serializable
class SolvedComponent(object):
    _serialization_properties = [
        'component_hash',
        'name',
        'source',
        'version',
    ]

    def __init__(
            self,
            name,  # type: str
            version,  # type: ComponentVersion
            source,  # type: BaseSource
            component_hash=None,  # type: Optional[str]
            dependencies=None,  # type: Optional[Iterable[ComponentRequirement]]
    ):
        # type: (...) -> None
        self.name = name
        self.version = version
        self.source = source
        self.component_hash = component_hash

        if dependencies is None:
            dependencies = []
        self.dependencies = dependencies

    def __str__(self):
        return 'SolvedComponent: %s %s %s' % (self.name, self.version, self.component_hash)

    @classmethod
    def fromdict(cls, details):
        try:
            source_details = dict(details['source'])
            source_name = source_details.pop('type')
            source = BaseSource.fromdict(source_name, source_details)
            component_hash = details.get('component_hash', None)
            if source.component_hash_required and not component_hash:
                raise LockError(
                    '"component_hash" is required for component "%s" in the "dependencies.lock" file' % details['name'])

            return cls(
                name=source.normalized_name(details['name']),
                version=ComponentVersion(details['version']),
                source=source,
                component_hash=component_hash)
        except KeyError as e:
            raise LockError(
                'Cannot parse dependencies lock file. Required field %s is not found for component "%s"' %
                (str(e), details['name']))
