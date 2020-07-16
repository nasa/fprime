"""
Created on Dec 18, 2014
@author: tcanham, reder
"""
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import TypeMismatchException
from . import type_base


@type_base.serialize
@type_base.deserialize
class F64Type(type_base.BaseType):
    """
    Representation of the F64 type
    """

    def __init__(self, val=None):
        """
        Constructor
        """
        super().__init__()
        self.__val = val
        if val == None:
            return

        self._check_val(val)

    def _check_val(self, val):
        if not type(val) == type(float()):
            raise TypeMismatchException(type(float()), type(val))

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
