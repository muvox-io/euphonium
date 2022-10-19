class APIClientError(Exception):
    pass


class ComponentNotFound(APIClientError):
    pass


class NamespaceNotFound(APIClientError):
    pass


KNOWN_API_ERRORS = {
    'NamespaceNotFoundError': NamespaceNotFound,
    'ComponentNotFoundError': ComponentNotFound,
}
