"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
import struct

from .type_base import ValueType
from .type_exceptions import (
    DeserializeException,
    NotInitializedException,
    TypeMismatchException,
    TypeRangeException,
)


class BoolType(ValueType):
    """
    Representation of a boolean type that will be stored for F prime. True values are stored as a U8 of 0xFF and False
    is stored as a U8 of 0x00.
    """

    TRUE = 0xFF
    FALSE = 0x00

    def validate(self, val):
        """ Validate the given class """
        if not isinstance(val, bool):
            raise TypeMismatchException(bool, type(val))

    def serialize(self):
        """ Serialize a boolean value """
        if self.val is None:
            raise NotInitializedException(type(self))
        return struct.pack("B", 0xFF if self.val else 0x00)

    def deserialize(self, data, offset):
        """ Deserialize boolean value """
        try:
            int_val = struct.unpack_from("B", data, offset)[0]
            if int_val not in [self.TRUE, self.FALSE]:
                raise TypeRangeException(int_val)
            self.val = int_val == self.TRUE
        except struct.error:
            raise DeserializeException("Not enough bytes to deserialize bool.")

    def getSize(self):
        return struct.calcsize("B")
