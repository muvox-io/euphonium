from io import open

import yaml
from schema import Or, Schema, SchemaError
from six import string_types

from idf_component_tools.errors import ProcessingError

try:
    from typing import Dict, List
except ImportError:
    pass

COMPONENT_LIST_SCHEMA = Schema(
    {'components': [
        {
            'name': Or(*string_types),
            'path': Or(*string_types)
        },
    ]}, ignore_extra_keys=True)


def parse_component_list(path):  # type: (str) -> List[Dict[str,str]]
    with open(path, mode='r', encoding='utf-8') as f:
        try:
            components = COMPONENT_LIST_SCHEMA.validate(yaml.safe_load(f.read()))
            return components['components']
        except (yaml.YAMLError, SchemaError):
            raise ProcessingError('Cannot parse components list file.')
