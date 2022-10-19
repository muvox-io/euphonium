try:
    from typing import Any, Optional
except ImportError:
    pass

from .constraint import Constraint
from .incompatibility import Incompatibility
from .package import Package
from .range import Range
from .term import Term


class Assignment(Term):
    """
    A term in a PartialSolution that tracks some additional metadata.
    """
    def __init__(
            self,
            constraint,
            is_positive,
            decision_level,
            index,
            cause=None):  # type: (Constraint, bool, int, int, Optional[Incompatibility]) -> None
        super(Assignment, self).__init__(constraint, is_positive)

        self._decision_level = decision_level
        self._index = index
        self._cause = cause

    @property
    def decision_level(self):  # type: () -> int
        return self._decision_level

    @property
    def index(self):  # type: () -> int
        return self._index

    @property
    def cause(self):  # type: () -> Incompatibility
        return self._cause

    @classmethod
    def decision(cls, package, version, decision_level, index):  # type: (Package, Any, int, int) -> Assignment
        return cls(
            Constraint(package, Range(version, version, True, True)),
            True,
            decision_level,
            index,
        )

    @classmethod
    def derivation(
            cls, constraint, is_positive, cause, decision_level,
            index):  # type: (Constraint, bool, Incompatibility, int, int) -> Assignment
        return cls(constraint, is_positive, decision_level, index, cause)

    def is_decision(self):  # type: () -> bool
        return self._cause is None
