'''
Created on Dec 16, 2015
@author: dinkel
'''
from type_exceptions import *
import type_base
from u32_type import U32Type

class TimeType(type_base.BaseType):
    """
    Representation of the time type
    TODO: May need to come up with a better representatino for timeBase. Right now it is a U32, but in reality it will
    be a project defined enumeration
    """
    def __init__(self, seconds=0, useconds=0, timeBase=0 ):
        """
        Constructor
        """
        self.__secs = U32Type(seconds)
        self.__usecs = U32Type(useconds)
        self.__timeBase = U32Type(timeBase)

        self._check_useconds(useconds)

    def _check_useconds(self, useconds):
        # check range
        if (useconds < 0) or (useconds > 999999):
            raise TypeRangeException(usecs)

    @property
    def seconds(self):
        return self.__secs.val

    @seconds.setter
    def seconds(self, val):
        self.__secs = U32Type(val)

    @property
    def useconds(self):
        return self.__usecs.val

    @useconds.setter
    def useconds(self, val):
        self._check_useconds(val)
        self.__usecs = U32Type(val)

    @property
    def timeBase(self):
        return self.__timeBase.val

    @timeBase.setter
    def timeBase(self, val):
        self.__timeBase = U32Type(val)

    def serialize(self):
        """
        Utilize serialize decorator here...
        """
        buf = ""
        buf += self.__timeBase.serialize()
        buf += self.__secs.serialize()
        buf += self.__usecs.serialize()
        return buf

    def deserialize(self, data, offset):
        """
        Utilize deserialized decorator here...
        """
        ptr = 0
        self.__timeBase.deserialize(data, ptr)
        ptr += self.__timeBase.getSize()
        self.__secs.deserialize(data, ptr)
        ptr += self.__secs.getSize()
        self.__usecs.deserialize(data, ptr)

    def getSize(self):
        return self.__timeBase.getSize() + self.__secs.getSize() + self.__usecs.getSize()

    def getNumFields(self):
      return 3

    def __repr__(self): return 'Time'


if __name__ == '__main__':
    print "Time"
    try:
        val = TimeType(10, 9, 8)
        print "Base: %s" % str(val.timeBase)
        print "Seconds: %s" % str(val.seconds)
        print "Useconds: %s" % str(val.useconds)
        buff = val.serialize()
        type_base.showBytes(buff)
        print "Serialized: ",repr(buff)
        val2 = TimeType()
        val2.deserialize(buff,len(buff))
        print "Deserialize Base: %s" % str(val2.timeBase)
        print "Deserialize Seconds: %s" % str(val2.seconds)
        print "Deserialize Useconds: %s" % str(val2.useconds)
    except TypeException as e:
        print "Exception: %s"%e.getMsg()

