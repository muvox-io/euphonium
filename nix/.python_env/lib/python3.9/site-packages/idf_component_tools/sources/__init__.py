from .base import BaseSource
from .git import GitSource
from .idf import IDFSource
from .local import LocalSource
from .web_service import WebServiceSource

try:
    from typing import List, Type
except ImportError:
    pass

KNOWN_SOURCES = [IDFSource, GitSource, LocalSource, WebServiceSource]  # type: List[Type[BaseSource]]

__all__ = [
    'BaseSource',
    'WebServiceSource',
    'LocalSource',
    'IDFSource',
    'GitSource',
    'KNOWN_SOURCES',
]
