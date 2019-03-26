'''
Created on Dec 18, 2014

@author: reder
Replaced type base class with decorators
'''
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import *
#
#
class BaseType(object):
    """
    An abstract base class to define all type classes.
    """
    def __init__(self):
        """
        Constructor.
        """
        pass

    def serialize(self, *args):
        """
        AbstractSerialize interface
        """
        raise AbstractMethodException("serialize")

    def deserialize(self, *args):
        """
        AbstractDeserialize interface
        """
        raise AbstractMethodException("deserialize")

    def getSize(self):
        """
        Abstract getSize interface
        """
        raise AbstractMethodException("getSize")
#
#
def deserialize(Class):
    """
    Decorator adds common deserialize method
    """
    setattr(Class, "__val", None)
    def _deserialize(self,tformat,data,offset):
        if offset > (len(data) - len(data[offset:])):
            raise DeserializeException("Not enough data to deserialize! Needed: %d Left: %d" % (self.getSize(),offset))
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
    def _serialize(self,tformat, arg=None):
        if self.val == None:
            raise NotInitializedException(type(self))
        if arg == None:
            return struct.pack(tformat,self.val)
        else:
            return struct.pack(tformat,arg)
    setattr(Class, "_serialize", _serialize)
    return Class
#
#
def showBytes(byteBuffer):
    """
    Routine to show bytes in buffer for testing.
    """
    print("Byte buffer size: %d" % len(byteBuffer))
    for entry in range(0,len(byteBuffer)):
        print("Byte %d: 0x%02X (%c)"%(entry,struct.unpack("B",byteBuffer[entry])[0],struct.unpack("B",byteBuffer[entry])[0]))


if __name__ == '__main__':
    pass
