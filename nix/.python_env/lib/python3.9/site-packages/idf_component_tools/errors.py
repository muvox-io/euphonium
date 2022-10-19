try:
    from typing import Any
except ImportError:
    pass


class FatalError(RuntimeError):
    """Generic unrecoverable runtime error"""
    exit_code = 2

    def __init__(self, *args, **kwargs):  # type: (Any, Any) -> None
        super(FatalError, self).__init__(*args)
        exit_code = kwargs.pop('exit_code', None)
        if exit_code:
            self.exit_code = exit_code


class NothingToDoError(FatalError):
    '''Generic Runtime error for states when operation is prematurely aborted due to nothing to do'''
    exit_code = 144  # NOP


class SolverError(FatalError):
    pass


class ProcessingError(FatalError):
    pass


class FetchingError(ProcessingError):
    pass


class SourceError(ProcessingError):
    pass


class ManifestError(ProcessingError):
    pass


class LockError(ProcessingError):
    pass


class GitError(ProcessingError):
    pass


class ComponentModifiedError(ProcessingError):
    pass


class InvalidComponentHashError(ProcessingError):
    pass
