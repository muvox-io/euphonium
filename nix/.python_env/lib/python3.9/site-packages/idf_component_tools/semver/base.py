# -*- coding: utf-8 -*-
# Copyright (c) The python-semanticversion project
# This code is distributed under the two-clause BSD License.

import functools
import re


def _has_leading_zero(value):
    return value and value[0] == '0' and value.isdigit() and value != '0'


class MaxIdentifier(object):
    __slots__ = []  # type: list[str]

    def __repr__(self):
        return 'MaxIdentifier()'

    def __eq__(self, other):
        return isinstance(other, self.__class__)


@functools.total_ordering
class NumericIdentifier(object):
    __slots__ = ['value']

    def __init__(self, value):
        self.value = int(value)

    def __repr__(self):
        return 'NumericIdentifier(%r)' % self.value

    def __eq__(self, other):
        if isinstance(other, NumericIdentifier):
            return self.value == other.value
        return NotImplemented

    def __lt__(self, other):
        if isinstance(other, MaxIdentifier):
            return True
        elif isinstance(other, AlphaIdentifier):
            return True
        elif isinstance(other, NumericIdentifier):
            return self.value < other.value
        else:
            return NotImplemented


@functools.total_ordering
class AlphaIdentifier(object):
    __slots__ = ['value']

    def __init__(self, value):
        self.value = value.encode('ascii')

    def __repr__(self):
        return 'AlphaIdentifier(%r)' % self.value

    def __eq__(self, other):
        if isinstance(other, AlphaIdentifier):
            return self.value == other.value
        return NotImplemented

    def __lt__(self, other):
        if isinstance(other, MaxIdentifier):
            return True
        elif isinstance(other, NumericIdentifier):
            return False
        elif isinstance(other, AlphaIdentifier):
            return self.value < other.value
        else:
            return NotImplemented


