"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
import struct

from .type_base import ValueType
from .type_exceptions import (
    DeserializeException,
    EnumMismatchException,
    NotInitializedException,
    TypeMismatchException,
    TypeRangeException,
)


class EnumType(ValueType):
    """
    Representation of the ENUM type.

    The enumeration takes a dictionary that stores the enumeration members
    and current value as a string. The member values will have to be computed
    containing code based on C enum rules
    """

    def __init__(self, typename="", enum_dict=None, val=None):
        """
        Constructor
        :param typename: name of the enumeration type
        :param enum_dict: dictionary of value to integer representation
        :param val: value of the enumeration
        """
        super().__init__()
        if not isinstance(typename, str):
            raise TypeMismatchException(str, type(val))
        self.__typename = typename
        # Setup the enum dictionary
        if enum_dict is None:
            enum_dict = {"UNDEFINED": 0}
        # Check if the enum dict is an instance of dictionary
        self.__enum_dict = enum_dict
        # Set val to undefined if not set
        if val is None:
            val = "UNDEFINED"
        self.val = val

    def validate(self, val):
        """ Validate the value passed into the enumeration """
        if not isinstance(self.enum_dict(), dict):
            raise TypeMismatchException(dict, type(self.enum_dict()))
        for member in self.keys():
            if not isinstance(member, str):
                raise TypeMismatchException(str, type(member))
            elif not isinstance(self.enum_dict()[member], int):
                raise TypeMismatchException(int, self.enum_dict()[member])
        if val != "UNDEFINED" and val not in self.keys():
            raise EnumMismatchException(self.__typename, val)

    def keys(self):
        """
        Return all the enum key values.
        """
        return list(self.enum_dict().keys())

    def typename(self):
        return self.__typename

    def enum_dict(self):
        return self.__enum_dict

    def serialize(self):
        """
        Serialize the enumeration type using an int type
        """
        # for enums, take the string value and convert it to
        # the numeric equivalent
        if self.val is None:
            raise NotInitializedException(type(self))
        return struct.pack(">i", self.enum_dict()[self.val])

    def deserialize(self, data, offset):
        """
        Deserialize the enumeration using an int type
        """
        try:
            int_val = struct.unpack_from(">i", data, offset)[0]
        except:
            raise DeserializeException(
                "Could not deserialize enum value. Needed: {} bytes Found: {}".format(
                    self.getSize(), len(data[offset:])
                )
            )
        for key, val in self.enum_dict().items():
            if int_val == val:
                self.val = key
                break
        # Value not found, invalid enumeration value
        else:
            raise TypeRangeException(int_val)

    def getSize(self):
        """ Calculates the size based on the size of an integer used to store it """
        return struct.calcsize(">i")
