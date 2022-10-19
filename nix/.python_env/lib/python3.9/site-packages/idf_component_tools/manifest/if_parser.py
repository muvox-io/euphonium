import inspect
import re
from ast import literal_eval

from schema import SchemaError

from idf_component_tools.build_system_tools import get_env_idf_target, get_idf_version
from idf_component_tools.manifest.constants import IF_IDF_VERSION_REGEX, IF_TARGET_REGEX
from idf_component_tools.semver import SimpleSpec, Version
from idf_component_tools.serialization import serializable

IF_IDF_VERSION_REGEX_COMPILED = re.compile(IF_IDF_VERSION_REGEX)
IF_TARGET_REGEX_COMPILED = re.compile(IF_TARGET_REGEX)

COMMON_ERR_MSG = inspect.cleandoc(
    """
Invalid if clause. Here are some valid examples:
    For keyword "idf_version", you could use:
        - if: "idf_version ~= 5.0.0"
        - if: "idf_version >=3.3,<5.0"
    For keyword "target", you could use:
        - if: "target not in [esp32, esp32c3]"
        - if: "target != esp32"
""")


@serializable
class IfClause:
    _serialization_properties = [
        'clause',
        'bool_value',
    ]

    def __init__(self, clause, bool_value):  # type: (str, bool) -> None
        self.clause = clause
        self.bool_value = bool_value

    def __repr__(self):
        return '{} ({})'.format(self.clause, self.bool_value)


def _eval_str(s):  # type: (str) -> str
    _s = s.strip()
    if not (_s[0] == _s[-1] == '"'):
        _s = '"{}"'.format(_s.replace('"', r'\"'))

    try:
        return literal_eval(_s)
    except (ValueError, SyntaxError):
        raise SchemaError(None, ['Invalid string `{}`'.format(s), COMMON_ERR_MSG])


def _eval_list(s):  # type: (str) -> list[str]
    _s = s.strip()

    if _s[0] == '[' and _s[-1] == ']':
        _s = _s[1:-1]

    try:
        return [_eval_str(part) for part in _s.split(',')]
    except (ValueError, SyntaxError):
        raise SchemaError(None, ['Invalid list `{}`'.format(s), COMMON_ERR_MSG])


def _parse_if_idf_version_clause(mat):  # type: (re.Match) -> IfClause
    comparison = mat.group('comparison')
    spec = mat.group('spec')
    spec = ','.join([part.strip() for part in spec.split(',')])

    try:
        simple_spec = SimpleSpec('{}{}'.format(_eval_str(comparison), _eval_str(spec)))
    except ValueError as e:
        raise SchemaError(None, [str(e), COMMON_ERR_MSG])

    idf_version = get_idf_version()
    return IfClause('{} {}'.format(idf_version, simple_spec.expression), simple_spec.match(Version(idf_version)))


def _parser_if_target_clause(mat):  # type: (re.Match) -> IfClause
    comparison = mat.group('comparison')
    versions = mat.group('versions').strip()

    env_target = get_env_idf_target()
    if comparison == '!=':
        bool_value = env_target != _eval_str(versions)
    elif comparison == '==':
        bool_value = env_target == _eval_str(versions)
    elif comparison == 'not in':
        bool_value = env_target not in _eval_list(versions)
    elif comparison == 'in':
        bool_value = env_target in _eval_list(versions)
    else:
        raise SchemaError(None, COMMON_ERR_MSG)

    return IfClause('{} {} {}'.format(env_target, comparison, versions), bool_value)


def parse_if_clause(if_clause):  # type: (str) -> IfClause
    is_idf_version = True
    res = IF_IDF_VERSION_REGEX_COMPILED.match(if_clause)
    if not res:
        is_idf_version = False
        res = IF_TARGET_REGEX_COMPILED.match(if_clause)
    if not res:
        raise SchemaError(None, COMMON_ERR_MSG)

    if is_idf_version:
        return _parse_if_idf_version_clause(res)
    else:
        return _parser_if_target_clause(res)
