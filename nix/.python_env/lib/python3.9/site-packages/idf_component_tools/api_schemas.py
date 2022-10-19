from schema import Optional, Or, Schema, Use
from six import string_types

STRING = Or(*string_types)
OPTIONAL_STRING = Or(None, *string_types)

ERROR_SCHEMA = Schema(
    {
        'error': STRING,
        'messages': Or([STRING], {STRING: object}),
        Optional(STRING): object,
    },
    error='Unexpected error format',
)

DEPENDENCY = {
    'spec': STRING,
    'source': STRING,
    Optional('name'): OPTIONAL_STRING,
    Optional('namespace'): OPTIONAL_STRING,
    Optional('is_public'): Use(bool),
    Optional(Use(str)): object,
}

VERSION = {
    'version': STRING,
    'component_hash': STRING,
    'url': STRING,
    Optional('dependencies'): [DEPENDENCY],
    Optional('targets'): Or([STRING], None),
    Optional(STRING): object,
}

COMPONENT_SCHEMA = Schema(
    {
        'name': STRING,
        'namespace': STRING,
        'versions': [VERSION],
        Optional(STRING): object,
    },
    error='Unexpected format of the component',
)

VERSION_UPLOAD_SCHEMA = Schema(
    {
        'job_id': STRING,
        Optional(STRING): object,
    },
    error='Unexpected response to version upload',
)

TASK_STATUS_SCHEMA = Schema(
    {
        'id': STRING,
        'status': STRING,
        Optional('message'): OPTIONAL_STRING,
        Optional('progress'): Or(Use(float), None),
        Optional(STRING): object
    })
