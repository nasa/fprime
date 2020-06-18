"""
Created on Dec 18, 2014

@author: tcanham, reder
"""
from __future__ import print_function
from __future__ import absolute_import
from .type_exceptions import TypeException
from .type_exceptions import TypeMismatchException
from .type_exceptions import TypeRangeException
from . import type_base


@type_base.serialize
@type_base.deserialize
class U8Type(type_base.BaseType):
    """
    Representation of the U8 type
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
        if not type(val_) == type(int()):
            raise TypeMismatchException(type(int()), type(val_))

        # check range
        if (val_ < 0) or (val_ > pow(2, 8) - 1):
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
        return self._serialize("B")

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        self._deserialize("B", data, offset)

    def getSize(self):
        return 1

    def __repr__(self):
        return "U8"


if __name__ == "__main__":
    print("U8")
    try:
        val = U8Type(8)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ", repr(buff))
        val2 = U8Type()
        val2.deserialize(buff, len(buff))
        print("Deserialized: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