class Version(object):
    version_re = re.compile(r'^(\d+)\.(\d+)\.(\d+)(?:~(\d+))?(?:-([0-9a-zA-Z.-]+))?(?:\+([0-9a-zA-Z.-]+))?$')

    def __init__(
            self, version_string=None, major=None, minor=None, patch=None, revision=None, prerelease=None, build=None):
        has_text = version_string is not None
        has_parts = not (major is minor is patch is prerelease is build is None)
        if not has_text ^ has_parts:
            raise ValueError("Call either Version('1.2.3') or Version(major=1, ...).")

        if has_text:
            major, minor, patch, revision, prerelease, build = self.parse(version_string)
        else:
            revision = revision or 0
            # Convenience: allow to omit prerelease/build.
            prerelease = tuple(prerelease or ())
            build = tuple(build or ())
            self._validate_kwargs(major, minor, patch, revision, prerelease, build)

        self.major = major
        self.minor = minor
        self.patch = patch
        self.revision = revision
        self.prerelease = prerelease
        self.build = build

    @classmethod
    def _coerce(cls, value, allow_none=False):
        if value is None and allow_none:
            return value
        return int(value)

    def next_major(self):
        return Version(
            major=self.major + 1,
            minor=0,
            patch=0,
        )

    def next_minor(self):
        return Version(
            major=self.major,
            minor=self.minor + 1,
            patch=0,
        )

    def next_patch(self):
        return Version(
            major=self.major,
            minor=self.minor,
            patch=self.patch + 1,
        )

    def truncate(self, level='patch'):
        """Return a new Version object, truncated up to the selected level."""
        if level == 'build':
            return self
        elif level == 'prerelease':
            return Version(
                major=self.major,
                minor=self.minor,
                patch=self.patch,
                revision=self.revision,
                prerelease=self.prerelease,
            )
        elif level == 'patch':
            return Version(
                major=self.major,
                minor=self.minor,
                patch=self.patch,
            )
        elif level == 'minor':
            return Version(
                major=self.major,
                minor=self.minor,
                patch=0,
            )
        elif level == 'major':
            return Version(
                major=self.major,
                minor=0,
                patch=0,
            )
        else:
            raise ValueError('Invalid truncation level `%s`.' % level)

    @classmethod
    def coerce(cls, version_string):
        """Coerce an arbitrary version string into a semver-compatible one.

        The rule is:
        - If not enough components, fill minor/patch with zeroes
        - If more than 3 dot-separated components, extra components are "build"
          data. If some "build" data already appeared, append it to the
          extra components

        Examples:
            >>> Version.coerce('0.1')
            Version(0, 1, 0, 1)
            >>> Version.coerce('0.1.2.3')
            Version(0, 1, 2, 1, (), ('3',))
            >>> Version.coerce('0.1.2.3+4')
            Version(0, 1, 2, 1, (), ('3', '4'))
            >>> Version.coerce('0.1+2-3+4_5')
            Version(0, 1, 0, 1, (), ('2-3', '4-5'))
            >>> Version.coerce('0.1~2')
            Version(0, 1, 0, 2)
        """
        base_re = re.compile(r'^\d+(?:\.\d+(?:\.\d+)?)?(?:~\d+)?')

        match = base_re.match(version_string)
        if not match:
            raise ValueError('Version string lacks a numerical component: %r' % version_string)

        version = version_string[:match.end()]
        if '~' in version:
            version, revision = version.split('~')
        else:
            version, revision = version, '0'
        revision = int(revision)

        while version.count('.') < 2:
            version += '.0'

        # Strip leading zeros in components
        # Version is of the form nn, nn.pp or nn.pp.qq
        version = '.'.join(
            # If the part was '0', we end up with an empty string.
            part.lstrip('0') or '0' for part in version.split('.'))

        if match.end() == len(version_string):
            return Version(version, revision=revision)

        rest = version_string[match.end():]

        # Cleanup the 'rest'
        rest = re.sub(r'[^a-zA-Z0-9+.-]', '-', rest)

        if rest[0] == '+':
            # A 'build' component
            prerelease = ''
            build = rest[1:]
        elif rest[0] == '.':
            # An extra version component, probably 'build'
            prerelease = ''
            build = rest[1:]
        elif rest[0] == '-':
            rest = rest[1:]
            if '+' in rest:
                prerelease, build = rest.split('+', 1)
            else:
                prerelease, build = rest, ''
        elif '+' in rest:
            prerelease, build = rest.split('+', 1)
        else:
            prerelease, build = rest, ''

        build = build.replace('+', '.')

        if prerelease:
            version = '%s-%s' % (version, prerelease)
        if build:
            version = '%s+%s' % (version, build)

        return cls(version, revision=revision)

    @classmethod
    def parse(cls, version_string):
        """Parse a version string into a tuple of components:
           (major, minor, patch, revision, prerelease, build).

        Args:
            version_string (str), the version string to parse
        """
        if not version_string:
            raise ValueError('Invalid empty version string: %r' % version_string)

        version_re = cls.version_re

        match = version_re.match(version_string)
        if not match:
            raise ValueError('Invalid version string: %r' % version_string)

        major, minor, patch, revision, prerelease, build = match.groups()

        if _has_leading_zero(major):
            raise ValueError('Invalid leading zero in major: %r' % version_string)
        if _has_leading_zero(minor):
            raise ValueError('Invalid leading zero in minor: %r' % version_string)
        if _has_leading_zero(patch):
            raise ValueError('Invalid leading zero in patch: %r' % version_string)

        major = int(major)
        minor = cls._coerce(minor)
        patch = cls._coerce(patch)
        revision = int(revision) if revision else 0
        if revision < 0:
            raise ValueError('Invalid revision number, must be an integer greater than or equal to 0')

        if not prerelease:
            prerelease = ()
        else:
            prerelease = tuple(prerelease.split('.'))
            cls._validate_identifiers(prerelease, allow_leading_zeroes=False)

        if not build:
            build = ()
        else:
            build = tuple(build.split('.'))
            cls._validate_identifiers(build, allow_leading_zeroes=True)

        return major, minor, patch, revision, prerelease, build

    @classmethod
    def _validate_identifiers(cls, identifiers, allow_leading_zeroes=False):
        for item in identifiers:
            if not item:
                raise ValueError('Invalid empty identifier %r in %r' % (item, '.'.join(identifiers)))

            if item[0] == '0' and item.isdigit() and item != '0' and not allow_leading_zeroes:
                raise ValueError('Invalid leading zero in identifier %r' % item)

    @classmethod
    def _validate_kwargs(cls, major, minor, patch, revision, prerelease, build):
        if (major != int(major) or minor != cls._coerce(minor) or patch != cls._coerce(patch)
                or revision != int(revision)):
            raise ValueError(
                'Invalid kwargs to Version(major=%r, minor=%r, patch=%r, revision=%r, '
                'prerelease=%r, build=%r' % (major, minor, patch, revision, prerelease, build))
        if prerelease is not None:
            cls._validate_identifiers(prerelease, allow_leading_zeroes=False)
        if build is not None:
            cls._validate_identifiers(build, allow_leading_zeroes=True)

    def __iter__(self):
        return iter((self.major, self.minor, self.patch, self.revision, self.prerelease, self.build))

    def __str__(self):
        version = '%d' % self.major
        if self.minor is not None:
            version = '%s.%d' % (version, self.minor)
        if self.patch is not None:
            version = '%s.%d' % (version, self.patch)
        if self.revision:
            version = '%s~%d' % (version, self.revision)
        if self.prerelease:
            version = '%s-%s' % (version, '.'.join(self.prerelease))
        if self.build:
            version = '%s+%s' % (version, '.'.join(self.build))
        return version

    def __repr__(self):
        return '%s(%r, revision=%d)' % (
            self.__class__.__name__,
            str(self),
            self.revision,
        )

    def __hash__(self):
        return hash((self.major, self.minor, self.patch, self.revision, self.prerelease, self.build))

    @property
    def precedence_key(self):
        if self.prerelease:
            prerelease_key = tuple(
                NumericIdentifier(part) if re.match(r'^[0-9]+$', part) else AlphaIdentifier(part)
                for part in self.prerelease)
        else:
            prerelease_key = (MaxIdentifier(), )

        return (
            self.major,
            self.minor,
            self.patch,
            self.revision,
            prerelease_key,
        )

    def __cmp__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        if self < other:
            return -1
        elif self > other:
            return 1
        elif self == other:
            return 0
        else:
            return NotImplemented

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return (
            self.major == other.major and self.minor == other.minor and self.patch == other.patch
            and (self.revision == other.revision) and (self.prerelease or ()) == (other.prerelease or ())
            and (self.build or ()) == (other.build or ()))

    def __ne__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return tuple(self) != tuple(other)

    def __lt__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.precedence_key < other.precedence_key

    def __le__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.precedence_key <= other.precedence_key

    def __gt__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.precedence_key > other.precedence_key

    def __ge__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.precedence_key >= other.precedence_key


