from idf_component_tools.manifest import HashedComponentVersion

try:
    from typing import Any, Dict, List, Optional
    from typing import Union as _Union
except ImportError:
    pass

from idf_component_tools.semver import Range as SemverRange
from idf_component_tools.semver import SimpleSpec

from .mixology.constraint import Constraint
from .mixology.package import Package
from .mixology.package_source import PackageSource as BasePackageSource
from .mixology.range import Range
from .mixology.union import Union


def parse_constraint(spec):  # type: (str) -> _Union[Union, Range]
    try:
        clause = SimpleSpec(spec).clause
    except ValueError:  # if not semspec, expect an exact version
        constraint = parse_single_constraint(HashedComponentVersion(spec))
    else:
        if isinstance(clause, SemverRange):  # single range
            constraint = parse_single_constraint(clause)
        else:  # multi ranges
            ranges = [parse_single_constraint(_range) for _range in clause.clauses]
            constraint = ranges[0]
            for r in ranges[1:]:
                constraint = constraint.intersect(r)

    return constraint


def parse_single_constraint(_range):  # type: (_Union[SemverRange, HashedComponentVersion]) -> _Union[Union, Range]
    if isinstance(_range, HashedComponentVersion):  # not semver
        return Range(_range, _range, True, True, _range.text)

    version = HashedComponentVersion(str(_range.target))
    if _range.operator == _range.OP_LT:
        return Range(max=version, string=str(_range))
    elif _range.operator == _range.OP_LTE:
        return Range(max=version, include_max=True, string=str(_range))
    elif _range.operator == _range.OP_GT:
        return Range(min=version, string=str(_range))
    elif _range.operator == _range.OP_GTE:
        return Range(min=version, include_min=True, string=str(_range))
    elif _range.operator == _range.OP_NEQ:
        return Union(Range(min=version, string=str(_range)), Range(max=version, string=str(_range)))
    else:
        return Range(version, version, True, True, str(_range))


class Dependency:
    def __init__(self, package, spec):  # type: (Package, str) -> None
        self.package = package
        self.constraint = parse_constraint(spec)
        self.text = spec


class PackageSource(BasePackageSource):
    def __init__(self):  # type: () -> None
        self._root_version = HashedComponentVersion('0.0.0')
        self._root_dependencies = []  # type: List[Dependency]
        self._packages = {}  # type: Dict[Package, Dict[HashedComponentVersion, List[Dependency]]]

        super(PackageSource, self).__init__()

    @property
    def root_version(self):
        return self._root_version

    def add(
            self,
            package,
            version,
            deps=None):  # type: (Package, _Union[str, HashedComponentVersion],  Optional[Dict[Package, str]]) -> None
        if deps is None:
            deps = {}

        if not isinstance(version, HashedComponentVersion):
            version = HashedComponentVersion(version)

        if package not in self._packages:
            self._packages[package] = {}

        if version in self._packages[package]:
            return

        dependencies = []
        for dep_package, spec in deps.items():
            dependencies.append(Dependency(dep_package, spec))

        self._packages[package][version] = dependencies

    def override_dependencies(self, overriders):  # type: (set[str]) -> None
        for package in list(self._packages.keys()):
            if not package.source.is_overrider and package.name in overriders:
                del self._packages[package]

        for package in self._packages.keys():
            for version in self._packages[package].keys():
                self._packages[package][version] = [
                    elem for elem in self._packages[package][version]
                    if elem.package.source.is_overrider or elem.package.name not in overriders
                ]

    def root_dep(self, package, spec):  # type: (Package, str) -> None
        self._root_dependencies.append(Dependency(package, spec))

    def _versions_for(self, package, constraint=None):  # type: (Package, Any) -> List[HashedComponentVersion]
        if package not in self._packages:
            return []

        versions = []
        for version in self._packages[package].keys():
            if not constraint or constraint.allows_any(Range(version, version, True, True)):
                versions.append(version)

        return sorted(versions, reverse=True)

    def dependencies_for(self, package, version):  # type: (Package, Any) -> List[Any]
        if package == self.root:
            return self._root_dependencies

        return self._packages[package][version]

    def convert_dependency(self, dependency):  # type: (Dependency) -> Constraint
        if isinstance(dependency.constraint, Range):
            constraint = Range(
                dependency.constraint.min,
                dependency.constraint.max,
                dependency.constraint.include_min,
                dependency.constraint.include_max,
                dependency.text,
            )
        elif isinstance(dependency.constraint, Union):
            ranges = [
                Range(
                    r.min,
                    r.max,
                    r.include_min,
                    r.include_max,
                    str(r),
                ) for r in dependency.constraint.ranges
            ]
            constraint = Union.of(ranges)
        else:
            raise ValueError('should be "Union" or "Range"')

        return Constraint(dependency.package, constraint)
