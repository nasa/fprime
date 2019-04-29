'''
Created on Dec 18, 2014
@author: tcanham, reder

'''
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import *
from . import type_base

@type_base.serialize
@type_base.deserialize
class I16Type(type_base.BaseType):
    """
    Representation of the I16 type
    """
    def __init__(self, val = None):
        """
        Constructor
        """
        self.__val = val
        if val == None:
            return;

        self._check_val(val)

    def _check_val(self, val):
        if not type(val) == type(int()):
            raise TypeMismatchException(type(int()),type(val))

        # check range
        if (val < -pow(2,15)) or (val > pow(2,15)-1):
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
        return self._serialize('>h')

    def deserialize(self,data,offset):
        """
        Utilize deserialized decorator here...
        """
        self._deserialize('>h', data, offset)

    def getSize(self):
        return struct.calcsize('>h');

    def __repr__(self): return 'I16'


if __name__ == '__main__':
    print("I16")
    try:
        val = I16Type(-2000)
        print("Value: %s" % str(val.val))
        buff = val.serialize()
        type_base.showBytes(buff)
        print("Serialized: ",repr(buff))
        val2 = I16Type()
        val2.deserialize(buff,len(buff))
        print("Deserialize: %s" % str(val2.val))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
