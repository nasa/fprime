'''
Created on Dec 18, 2014

@author: tcanham, reder

'''
import struct
from type_exceptions import *
import type_base

@type_base.serialize
@type_base.deserialize
class I32Type(type_base.BaseType):
    """
    Representation of the I32 type
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
        if (val < -pow(2,31)) or (val > pow(2,31)-1):
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
        return self._serialize('>i')

    def deserialize(self,data,offset):
        """
        Utilize deserialized decorator here...
        """
        self._deserialize('>i', data, offset)

    def getSize(self):
        return struct.calcsize('>i');

    def __repr__(self): return 'I32'


if __name__ == '__main__':
    print "I32"
    try:
        val = I32Type(-2000000)
        print "Value: %s" % str(val.val)
        buff = val.serialize()
        type_base.showBytes(buff)
        print "Serialized: ",repr(buff)
        val2 = I32Type()
        val2.deserialize(buff,len(buff))
        print "Deserialize: %s" % str(val2.val)
        #
        val3 = I32Type(1000000)
        print "Value: %s" % str(val3.val)
        buff = val3.serialize()
        type_base.showBytes(buff)
        print "Serialized: ",repr(buff)
        val4 = I32Type()
        val4.deserialize(buff,len(buff))
        print "Deserialize: %s" % str(val4.val)

    except TypeException as e:
        print "Exception: %s" % e.getMsg()
