from idf_component_tools.manifest import ComponentRequirement, ProjectRequirements, SolvedComponent, SolvedManifest

from .helper import PackageSource
from .mixology.package import Package
from .mixology.version_solver import VersionSolver as Solver

try:
    from typing import Optional
except ImportError:
    pass


class VersionSolver(object):
    """
    The version solver that finds a set of package versions
    that satisfy the root package's dependencies.
    """
    def __init__(
            self, requirements, old_solution=None):  # type: (ProjectRequirements, Optional[SolvedManifest]) -> None
        self.requirements = requirements
        self.old_solution = old_solution
        self._source = PackageSource()
        self._solver = Solver(self._source)
        self._target = None
        self._overriders = set()  # type: set[str]

    def solve(self):  # type: () -> SolvedManifest
        for manifest in self.requirements.manifests:
            self.solve_manifest(manifest)

        self._source.override_dependencies(self._overriders)

        result = self._solver.solve()

        solved_components = []
        for package, version in result.decisions.items():
            if package == Package.root():
                continue
            kwargs = {'name': package.name, 'source': package.source, 'version': version}
            if package.source.component_hash_required:
                kwargs['component_hash'] = version.component_hash
            solved_components.append(SolvedComponent(**kwargs))  # type: ignore
        return SolvedManifest(solved_components, self.requirements.manifest_hash, self.requirements.target)

    def solve_manifest(self, manifest):
        for requirement in manifest.dependencies:  # type: ComponentRequirement
            self._source.root_dep(Package(requirement.name, requirement.source), requirement.version_spec)
            self.solve_component(requirement)

    def solve_component(self, requirement):  # type: (ComponentRequirement) -> None
        cmp_with_versions = requirement.source.versions(
            name=requirement.name, spec=requirement.version_spec, target=self.requirements.target)

        for version in cmp_with_versions.versions:
            if requirement.source.is_overrider:
                self._overriders.add(requirement.name)

            self._source.add(
                Package(requirement.name, requirement.source),
                version,
                deps={Package(req.name, req.source): req.version_spec
                      for req in version.dependencies} if version.dependencies else {})

            if version.dependencies:
                for dep in version.dependencies:
                    self.solve_component(dep)
