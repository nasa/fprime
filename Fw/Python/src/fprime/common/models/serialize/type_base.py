"""
Created on Dec 18, 2014

@author: reder
Replaced type base class with decorators
"""
import abc
import struct
from .type_exceptions import AbstractMethodException
from .type_exceptions import DeserializeException
from .type_exceptions import NotInitializedException


class BaseType(abc.ABC):
    """
    An abstract base defining the methods supported by all base classes.
    """

    @abc.abstractmethod
    def serialize(self):
        """
        Serializes the current object type.
        """
        pass

    @abc.abstractmethod
    def deserialize(self, offset):
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
        return type(self).replace("Type", "")

    def to_jsonable(self):
        """
        Converts this type to a JSON serializable object
        """
        if hasattr(self, "val"):
            return {"value": self.val, "type": str(self)}
        raise AbstractMethodException("to_jsonable")


class ValueType(BaseType, abc.ABC):
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
        Checks the val for validity with respect to the current type. This will raise TypeMissmatchException when the
        validation fails of the val's type fails. It will raise TypeRangeException when val is out of range.
        :param val: value to validate
        :raises TypeMismatchException: value has incorrect type, TypeRangeException: val is out of range
        """
        pass

    @property
    def val(self):
        """ Getter for .val """
        return self.__val

    @val.setter
    def val(self, val):
        """ Setter for .val calls validate internally """
        self.validate(val)
        self.__val = val


    @abc.abstractmethod
    @staticmethod
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



#
#
def deserialize(Class):
    """
    Decorator adds common deserialize method
    """
    setattr(Class, "__val", None)

    def _deserialize(self, tformat, data, offset):
        if offset > (len(data) - len(data[offset:])):
            raise DeserializeException(
                "Not enough data to deserialize! Needed: %d Left: %d"
                % (self.getSize(), offset)
            )
        self.val = struct.unpack_from(tformat, data, offset)[0]

    setattr(Class, "_deserialize", _deserialize)

    return Class


#
#
def serialize(Class):
    """
    Decorator adds common serialize method
    """
    setattr(Class, "__val", None)

    def _serialize(self, tformat, arg=None):
        if self.val == None:
            raise NotInitializedException(type(self))
        if arg == None:
            return struct.pack(tformat, self.val)
        else:
            return struct.pack(tformat, arg)

    setattr(Class, "_serialize", _serialize)
    return Class


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
