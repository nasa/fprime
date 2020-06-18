"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import TypeException
from .type_exceptions import TypeMismatchException
from .type_exceptions import TypeRangeException
from . import type_base


@type_base.serialize
@type_base.deserialize
class U64Type(type_base.BaseType):
    """
    Representation of the U32 type
    """

    def __init__(self, val_=None):
        """
        Constructor
        """
        self.__val = val_
        if val_ == None:
            return

        self._check_val(val_)

    def _check_val(self, val_):  # Sometimes small values of long come back as int
        if not (type(val_) == type(int()) or type(val_) == type(int())):
            raise TypeMismatchException(type(int()), type(val_))

        # check range
        if (val_ < 0) or (val_ > pow(2, 64) - 1):
            raise TypeRangeException(val_)

    @property
    def val(self):
        return self.__val

    @val.setter
    def val(self, val_):
        self._check_val(val_)
        self.__val = val_

    def serialize(self):
        """
        Utilize serialize decorator here...
        """
        return self._serialize(">Q")

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        self._deserialize(">Q", data, offset)

    def getSize(self):
        return 8

    def __repr__(self):
        return "U64"


if __name__ == "__main__":

    def showBytes(byteBuffer):
        print("Byte buffer size: %d" % len(byteBuffer))
        for entry in range(0, len(byteBuffer)):
            print(
                "Byte %d: 0x%02X (%c)"
                % (
                    entry,
                    struct.unpack("B", byteBuffer[entry])[0],
                    struct.unpack("B", byteBuffer[entry])[0],
                )
            )

    print("U64")
    try:
        val = U64Type(1000000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = U64Type()
        val2.deserialize(buff, len(buff))
        print("Deserialized: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
