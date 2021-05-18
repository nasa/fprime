"""
Created on Dec 18, 2014

@author: reder
Replaced type base class with decorators
"""
import abc
import struct

from .type_exceptions import AbstractMethodException


class BaseType(abc.ABC):
    """
    An abstract base defining the methods supported by all base classes.
    """

    @abc.abstractmethod
    def serialize(self):
        """
        Serializes the current object type.
        """
        raise AbstractMethodException("serialize")

    @abc.abstractmethod
    def deserialize(self, data, offset):
        """
        AbstractDeserialize interface
        """
        raise AbstractMethodException("deserialize")

    @abc.abstractmethod
    def getSize(self):
        """
        Abstract getSize interface
        """
        raise AbstractMethodException("getSize")

    def __repr__(self):
        """ Produces a string representation of a given type """
        return self.__class__.__name__.replace("Type", "")

    @abc.abstractmethod
    def to_jsonable(self):
        """
        Converts this type to a JSON serializable object
        """
        raise AbstractMethodException("to_jsonable")


class ValueType(BaseType):
    """
    An abstract base type used to represent a single value. This defines the value property, allowing for setting and
    reading from the .val member.
    """

    def __init__(self, val=None):
        """ Defines the single value """
        self.__val = None
        # Run full setter
        if val is not None:
            self.val = val

    @abc.abstractmethod
    def validate(self, val):
        """
        Checks the val for validity with respect to the current type. This will raise TypeMismatchException when the
        validation fails of the val's type fails. It will raise TypeRangeException when val is out of range.

        :param val: value to validate
        :raises TypeMismatchException: value has incorrect type, TypeRangeException: val is out of range
        """

    @property
    def val(self):
        """ Getter for .val """
        return self.__val

    @val.setter
    def val(self, val):
        """ Setter for .val calls validate internally """
        self.validate(val)
        self.__val = val

    def to_jsonable(self):
        """
        Converts this type to a JSON serializable object
        """
        return {"value": self.val, "type": str(self)}


#
#
def showBytes(byteBuffer):
    """
    Routine to show bytes in buffer for testing.
    """
    print("Byte buffer size: %d" % len(byteBuffer))
    for entry in range(0, len(byteBuffer)):
        print(
            "Byte %d: 0x%02X (%c)"
            % (
                entry,
                struct.unpack("B", bytes([byteBuffer[entry]]))[0],
                struct.unpack("B", bytes([byteBuffer[entry]]))[0],
            )
        )
