"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import absolute_import
import struct
from .type_exceptions import DeserializeException, NotInitializedException, TypeMismatchException
from .type_base import ValueType


class BoolType(ValueType):
    """
    Representation of a boolean type that will be stored for F prime. True values are stored as a U8 of 0xFF and False
    is stored as a U8 of 0x00.
    """
    TRUE = 0xFF
    FALSE = 0x00

    def validate(self, val):
        """ Validate the given class """
        if not isinstance(bool, val):
            raise TypeMismatchException(bool, type(val))

    def serialize(self):
        """ Serialize a boolean value"""
        if self.val is None:
            raise NotInitializedException(type(self))
        return struct.pack("B", 0xFF if self.val else 0x00)

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        try:
            return struct.unpack_from("B", self.TRUE if self.val else self.FALSE)
        except struct.error:
            raise DeserializeException("Not enough bytes to deserialize bool.")

    def getSize(self):
        return struct.calcsize("B")
