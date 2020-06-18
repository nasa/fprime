"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import NotInitializedException
from .type_exceptions import TypeException
from .type_exceptions import TypeMismatchException
from . import type_base


@type_base.serialize
@type_base.deserialize
class BoolType(type_base.BaseType):
    def __init__(self, val_=None):
        """
        Constructor
        """
        self.__val = val_
        if val_ == None:
            return

        self.__do_check = True
        self._check_val(val_)

    def _check_val(self, val_):
        # Make sure deserialize can set val
        if val_ is not None and self.__do_check:
            if not type(val_) == type(bool()):
                raise TypeMismatchException(type(bool()), type(val_))

    @property
    def val(self):
        return self.__val

    @val.setter
    def val(self, val_):
        self._check_val(val_)
        self.__val = val_

    def serialize(self):
        if self.val == None:
            raise NotInitializedException(type(self))

        if self.val:
            return struct.pack("B", 0xFF)
        else:
            return struct.pack("B", 0x00)

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        self.__do_check = False
        self._deserialize("B", data, offset)
        self.__do_check = True

        if self.val == 0xFF:
            self.val = True
        else:
            self.val = False

    def getSize(self):
        return struct.calcsize("B")

    def __repr__(self):
        return "Bool"


if __name__ == "__main__":
    print("bool")
    try:
        val = BoolType(True)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        val2 = BoolType()
        val2.deserialize(buff, len(buff))
        print("Deserialized: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
