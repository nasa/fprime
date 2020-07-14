"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import absolute_import
import struct
from .type_exceptions import NotInitializedException
from .type_exceptions import TypeMismatchException
from . import type_base


@type_base.serialize
@type_base.deserialize
class BoolType(type_base.BaseType):
    def __init__(self, val=None):
        """
        Constructor
        """
        self.__val = val
        if val == None:
            return

        self.__do_check = True
        self._check_val(val)

    def _check_val(self, val):
        # Make sure deserialize can set val
        if val is not None and self.__do_check:
            if not type(val) == type(bool()):
                raise TypeMismatchException(type(bool()), type(val))

    @property
    def val(self):
        return self.__val

    @val.setter
    def val(self, val):
        self._check_val(val)
        self.__val = val

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
