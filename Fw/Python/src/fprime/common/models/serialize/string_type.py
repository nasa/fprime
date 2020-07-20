"""
Created on Dec 18, 2014

@author: tcanham

"""
import struct

from fprime.constants import DATA_ENCODING

from . import type_base
from .type_exceptions import (
    DeserializeException,
    NotInitializedException,
    StringSizeException,
    TypeMismatchException,
)


class StringType(type_base.ValueType):
    """
    String type representation for F prime. This is a value type that stores a half-word first for representing the
    length of this given string.
    """

    def __init__(self, val=None, max_string_len=None):
        """
        Constructor to build a string
        @param val: value form which to create a string. Default: None.
        @param max_string: maximum length of the string. Default: None, not used.
        """
        self.__max_string_len = max_string_len
        super().__init__(val)

    def validate(self, val):
        """ Validates that this is a string """
        if not isinstance(val, str):
            raise TypeMismatchException(str, type(val))
        elif self.__max_string_len is not None and len(val) > self.__max_string_len:
            raise StringSizeException(len(self.val), self.__max_string_len)

    def serialize(self):
        """
        Serializes the string in a binary format
        """
        # If val is never set then it is init exception...
        if self.val is None:
            raise NotInitializedException(type(self))
        # Check string size before serializing
        elif (
            self.__max_string_len is not None and len(self.val) > self.__max_string_len
        ):
            raise StringSizeException(len(self.val), self.__max_string_len)
        # Pack the string size first then return the encoded data
        buff = struct.pack(">H", len(self.val)) + self.val.encode(DATA_ENCODING)
        return buff

    def deserialize(self, data, offset):
        """
        Deserializes a string from the given data buffer.
        """
        try:
            val_size = struct.unpack_from(">H", data, offset)[0]
            # Deal with not enough data left in the buffer
            if len(data[offset + 2 :]) < val_size:
                raise DeserializeException(
                    "Not enough data to deserialize string data. Needed: {} Left: {}".format(
                        val_size, len(data[offset + 2 :])
                    )
                )
            # Deal with a string that is larger than max string
            elif self.__max_string_len is not None and val_size > self.__max_string_len:
                raise StringSizeException(val_size, self.__max_string_len)
            self.val = data[offset + 2 : offset + 2 + val_size].decode(DATA_ENCODING)
        except struct.error:
            raise DeserializeException("Not enough bytes to deserialize string length.")

    def getSize(self):
        """
        Get the size of this object
        """
        return struct.calcsize(">H") + len(self.val)
