"""
numerical_types.py:

A file that contains the definitions for all the integer types provided as part of F prime.  F prime supports integers
that map to stdint.h integer sizes, that is, 8-bit, 16-bit, 32-bit, and 64-bit signed and unsigned integers.

@author mstarch
"""
import abc
import re
import struct

from .type_base import ValueType
from .type_exceptions import (
    DeserializeException,
    NotInitializedException,
    TypeMismatchException,
    TypeRangeException,
)

BITS_RE = re.compile(r"[IUF](\d\d?)")


class NumericalType(ValueType, abc.ABC):
    """ Numerical types that can be serialized using struct and are of some power of 2 byte width """

    @classmethod
    def get_bits(cls):
        """ Gets the integer bits of a given type """
        match = BITS_RE.match(cls.__name__)
        assert (
            match
        ), "Type {} does not follow format I#Type U#Type nor F#Type required of numerical types".format(
            cls
        )
        return int(match.group(1))

    @classmethod
    def getSize(cls):
        """ Gets the size of the integer based on the size specified in the class name """
        return int(cls.get_bits() / 8)

    @staticmethod
    @abc.abstractmethod
    def get_serialize_format():
        """ Gets the format serialization string such that the class can be serialized via struct """

    def serialize(self):
        """ Serializes this type using struct and the val property """
        if self.val is None:
            raise NotInitializedException(type(self))
        return struct.pack(self.get_serialize_format(), self.val)

    def deserialize(self, data, offset):
        """ Serializes this type using struct and the val property """
        try:
            self.val = struct.unpack_from(self.get_serialize_format(), data, offset)[0]
        except struct.error as err:
            raise DeserializeException(str(err))


class IntegerType(NumericalType, abc.ABC):
    """ Base class thar represents all integer common functions """

    def validate(self, val):
        """ Validates the given integer. """
        if not isinstance(val, int):
            raise TypeMismatchException(int, type(val))
        max_val = 1 << (
            self.get_bits() - (1 if self.__class__.__name__.startswith("I") else 0)
        )
        min_val = -max_val if self.__class__.__name__.startswith("I") else 0
        # Compare to min and max
        if val < min_val or val >= max_val:
            raise TypeRangeException(val)


class FloatType(NumericalType, abc.ABC):
    """ Base class thar represents all float common functions """

    def validate(self, val):
        """ Validates the given integer. """
        if not isinstance(val, float):
            raise TypeMismatchException(float, type(val))


class I8Type(IntegerType):
    """ Single byte integer type. Represents C chars """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return "b"


class I16Type(IntegerType):
    """ Double byte integer type. Represents C shorts """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">h"


class I32Type(IntegerType):
    """ Four byte integer type. Represents C int32_t, """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">i"


class I64Type(IntegerType):
    """ Eight byte integer type. Represents C int64_t, """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">q"


class U8Type(IntegerType):
    """ Single byte integer type. Represents C chars """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return "B"


class U16Type(IntegerType):
    """ Double byte integer type. Represents C shorts """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">H"


class U32Type(IntegerType):
    """ Four byte integer type. Represents C unt32_t, """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">I"


class U64Type(IntegerType):
    """ Eight byte integer type. Represents C unt64_t, """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">Q"


class F32Type(FloatType):
    """ Eight byte integer type. Represents C unt64_t, """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">f"


class F64Type(FloatType):
    """ Eight byte integer type. Represents C unt64_t, """

    @staticmethod
    def get_serialize_format():
        """ Allows serialization using struct """
        return ">d"