def compare(v1, v2):
    return Version(v1).__cmp__(Version(v2))


def match(spec, version):
    return SimpleSpec(spec).match(Version(version))


def validate(version_string):
    """Validates a version string againt the SemVer specification."""
    try:
        Version.parse(version_string)
        return True
    except ValueError:
        return False


DEFAULT_SYNTAX = 'simple'


class BaseSpec(object):
    """A specification of compatible versions.

    Usage:
    >>> Spec('>=1.0.0', syntax='npm')

    A version matches a specification if it matches any
    of the clauses of that specification.

    Internally, a Spec is AnyOf(
        AllOf(Matcher, Matcher, Matcher),
        AllOf(...),
    )
    """
    def __init__(self, expression):
        super(BaseSpec, self).__init__()
        self.expression = expression
        self.clause = self._parse_to_clause(expression)

    @classmethod
    def parse(cls, expression):
        """Convert a syntax-specific expression into a BaseSpec instance."""
        return cls(expression)

    @classmethod
    def _parse_to_clause(cls, expression):
        """Converts an expression to a clause."""
        raise NotImplementedError()

    def filter(self, versions):
        """Filter an iterable of versions satisfying the Spec."""
        for version in versions:
            if self.match(version):
                yield version

    def match(self, version):
        """Check whether a Version satisfies the Spec."""
        return self.clause.match(version)

    def select(self, versions):
        """Select the best compatible version among an iterable of options."""
        options = list(self.filter(versions))
        if options:
            return max(options)
        return None

    def __contains__(self, version):
        """Whether `version in self`."""
        if isinstance(version, Version):
            return self.match(version)
        return False

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented

        return self.clause == other.clause

    def __hash__(self):
        return hash(self.clause)

    def __str__(self):
        return self.expression

    def __repr__(self):
        return '<%s: %r>' % (self.__class__.__name__, self.expression)


