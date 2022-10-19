import os
import re
import subprocess  # nosec
import time
from datetime import datetime
from functools import wraps

from .errors import GitError
from .semver import Version

try:
    from typing import Any, Callable, List, Optional, Union
except ImportError:
    pass


# Git error that is supposed to be handled in the code, non-fatal
class GitCommandError(Exception):
    pass


class GitClient(object):
    """ Set of tools for working with git repos """
    def __init__(self, git_command='git', min_supported='2.0.0'):  # type: (str, Union[str, Version]) -> None
        self.git_command = git_command or 'git'
        self.git_min_supported = min_supported if isinstance(min_supported, Version) else Version(min_supported)

        self._git_checked = False
        self._repo_updated = False

    def _git_cmd(func):  # type: (Union[GitClient, Callable[..., Any]]) -> Callable
        @wraps(func)  # type: ignore
        def wrapper(self, *args, **kwargs):
            if not self._git_checked:
                self.check_version()
                self._git_checked = True

            return func(self, *args, **kwargs)

        return wrapper

    def _update_bare_repo(self, *args, **kwargs):
        repo = kwargs.get('repo') or args[0]
        bare_path = kwargs.get('bare_path') or args[1]
        if not os.path.exists(bare_path):
            os.makedirs(bare_path)

        if not os.listdir(bare_path):
            self.run(['init', '--bare'], cwd=bare_path)
            self.run(['remote', 'add', 'origin', '--tags', '--mirror=fetch', repo], cwd=bare_path)

        if self.run(['config', '--get', 'remote.origin.url'], cwd=bare_path) != repo:
            self.run(['remote', 'set-url', 'origin', repo], cwd=bare_path)

        fetch_file = os.path.join(bare_path, 'FETCH_HEAD')
        current_time = time.mktime(datetime.now().timetuple())

        # Don't fetch too often, at most once a minute
        if not os.path.isfile(fetch_file) or current_time - os.stat(fetch_file).st_mtime > 60:
            self.run(['fetch', 'origin'], cwd=bare_path)

    def _bare_repo(func):  # type: (Union[GitClient, Callable[..., Any]]) -> Callable
        @wraps(func)  # type: ignore
        def wrapper(self, *args, **kwargs):
            if not self._repo_updated:
                self._update_bare_repo(*args, **kwargs)
                self._repo_updated = True

            return func(self, *args, **kwargs)

        return wrapper

    @_git_cmd
    def commit_id(self, path):  # type: (str) -> str
        return self.run(['show', '--format="%H"', '--no-patch'], cwd=path)

    @_git_cmd
    def is_dirty(self, path):  # type: (str) -> bool
        try:
            self.run(['diff', '--quiet'], cwd=path)
            return False
        except GitCommandError:
            return True

    @_git_cmd
    def is_git_dir(self, path):  # type: (str) -> bool
        try:
            return self.run(['rev-parse', '--is-inside-work-tree'], cwd=path).strip() == 'true'
        except GitCommandError:
            return False

    @_git_cmd
    @_bare_repo
    def prepare_ref(
            self,
            repo,  # type: str
            bare_path,  # type: str
            checkout_path,  # type: str
            ref=None,  # type: str
            with_submodules=True,  # type: bool
            selected_paths=None  # type: list[str]
    ):  # type: (...) -> str
        """
        Checkout required branch to desired path. Create a bare repo, if necessary

        Parameters
        ----------
        repo: str
            URL of the repository
        bare_path: str
            Path to the bare repository
        checkout_path: str
            Path to checkout working repository
        ref: str
            Branch name, commit id or '*'
        with_submodules: bool
             If True, submodules will be downloaded
        selected_paths: List[str]
            List of folders and files that need to download
        Returns
        -------
            Commit id of the current checkout
        """
        commit_id = self.get_commit_id_by_ref(repo, bare_path, ref)

        # Checkout required branch
        checkout_command = ['--work-tree', checkout_path, '--git-dir', bare_path, 'checkout', '--force', commit_id]
        if selected_paths:
            checkout_command += ['--'] + selected_paths
        self.run(checkout_command)

        # And remove all untracked files
        self.run(['--work-tree', checkout_path, '--git-dir', bare_path, 'clean', '--force'])
        # Submodules
        if with_submodules:
            self.run(
                [
                    '--work-tree=.', '-C', checkout_path, '--git-dir', bare_path, 'submodule', 'update', '--init',
                    '--recursive'
                ])

        return commit_id

    @_git_cmd
    @_bare_repo
    def get_commit_id_by_ref(self, repo, bare_path, ref):  # type: (str, str, str) -> str
        if ref:
            # If branch is provided check that exists
            try:
                self.run(['branch', '--contains', ref], cwd=bare_path)
            except GitCommandError:
                raise GitError('Branch "%s" doesn\'t exist in repo "%s"' % (ref, repo))

        else:
            # Set to latest commit from remote's HEAD
            ref = self.run(['ls-remote', '--exit-code', 'origin', 'HEAD'], cwd=bare_path)[:40]

        return self.run(['rev-parse', '--verify', ref], cwd=bare_path).strip()

    def run(self, args, cwd=None, env=None):  # type: (List[str], str, dict) -> str
        if cwd is None:
            cwd = os.getcwd()
        env_copy = dict(os.environ)
        if env:
            env_copy.update(env)
        try:
            return subprocess.check_output(  # nosec
                [self.git_command] + list(args),
                cwd=cwd,
                env=env_copy,
                stderr=subprocess.STDOUT,
            ).decode('utf-8')
        except subprocess.CalledProcessError as e:
            raise GitCommandError(
                "'git %s' failed with exit code %d \n%s" % (' '.join(args), e.returncode, e.output.decode('utf-8')))

    def check_version(self):  # type: () -> None
        version = self.version()

        if version < self.git_min_supported:
            raise GitError(
                'Your git version %s is older than minimally required %s.' % (
                    version,
                    self.git_min_supported,
                ))

    def version(self):  # type: () -> Version
        try:
            git_version_str = subprocess.check_output(  # nosec
                [self.git_command, '--version'],
                stderr=subprocess.STDOUT
            ).decode('utf-8')
        except OSError:
            raise GitError("git command wasn't found")

        ver_match = re.match(r'^git version (\d+\.\d+\.\d+)', git_version_str)

        try:
            if ver_match:
                return Version(ver_match.group(1))
            else:
                raise GitCommandError()
        except (IndexError, ValueError, GitCommandError):
            raise GitError('Cannot recognize git version')

    @_git_cmd
    def get_tag_version(self):  # type: () -> Optional[Version]
        try:
            tag_str = self.run(['describe', '--exact-match'])
            if tag_str.startswith('v'):
                tag_str = tag_str[1:]
        except GitCommandError:
            return None

        try:
            semantic_version = Version(tag_str)
            return semantic_version
        except ValueError:
            return None
