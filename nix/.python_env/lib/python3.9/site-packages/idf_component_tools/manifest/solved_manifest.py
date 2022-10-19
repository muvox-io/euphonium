from .solved_component import SolvedComponent

try:
    from typing import Dict, List, Optional
except ImportError:
    pass


class SolvedManifest(object):
    def __init__(
            self,
            solved_components,
            manifest_hash,
            target=None):  # type: (Optional[List[SolvedComponent]], str, Optional[str]) -> None
        if solved_components is None:
            solved_components = []
        solved_components.sort(key=lambda c: c.name)
        self.dependencies = solved_components
        self.target = target
        self.manifest_hash = manifest_hash

    @classmethod
    def fromdict(cls, lock):  # type: (Dict) -> SolvedManifest
        solved_components = []
        for name, component in lock.get('dependencies', {}).items():
            component['name'] = name
            solved_components.append(SolvedComponent.fromdict(component))

        return cls(
            solved_components,
            manifest_hash=lock['manifest_hash'],
            target=lock.get('target'),
        )

    def serialize(self):
        dependencies = {}
        for dependency in self.dependencies:
            dep_dict = dependency.serialize()
            name = dep_dict.pop('name')
            dependencies[name] = dep_dict

        solution = {
            'manifest_hash': self.manifest_hash,
            'target': self.target,
        }

        if dependencies:
            solution['dependencies'] = dependencies

        return solution