class Clause(object):
    __slots__ = []  # type: list[str]

    def match(self, version):
        raise NotImplementedError()

    def __and__(self, other):
        raise NotImplementedError()

    def __or__(self, other):
        raise NotImplementedError()

    def __eq__(self, other):
        raise NotImplementedError()

    def prettyprint(self, indent='\t'):
        """Pretty-print the clause.
        """
        return '\n'.join(self._pretty()).replace('\t', indent)

    def _pretty(self):
        """Actual pretty-printing logic.

        Yields:
            A list of string. Indentation is performed with \t.
        """
        yield repr(self)

    def __ne__(self, other):
        return not self == other

    def simplify(self):
        return self


class AnyOf(Clause):
    __slots__ = ['clauses']

    def __init__(self, *clauses):
        super(AnyOf, self).__init__()
        self.clauses = frozenset(clauses)

    def match(self, version):
        return any(c.match(version) for c in self.clauses)

    def simplify(self):
        subclauses = set()
        for clause in self.clauses:
            simplified = clause.simplify()
            if isinstance(simplified, AnyOf):
                subclauses |= simplified.clauses
            elif simplified == Never():
                continue
            else:
                subclauses.add(simplified)
        if len(subclauses) == 1:
            return subclauses.pop()
        return AnyOf(*subclauses)

    def __hash__(self):
        return hash((AnyOf, self.clauses))

    def __iter__(self):
        return iter(self.clauses)

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.clauses == other.clauses

    def __and__(self, other):
        if isinstance(other, AllOf):
            return other & self
        elif isinstance(other, Matcher) or isinstance(other, AnyOf):
            return AllOf(self, other)
        else:
            return NotImplemented

    def __or__(self, other):
        if isinstance(other, AnyOf):
            clauses = list(self.clauses | other.clauses)
        elif isinstance(other, Matcher) or isinstance(other, AllOf):
            clauses = list(self.clauses | set([other]))
        else:
            return NotImplemented
        return AnyOf(*clauses)

    def __repr__(self):
        return 'AnyOf(%s)' % ', '.join(sorted(repr(c) for c in self.clauses))

    def _pretty(self):
        yield 'AnyOF('
        for clause in self.clauses:
            lines = list(clause._pretty())
            for line in lines[:-1]:
                yield '\t' + line
            yield '\t' + lines[-1] + ','
        yield ')'


class AllOf(Clause):
    __slots__ = ['clauses']

    def __init__(self, *clauses):
        super(AllOf, self).__init__()
        self.clauses = frozenset(clauses)

    def match(self, version):
        return all(clause.match(version) for clause in self.clauses)

    def simplify(self):
        subclauses = set()
        for clause in self.clauses:
            simplified = clause.simplify()
            if isinstance(simplified, AllOf):
                subclauses |= simplified.clauses
            elif simplified == Always():
                continue
            else:
                subclauses.add(simplified)
        if len(subclauses) == 1:
            return subclauses.pop()
        return AllOf(*subclauses)

    def __hash__(self):
        return hash((AllOf, self.clauses))

    def __iter__(self):
        return iter(self.clauses)

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.clauses == other.clauses

    def __and__(self, other):
        if isinstance(other, Matcher) or isinstance(other, AnyOf):
            clauses = list(self.clauses | set([other]))
        elif isinstance(other, AllOf):
            clauses = list(self.clauses | other.clauses)
        else:
            return NotImplemented
        return AllOf(*clauses)

    def __or__(self, other):
        if isinstance(other, AnyOf):
            return other | self
        elif isinstance(other, Matcher):
            return AnyOf(self, AllOf(other))
        elif isinstance(other, AllOf):
            return AnyOf(self, other)
        else:
            return NotImplemented

    def __repr__(self):
        return 'AllOf(%s)' % ', '.join(sorted(repr(c) for c in self.clauses))

    def _pretty(self):
        yield 'AllOF('
        for clause in self.clauses:
            lines = list(clause._pretty())
            for line in lines[:-1]:
                yield '\t' + line
            yield '\t' + lines[-1] + ','
        yield ')'


