# Copyright 2019 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Contains elements taken from "appdirs" python package
# https://github.com/ActiveState/appdirs/tree/1.4.3
# Copyright (c) 2005-2010 ActiveState Software Inc.
# Copyright (c) 2013 Eddy Petrisor
# Originally released under MIT license
"""Classes to work with file cache"""

import os
import sys


class FileCache(object):
    """Common functions to work with components cache"""

    _path = None

    @classmethod
    def path(cls):
        if not cls._path:
            cls._path = cls.cache_path()

            if not os.path.exists(cls._path):
                os.makedirs(cls._path)

        return cls._path

    @classmethod
    def cache_path(cls):
        """Path of cache directory"""
        system_cache_path = SystemCachePath()

        if sys.platform.startswith('win'):
            cache_directory = system_cache_path.cache_path_win()
            return os.path.join(cache_directory, 'Espressif', 'ComponentManager', 'Cache')
        else:
            if sys.platform == 'darwin':
                cache_directory = system_cache_path.cache_path_macos()
            else:
                cache_directory = system_cache_path.cache_path_unix()

            return os.path.join(cache_directory, 'Espressif', 'ComponentManager')


class SystemCachePath(object):
    """Methods to fetch user specific cache path for every platform"""

    PY3 = sys.version_info[0] == 3

    def _get_win_folder_from_registry(self):
        """This is a fallback technique at best. I'm not sure if using the
        registry for this guarantees us the correct answer for all CSIDL_*
        names.
        """
        if self.PY3:
            import winreg as _winreg
        else:
            import _winreg

        shell_folder_name = 'Local AppData'

        key = _winreg.OpenKey(
            _winreg.HKEY_CURRENT_USER,
            r'Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders',
        )
        dir, type = _winreg.QueryValueEx(key, shell_folder_name)
        return dir

    def _get_win_folder_with_pywin32(self):
        from win32com.shell import shell, shellcon

        dir = shell.SHGetFolderPath(0, getattr(shellcon, 'CSIDL_LOCAL_APPDATA'), 0, 0)
        # Try to make this a unicode path because SHGetFolderPath does
        # not return unicode strings when there is unicode data in the
        # path.
        try:
            dir = str(dir) if self.PY3 else unicode(dir)  # noqa: F821

            # Downgrade to short path name if have highbit chars.
            has_high_char = False
            for c in dir:
                if ord(c) > 255:
                    has_high_char = True
                    break
            if has_high_char:
                try:
                    import win32api

                    dir = win32api.GetShortPathName(dir)
                except ImportError:
                    pass
        except UnicodeError:
            pass
        return dir

    def _get_win_folder_with_ctypes(csidl_name):
        import ctypes

        # Constant for fetching app local data path
        CSIDL_LOCAL_APPDATA = 28

        buf = ctypes.create_unicode_buffer(1024)
        ctypes.windll.shell32.SHGetFolderPathW(None, CSIDL_LOCAL_APPDATA, None, 0, buf)

        # Downgrade to short path name if have highbit chars.
        has_high_char = False
        for c in buf:
            if ord(c) > 255:
                has_high_char = True
                break
        if has_high_char:
            buf2 = ctypes.create_unicode_buffer(1024)
            if ctypes.windll.kernel32.GetShortPathNameW(buf.value, buf2, 1024):
                buf = buf2

        return buf.value

    def _get_win_folder_with_jna(self):
        import array

        from com.sun import jna
        from com.sun.jna.platform import win32

        buf_size = win32.WinDef.MAX_PATH * 2
        buf = array.zeros('c', buf_size)
        shell = win32.Shell32.INSTANCE
        shell.SHGetFolderPath(
            None,
            getattr(win32.ShlObj, 'CSIDL_LOCAL_APPDATA'),
            None,
            win32.ShlObj.SHGFP_TYPE_CURRENT,
            buf,
        )
        dir = jna.Native.toString(buf.tostring()).rstrip('\0')

        # Downgrade to short path name if have highbit chars.
        has_high_char = False
        for c in dir:
            if ord(c) > 255:
                has_high_char = True
                break
        if has_high_char:
            buf = array.zeros('c', buf_size)
            kernel = win32.Kernel32.INSTANCE
            if kernel.GetShortPathName(dir, buf, buf_size):
                dir = jna.Native.toString(buf.tostring()).rstrip('\0')

        return dir

    def cache_path_win(self):
        try:
            import win32com.shell  # noqa: F401

            return self._get_win_folder_with_pywin32()
        except ImportError:
            try:
                from ctypes import windll  # noqa: F401

                return self._get_win_folder_with_ctypes()
            except ImportError:
                try:
                    import com.sun.jna  # noqa: F401

                    return self._get_win_folder_with_jna()
                except ImportError:
                    return self._get_win_folder_from_registry()

    def cache_path_macos(self):
        return os.path.expanduser('~/Library/Caches')

    def cache_path_unix(self):
        return os.getenv('XDG_CACHE_HOME') or os.path.expanduser('~/.cache')
