"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import print_function
from __future__ import absolute_import
from .type_exceptions import *
from . import type_base


@type_base.serialize
@type_base.deserialize
class U64Type(type_base.BaseType):
    """
    Representation of the U32 type
    """

    def __init__(self, val=None):
        """
        Constructor
        """
        self.__val = val
        if val == None:
            return

        self._check_val(val)

    def _check_val(self, val):  # Sometimes small values of long come back as int
        if not (type(val) == type(int()) or type(val) == type(int())):
            raise TypeMismatchException(type(int()), type(val))

        # check range
        if (val < 0) or (val > pow(2, 64) - 1):
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