class Matcher(Clause):
    __slots__ = []  # type: list[str]

    def __and__(self, other):
        if isinstance(other, AllOf):
            return other & self
        elif isinstance(other, Matcher) or isinstance(other, AnyOf):
            return AllOf(self, other)
        else:
            return NotImplemented

    def __or__(self, other):
        if isinstance(other, AnyOf):
            return other | self
        elif isinstance(other, Matcher) or isinstance(other, AllOf):
            return AnyOf(self, other)
        else:
            return NotImplemented


class Never(Matcher):
    __slots__ = []  # type: list[str]

    def match(self, version):
        return False

    def __hash__(self):
        return hash((Never, ))

    def __eq__(self, other):
        return isinstance(other, self.__class__)

    def __and__(self, other):
        return self

    def __or__(self, other):
        return other

    def __repr__(self):
        return 'Never()'


class Always(Matcher):
    __slots__ = []  # type: list[str]

    def match(self, version):
        return True

    def __hash__(self):
        return hash((Always, ))

    def __eq__(self, other):
        return isinstance(other, self.__class__)

    def __and__(self, other):
        return other

    def __or__(self, other):
        return self

    def __repr__(self):
        return 'Always()'


class Range(Matcher):
    OP_EQ = '=='
    OP_GT = '>'
    OP_GTE = '>='
    OP_LT = '<'
    OP_LTE = '<='
    OP_NEQ = '!='

    # <1.2.3 matches 1.2.3-a1
    PRERELEASE_ALWAYS = 'always'
    # <1.2.3 does not match 1.2.3-a1
    PRERELEASE_NATURAL = 'natural'
    # 1.2.3-a1 is only considered if target == 1.2.3-xxx
    PRERELEASE_SAMEPATCH = 'same-patch'

    # 1.2.3 matches 1.2.3+*
    BUILD_IMPLICIT = 'implicit'
    # 1.2.3 matches only 1.2.3, not 1.2.3+4
    BUILD_STRICT = 'strict'

    __slots__ = ['operator', 'target', 'prerelease_policy', 'build_policy']

    def __init__(self, operator, target, prerelease_policy=PRERELEASE_NATURAL, build_policy=BUILD_IMPLICIT):
        super(Range, self).__init__()
        if target.build and operator not in (self.OP_EQ, self.OP_NEQ):
            raise ValueError('Invalid range %s%s: build numbers have no ordering.' % (operator, target))
        self.operator = operator
        self.target = target
        self.prerelease_policy = prerelease_policy
        self.build_policy = self.BUILD_STRICT if target.build else build_policy

    def match(self, version):
        if self.build_policy != self.BUILD_STRICT:
            version = version.truncate('prerelease')

        if version.prerelease:
            same_patch = self.target.truncate() == version.truncate()

            if self.prerelease_policy == self.PRERELEASE_SAMEPATCH and not same_patch:
                return False

        if self.operator == self.OP_EQ:
            if self.build_policy == self.BUILD_STRICT:
                return (
                    self.target.truncate('prerelease') == version.truncate('prerelease')
                    and version.build == self.target.build)
            return version == self.target
        elif self.operator == self.OP_GT:
            return version > self.target
        elif self.operator == self.OP_GTE:
            return version >= self.target
        elif self.operator == self.OP_LT:
            if (version.prerelease and self.prerelease_policy == self.PRERELEASE_NATURAL
                    and version.truncate() == self.target.truncate() and not self.target.prerelease):
                return False
            return version < self.target
        elif self.operator == self.OP_LTE:
            return version <= self.target
        else:
            assert self.operator == self.OP_NEQ
            if self.build_policy == self.BUILD_STRICT:
                return not (
                    self.target.truncate('prerelease') == version.truncate('prerelease')
                    and version.build == self.target.build)

            if (version.prerelease and self.prerelease_policy == self.PRERELEASE_NATURAL
                    and version.truncate() == self.target.truncate() and not self.target.prerelease):
                return False
            return version != self.target

    def __hash__(self):
        return hash((Range, self.operator, self.target, self.prerelease_policy))

    def __eq__(self, other):
        return (
            isinstance(other, self.__class__) and self.operator == other.operator and self.target == other.target
            and self.prerelease_policy == other.prerelease_policy)

    def __str__(self):
        return '%s%s' % (self.operator, self.target)

    def __repr__(self):
        policy_part = (
            '' if self.prerelease_policy == self.PRERELEASE_NATURAL else ', prerelease_policy=%r' %
            self.prerelease_policy) + (
                '' if self.build_policy == self.BUILD_IMPLICIT else ', build_policy=%r' % self.build_policy)
        return 'Range(%r, %r%s)' % (
            self.operator,
            self.target,
            policy_part,
        )


