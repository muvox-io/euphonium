import os
from string import Template

from ..errors import ManifestError

try:
    from typing import Any, Callable
except ImportError:
    pass


def subst_vars_in_str(s, env):  # type: (str, dict[str, Any]) -> str
    try:
        return Template(s).substitute(env)
    except KeyError as e:
        raise ManifestError('Environment variable "{}" is not set'.format(e.args[0]))
    except ValueError:
        raise ManifestError(
            'Invalid format of environment varible in the value: "{}".\n'
            'Note: you can use "$$" to escape the "$" character'.format(s))


def expand_env_vars(
        obj,  # type: dict[str, Any] | list | str | Any
        env=None  # type: dict | None
):
    # type: (...) -> dict[str, Any] | list | str | Any
    '''
    Expand variables in the results of YAML/JSON file parsing
    '''
    if env is None:
        env = dict(os.environ)

    def expand_env_in_str(value):
        return subst_vars_in_str(value, env)

    # we don't process other data types, like numbers
    return process_nested_strings(obj, expand_env_in_str)


def process_nested_strings(
        obj,  # type: dict[str, Any] | list | str | Any
        func  # type: Callable[[str], Any]
):
    # type: (...) -> dict[str, Any] | list | str | Any
    '''
    Recursively process strings in the results of YAML/JSON file parsing
    '''

    if isinstance(obj, dict):
        return {k: process_nested_strings(v, func) for k, v in obj.items()}
    elif isinstance(obj, str):
        return func(obj)
    elif isinstance(obj, (list, tuple)):
        # yaml dict won't have other iterable data types
        return [process_nested_strings(i, func) for i in obj]

    # we don't process other data types, like numbers
    return obj
