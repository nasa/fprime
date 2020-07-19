"""
int_types.py:

A file that contains the definitions for all the integer types provided as part of F prime.  F prime supports integers
that map to stdint.h integer sizes, that is, 8-bit, 16-bit, 32-bit, and 64-bit signed and unsigned integers.

@author mstarch
"""
import re
import abc
import struct

from .type_base import ValueType
from .type_exceptions import TypeMismatchException, TypeRangeException

BITS_RE = re.compile(r"[IU](\d\d?)")


@abc.ABC
class IntegerType(ValueType, abc.ABC):
    """ Base class thar represents all integer common functions """

    def __get_integer_bits(self):
        """ Gets the integer bits of a given type """
        my_type = type(self)
        match = BITS_RE.match(my_type)
        assert match, "Type {} does not follow format I#Type nor U#Type required of integer types".format(my_type)
        return int(match.group(1))

    def validate(self, val):
        """ Validates the given integer. """
        if not isinstance(val, int):
            raise TypeMismatchException(int, type(val))
        min_val = 0
        max_val = 1 << self.__get_integer_bits()
        if type(self).startswith("I"):
            min_val -= int(max_val/2)
            max_val -= int(max_val/2)
        if val < min_val or val >= max_val:
            raise TypeRangeException(val)

    def getSize(self):
        """ Gets the size of the integer based on the size specified in the class name """
        return int(self.__get_integer_bits()/8)


# class I8Type(IntegerType):
#     """ Single byte integer type. Represents C chars """
#
#
# class I16Type(IntegerType):
#     """ Double byte integer type. Represents C shorts """
#
#
# class I32Type(IntegerType):
#     """ Four byte integer type. Represents C int32_t, """
#
#
# class I64Type(IntegerType):
#     """ Eight byte integer type. Represents C int64_t, """
#
#
# class U8Type(IntegerType):
#     """ Single byte integer type. Represents C chars """
#
#
# class U16Type(IntegerType):
#     """ Double byte integer type. Represents C shorts """
#
#
# class U32Type(IntegerType):
#     """ Four byte integer type. Represents C unt32_t, """
#
#
# class U64Type(IntegerType):
#     """ Eight byte integer type. Represents C unt64_t, """
@type_base.serialize
@type_base.deserialize
class I8Type(type_base.BaseType):
    """
    Representation of the I8 type
    """

    def __init__(self, val=None):
        """
        Constructor
        """
        super().__init__()
        self.__val = val
        if val == None:
            return

        self._check_val(val)

    def _check_val(self, val):
        if not type(val) == type(int()):
            raise TypeMismatchException(type(int()), type(val))

        # check range
        if (val < -pow(2, 7)) or (val > pow(2, 7) - 1):
            raise TypeRangeException(val)

    @property
    def val(self):
        return self.__val

    @val.setter
    def val(self, val):
        self._check_val(val)
        self.__val = val

    def serialize(self):
        """
        Utilize serialize decorator here...
        """
        return self._serialize("b")

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        self._deserialize("b", data, offset)

    def getSize(self):
        return struct.calcsize("b")

    def __repr__(self):
        return "I8"