class SimpleSpec(BaseSpec):
    @classmethod
    def _parse_to_clause(cls, expression):
        return cls.Parser.parse(expression)

    class Parser:
        NUMBER = r'\*|0|[1-9][0-9]*'
        NAIVE_SPEC = re.compile(
            r"""^
            (?P<op><|<=||=|==|>=|>|!=|\^|~|~=)
            (?P<major>{nb})(?:\.(?P<minor>{nb})(?:\.(?P<patch>{nb}))?)?
            (?:~(?P<revision>\d+))?
            (?:-(?P<prerel>[a-z0-9A-Z.-]*))?
            (?:\+(?P<build>[a-z0-9A-Z.-]*))?
            $
            """.format(nb=NUMBER),
            re.VERBOSE,
        )

        @classmethod
        def parse(cls, expression):
            blocks = expression.split(',')
            clause = Always()
            for block in blocks:
                if not cls.NAIVE_SPEC.match(block):
                    raise ValueError('Invalid simple block %r' % block)
                clause &= cls.parse_block(block)

            return clause

        PREFIX_CARET = '^'
        PREFIX_TILDE = '~'
        PREFIX_COMPATIBLE = '~='
        PREFIX_EQ = '=='
        PREFIX_NEQ = '!='
        PREFIX_GT = '>'
        PREFIX_GTE = '>='
        PREFIX_LT = '<'
        PREFIX_LTE = '<='

        PREFIX_ALIASES = {
            '=': PREFIX_EQ,
            '': PREFIX_EQ,
        }

        EMPTY_VALUES = ['*', 'x', 'X', None]

        @classmethod
        def parse_block(cls, expr):
            if not cls.NAIVE_SPEC.match(expr):
                raise ValueError('Invalid simple spec component: %r' % expr)
            prefix, major_t, minor_t, patch_t, revision, prerel, build = cls.NAIVE_SPEC.match(expr).groups()
            prefix = cls.PREFIX_ALIASES.get(prefix, prefix)

            major = None if major_t in cls.EMPTY_VALUES else int(major_t)
            minor = None if minor_t in cls.EMPTY_VALUES else int(minor_t)
            patch = None if patch_t in cls.EMPTY_VALUES else int(patch_t)
            revision = None if revision in cls.EMPTY_VALUES else int(revision)

            if (major is None or minor is None or patch is None) and (revision or prerel or build):
                raise ValueError('Invalid simple spec: %r' % expr)

            if build is not None and prefix not in (cls.PREFIX_EQ, cls.PREFIX_NEQ):
                raise ValueError('Invalid simple spec: %r' % expr)

            if major is None:  # '*'
                target = Version(major=0, minor=0, patch=0)
                if prefix not in (cls.PREFIX_EQ, cls.PREFIX_GTE):
                    raise ValueError('Invalid simple spec: %r' % expr)
            elif minor is None:
                target = Version(major=major, minor=0, patch=0)
            elif patch is None:
                target = Version(major=major, minor=minor, patch=0)
            else:
                target = Version(
                    major=major,
                    minor=minor,
                    patch=patch,
                    revision=revision,
                    prerelease=prerel.split('.') if prerel else (),
                    build=build.split('.') if build else (),
                )

            if prefix == cls.PREFIX_CARET:
                # Accept anything with the same most-significant digit
                if target.major:
                    high = target.next_major()
                elif target.minor:
                    high = target.next_minor()
                else:
                    high = target.next_patch()
                return Range(Range.OP_GTE, target) & Range(Range.OP_LT, high)

            elif prefix == cls.PREFIX_TILDE:
                assert major is not None
                # Accept any higher patch in the same minor
                if minor is None:
                    high = target.next_major()
                else:
                    high = target.next_minor()
                return Range(Range.OP_GTE, target) & Range(Range.OP_LT, high)

            elif prefix == cls.PREFIX_COMPATIBLE:
                assert major is not None
                # ~1 is 1.0.0..2.0.0; ~=2.2 is 2.2.0..3.0.0; ~=1.4.5 is 1.4.5..1.5.0
                if minor is None or patch is None:
                    high = target.next_major()
                else:
                    high = target.next_minor()
                return Range(Range.OP_GTE, target) & Range(Range.OP_LT, high)

            elif prefix == cls.PREFIX_EQ:
                if major is None:
                    return Range(Range.OP_GTE, target)
                elif minor is None:
                    return Range(Range.OP_GTE, target) & Range(Range.OP_LT, target.next_major())
                elif patch is None:
                    return Range(Range.OP_GTE, target) & Range(Range.OP_LT, target.next_minor())
                elif build == '':
                    return Range(Range.OP_EQ, target, build_policy=Range.BUILD_STRICT)
                else:
                    return Range(Range.OP_EQ, target)

            elif prefix == cls.PREFIX_NEQ:
                assert major is not None
                if minor is None:
                    # !=1.x => <1.0.0 || >=2.0.0
                    return Range(Range.OP_LT, target) | Range(Range.OP_GTE, target.next_major())
                elif patch is None:
                    # !=1.2.x => <1.2.0 || >=1.3.0
                    return Range(Range.OP_LT, target) | Range(Range.OP_GTE, target.next_minor())
                elif prerel == '':
                    # !=1.2.3-
                    return Range(Range.OP_NEQ, target, prerelease_policy=Range.PRERELEASE_ALWAYS)
                elif build == '':
                    # !=1.2.3+ or !=1.2.3-a2+
                    return Range(Range.OP_NEQ, target, build_policy=Range.BUILD_STRICT)
                else:
                    return Range(Range.OP_NEQ, target)

            elif prefix == cls.PREFIX_GT:
                assert major is not None
                if minor is None:
                    # >1.x => >=2.0
                    return Range(Range.OP_GTE, target.next_major())
                elif patch is None:
                    return Range(Range.OP_GTE, target.next_minor())
                else:
                    return Range(Range.OP_GT, target)

            elif prefix == cls.PREFIX_GTE:
                return Range(Range.OP_GTE, target)

            elif prefix == cls.PREFIX_LT:
                assert major is not None
                if prerel == '':
                    # <1.2.3-
                    return Range(Range.OP_LT, target, prerelease_policy=Range.PRERELEASE_ALWAYS)
                return Range(Range.OP_LT, target)

            else:
                assert prefix == cls.PREFIX_LTE
                assert major is not None
                if minor is None:
                    # <=1.x => <2.0
                    return Range(Range.OP_LT, target.next_major())
                elif patch is None:
                    return Range(Range.OP_LT, target.next_minor())
                else:
                    return Range(Range.OP_LTE, target)
