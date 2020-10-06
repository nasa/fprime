""" Generic representation of autocoded array types

Created on May 29, 2020
@author: jishii
"""
import copy

from .type_base import ValueType
from .type_exceptions import (
    ArrayLengthException,
    NotInitializedException,
    TypeMismatchException,
)


class ArrayType(ValueType):
    """ Generic fixed-size array type representation.

    Represents a custom named type of a fixed number of like members, each of which are other types in the system.
    """

    def __init__(self, typename, config_info, val=None):
        """ Constructs a new array type.

        Args:
            typename: name of this array type
            config_info: (type, size, format string) information for array
            val: (optional) list of values to assign to array
        """
        super().__init__()
        if not isinstance(typename, str):
            raise TypeMismatchException(str, type(typename))
        self.__val = None
        self.__typename = typename
        self.__arr_type, self.__arr_size, self.__arr_format = config_info
        # Set value only if it is a valid, non-empty list
        if not val:
            return
        self.val = val

    def validate(self, val):
        """ Validates the values of the array """
        size = self.__arr_size
        if len(val) != size:
            raise ArrayLengthException(self.__arr_type, size, len(val))
        for i in range(self.__arr_size):
            if not isinstance(val[i], type(self.__arr_type)):
                raise TypeMismatchException(type(self.__arr_type), type(val[i]))


    @property
    def val(self) -> list:
        """
        The .val property typically returns the python-native type. This the python native type closes to a serializable
        without generating full classes would be a dictionary (anonymous object). This returns such an object.

        :return dictionary of member names to python values of member keys
        """
        return [item.val for item in self.__val]

    @val.setter
    def val(self, val: list):
        """
        The .val property typically returns the python-native type. This the python native type closes to a serializable
        without generating full classes would be a dictionary (anonymous object). This takes such an object and sets the
        member val list from it.

        :param val: dictionary containing python types to key names. This
        """
        items = []
        for item in val:
            cloned = copy.copy(self.arr_type)
            cloned.val = item
            items.append(cloned)
        self.__val = items


    def to_jsonable(self):
        """
        JSONable type
        """
        members = {
            "name": self.__typename,
            "type": self.__typename,
            "size": self.__arr_size,
            "format": self.__arr_format,
            "values": None
            if self.__val is None
            else [member.to_jsonable() for member in self.__val],
        }
        return members

    def serialize(self):
        """ Serialize the array by serializing the elements one by one """
        if self.val is None:
            raise NotInitializedException(type(self))
        return b"".join([item.serialize() for item in self.val])

    def deserialize(self, data, offset):
        """ Deserialize the members of the array """
        values = []
        for i in range(self.__arr_size):
            item = copy.copy(self.arr_type)
            item.deserialize(data, offset + i * item.getSize())
            values.append(item.val)
        self.val = values

    @property
    def arr_type(self):
        """ Property representing the size of the array """
        return self.__arr_type

    @property
    def arr_size(self):
        """ Property representing the number of elements of the array """
        return self.__arr_size

    @property
    def arr_format(self):
        """ Property representing the format string of an item in the array """
        return self.__arr_format

    def getSize(self):
        """ Return the size of the array """
        return self.arr_type.getSize() * self.arr_size
