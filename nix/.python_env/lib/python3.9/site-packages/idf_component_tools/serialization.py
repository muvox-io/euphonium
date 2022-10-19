'''Class decorators to help with serialization'''

from collections import OrderedDict
from numbers import Number

from six import string_types

try:
    from collections.abc import Iterable, Mapping
except ImportError:
    from collections import Iterable, Mapping  # type: ignore

BASIC_TYPES = (Number, type(None)) + string_types


def _by_key(item):
    return item[0]


def serialize(value):
    '''Serialize value'''
    if isinstance(value, BASIC_TYPES):
        return value

    if isinstance(value, Mapping):
        return OrderedDict((k, serialize(v)) for (k, v) in sorted(value.items(), key=_by_key))

    if isinstance(value, Iterable):
        return [serialize(v) for v in value]

    return value.serialize()


def serializable(_cls=None, like='dict'):
    """Returns the same class with `serialize` method to handle nested structures.
    Requires `_serialization_properties` to be defined in the class"""
    def wrapper(cls):
        # Check if class is already serializable by custom implementation
        if hasattr(cls, 'serialize'):
            return cls

        if like == 'dict':

            def _serialize(self):
                # Use all properties if list is not selected
                properties = sorted(list(set(getattr(self, '_serialization_properties', []))))
                return OrderedDict((prop, serialize(getattr(self, prop))) for prop in properties)
        elif like == 'str':

            def _serialize(self):
                return str(self)
        else:
            raise TypeError("'%s' is not known type for serialization" % like)

        setattr(cls, 'serialize', _serialize)
        return cls

    # handle both @serializable and @serializable() calls
    if _cls is None:
        return wrapper

    return wrapper(_cls)
