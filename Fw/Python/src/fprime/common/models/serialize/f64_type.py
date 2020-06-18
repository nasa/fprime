"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import TypeException
from .type_exceptions import TypeMismatchException
from . import type_base


@type_base.serialize
@type_base.deserialize
class F64Type(type_base.BaseType):
    """
    Representation of the F64 type
    """

    def __init__(self, val_=None):
        """
        Constructor
        """
        self.__val = val_
        if val_ == None:
            return

        self._check_val(val_)

    def _check_val(self, val_):
        if not type(val_) == type(float()):
            raise TypeMismatchException(type(float()), type(val_))

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
        return self._serialize(">d")

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        self._deserialize(">d", data, offset)

    def getSize(self):
        return struct.calcsize(">d")

    def __repr__(self):
        return "F64"


if __name__ == "__main__":
    print("F64")
    try:
        val = F64Type(100000.23)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = F64Type()
        val2.deserialize(buff, len(buff))
        print("Deserialize: %f" % val2.val)
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
