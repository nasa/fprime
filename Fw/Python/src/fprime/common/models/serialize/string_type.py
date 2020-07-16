"""
Created on Dec 18, 2014

@author: tcanham

"""
from __future__ import print_function
from __future__ import absolute_import
import struct
import sys

from fprime.constants import DATA_ENCODING

from .type_exceptions import TypeMismatchException
from .type_exceptions import NotInitializedException
from .type_exceptions import StringSizeException
from .type_exceptions import DeserializeException
from . import type_base


@type_base.serialize
@type_base.deserialize
class StringType(type_base.BaseType):
    def __init__(self, val=None, max_string_len=None):
        """
        Constructor
        @param val: Default value for the string, if None then empty string.
        @param max_string: Maximum allowed string length set in auto-code.
        """
        super().__init__()
        self.__val = val
        self.__max_string_len = max_string_len

        if val == None:
            return

        self._check_val(val)

    def _check_val(self, val):
        if not type(val) == type(str()) and not type(val) == type(u""):
            raise TypeMismatchException(type(str()), type(val))

    @property
    def val(self):
        return self.__val

    @val.setter
    def val(self, val):
        self._check_val(val)
        if not type(val) == type(str()):
            val = val.encode("utf-8")
        self.__val = val

    def serialize(self):
        """
        """
        # If val is never set then it is init exception...
        if self.val == None:
            raise NotInitializedException(type(self))

        # If max_string_len is set check that string size does not exceed...
        if not self.__max_string_len == None:
            s = len(self.val)
            if s > self.__max_string_len:
                raise StringSizeException(s, self.__max_string_len)

        # store string value plus size - place size in front
        buff = struct.pack(">H", len(self.val))
        if sys.version_info >= (3, 0):
            buff = buff + self.val.encode(DATA_ENCODING)
        else:
            buff = buff + self.val
        return buff

    def deserialize(self, data, offset):
        """
        """
        # make sure enough space to extract integer size
        if len(data) < 3:
            raise DeserializeException(
                "Not enough data to deserialize! Needed: %d Left: %d" % (4, offset)
            )

        # subtract size of int for stored string size
        strSize = struct.unpack_from(">H", data, offset)[0]

        if len(data) - offset - 2 < strSize:
            raise DeserializeException(
                "Not enough data to deserialize! Needed: %d Left: %d"
                % (strSize, offset)
            )

        # get string value here
        tmp_data = data[offset + 2 : offset + 2 + strSize]
        if sys.version_info >= (3, 0):
            self.val = tmp_data.decode(DATA_ENCODING)
        else:
            self.val = tmp_data
        # If max_string_len is set check that string size does not exceed...
        if not self.__max_string_len == None:
            s = len(self.val)
            if s > self.__max_string_len:
                raise StringSizeException(s, self.__max_string_len)

    def getSize(self):
        """
        """
        return len(self.val) + struct.calcsize(">H")

    def __repr__(self):
        return "String"
