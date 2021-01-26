## Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 # Use of this source code is governed by a BSD-style license that can
 # be found in the License.html file in the root of the source tree.
 ##

"""MediaConchLib Python wrapper module.
"""

import sys as _sys
from ctypes import CDLL as _CDLL
from ctypes import c_void_p as _c_void_p
from ctypes import c_char_p as _c_char_p
from ctypes import c_size_t as _c_size_t
from ctypes import c_bool as _c_bool
from ctypes import c_long as _c_long
from ctypes import c_int as _c_int

if _sys.platform == "darwin":
    _MediaConchLib_Handler = _CDLL("libmediaconch.0.dylib")
else:
    _MediaConchLib_Handler = _CDLL("libmediaconch.so.0")

class Format:
    """Possibles MediaConch output formats.

    Attributes:
        text:   Text report.
        xml:    XML report.
        maxml:  MediaArea XML report (default).
        html:   HTML report.
        simple: Global outcome.
        csv:    CSV report.
    """
    text, xml, maxml, _, html, _, simple, csv, max = list(range(9))

class Report:
    """Possibles reports types in MediaConch output.

    Attributes:
        mediaconch:      MediaConch implementation/policy report (Default).
        mediainfo:       MediaInfo report.
        mediatrace:      Parsing traces.
        micromediatrace: Parsing traces (in compact format).
    """
    mediaconch, mediainfo, _, _, mediatrace, micromediatrace, max =  list(range(0, 7))

class MediaConchException(BaseException):
    """Raised when MediaConchLib sets get_last_error() to non-empty-string.
    """

class MediaConch:
    """Wrapper to call MediaConchLib native library C API through ctypes interface.
    """
    _MediaConch_new = _MediaConchLib_Handler.MediaConch_new
    _MediaConch_new.argtypes = []
    _MediaConch_new.restype  = _c_void_p

    _MediaConch_delete = _MediaConchLib_Handler.MediaConch_delete
    _MediaConch_delete.argtypes = [_c_void_p]
    _MediaConch_delete.restype  = None

    _MediaConch_get_last_error = _MediaConchLib_Handler.MediaConch_get_last_error
    _MediaConch_get_last_error.argtypes = [_c_void_p]
    _MediaConch_get_last_error.restype = _c_char_p

    _MediaConch_add_file = _MediaConchLib_Handler.MediaConch_add_file
    _MediaConch_add_file.argtypes = [_c_void_p, _c_char_p]
    _MediaConch_add_file.restype = _c_long

    _MediaConch_add_report = _MediaConchLib_Handler.MediaConch_add_report
    _MediaConch_add_report.argtypes = [_c_void_p, _c_size_t]
    _MediaConch_add_report.restype = None

    _MediaConch_add_policy = _MediaConchLib_Handler.MediaConch_add_policy
    _MediaConch_add_policy.argtypes = [_c_void_p, _c_char_p]
    _MediaConch_add_policy.restype = None

    _MediaConch_set_format = _MediaConchLib_Handler.MediaConch_set_format
    _MediaConch_set_format.argtypes = [_c_void_p, _c_size_t]
    _MediaConch_set_format.restype = None

    _MediaConch_set_display = _MediaConchLib_Handler.MediaConch_set_display
    _MediaConch_set_display.argtypes = [_c_void_p, _c_char_p]
    _MediaConch_set_display.restype = None

    _MediaConch_set_mil_option = _MediaConchLib_Handler.MediaConch_set_mil_option
    _MediaConch_set_mil_option.argtypes = [_c_void_p, _c_char_p, _c_char_p]
    _MediaConch_set_mil_option.restype = None

    _MediaConch_set_force_analyze = _MediaConchLib_Handler.MediaConch_set_force_analyze
    _MediaConch_set_force_analyze.argtypes = [_c_void_p, _c_bool]
    _MediaConch_set_force_analyze.restype = None

    _MediaConch_set_policy_verbosity = _MediaConchLib_Handler.MediaConch_set_policy_verbosity
    _MediaConch_set_policy_verbosity.argtypes = [_c_void_p, _c_int]
    _MediaConch_set_policy_verbosity.restype = None

    _MediaConch_set_implementation_verbosity = _MediaConchLib_Handler.MediaConch_set_implementation_verbosity
    _MediaConch_set_implementation_verbosity.argtypes = [_c_void_p, _c_int]
    _MediaConch_set_implementation_verbosity.restype = None

    _MediaConch_get_report = _MediaConchLib_Handler.MediaConch_get_report
    _MediaConch_get_report.argtypes = [_c_void_p, _c_long]
    _MediaConch_get_report.restype = _c_char_p

    _handle = _c_void_p(0)

    def __init__(self, throws=True):
        """Create MediaConch object.

        Args:
            throws (bool): Raises exception if MediaConchLib report an error after an API call.
        """
        self._handle = self._MediaConch_new()
        self._throws = throws
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def __del__(self):
        self._MediaConch_delete(self._handle)

    # API
    def get_last_error(self):
        """Get MediaConchLib error message for the last API call.

        If self._throws is set to True, an exception is raised if
        this fuction returns an non-empty string after an API call.
        Otherwise you have to check the returned value manually.

        Returns:
            str: Error string (empty if none).
        """
        return self._MediaConch_get_last_error(self._handle).decode("utf_8", "ignore")

    def add_file(self, filename):
        """Add file to MediaConch database.

        Args:
            filename (str): File to add.

        Returns:
            int: File ID (-1 on error).

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        value = self._MediaConch_add_file(self._handle, filename.encode("utf-8"))
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)
        return value

    def add_report(self, report):
        """Add report type in the output report.

        Args:
            report (Report): Report type.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_add_report(self._handle, report)
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def add_policy(self, filename):
        """Add a policy to validate files against.

        Args:
            filename (str): Policy file.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_add_policy(self._handle, filename.encode("utf-8"))
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def set_format(self, format):
        """Set report format (internal display file).

        Args:
            format (Format): Report format.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_set_format(self._handle, format)
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def set_display(self, filename):
        """Set custom display.

        Args:
            filename (str): Display file.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_set_display(self._handle, filename.encode("utf-8"))
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def set_mil_option(self, name, value):
        """Pass option to the MediaInfo library.

        Args:
            name (str):  MediaInfo option name.
            value (str): Option value.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_set_mil_option(self._handle, name, value)
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def set_force_analyze(self, force):
        """Force add_file() to reparse the file if registered in database.

        Args:
            force (bool): Mode.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_set_force_analyze(self._handle, force)
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def set_policy_verbosity(self, verbosity):
        """Set verbosity for the policy report.

        Parameters:
            verbosity (int): Requested verbosity.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_set_policy_verbosity(self._handle, verbosity)
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def set_implementation_verbosity(self, verbosity):
        """Set verbosity for the implementation report.

        Args:
            verbosity (int): Requested verbosity.

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        self._MediaConch_set_implementation_verbosity(self._handle, verbosity)
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)

    def get_report(self, file_id):
        """Get report for the given file ID.

        Args:
            file_id (int): File ID returned by add_file() function.

        Returns:
            str: Requested report (empty if error).

        Raises:
            MediaConchException: If self._throws is set to True and MediaConchLib report an error.
        """
        value = self._MediaConch_get_report(self._handle, file_id).decode("utf_8", "ignore")
        if self._throws:
            last_error = self.get_last_error()
            if last_error:
                raise MediaConchException(last_error)
        return value
