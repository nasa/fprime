'''
Created on May 29, 2020
@author: jishii
'''
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import *
from . import type_base

@type_base.serialize
@type_base.deserialize
class ArrayType(type_base.BaseType):
    """
    Representation of the ARRAY type

    The array takes a list that stores the array members.
    
    @param param: typename = "SomeTypeName" string
    @param param: mem_type = "TypeOfMembers" string
    @param param: mem_len = integer length of mem_list
    @param param: mem_list = ["member", ...]
    """
    def __init__(self, typename, mem_type, mem_len, mem_list=None):
        """
        Constructor
        """
        if not type(typename) == type(str()):
            raise TypeMismatchException(type(str()),type(typename))

        self.__typename = typename
        setattr(self, "mem_list", None)

        if mem_list == None or len(mem_list) == 0:
            return;

        if not type(mem_list) == type(list()):
            raise TypeMismatchException(type(list()),type(mem_list))

        if not len(mem_list) <= mem_len:
            raise ArrayLengthException(typename, mem_len, len(mem_list))

        # Check all members?

        self.__mem_type = mem_type
        self.__mem_len = mem_len
        self.__mem_list = mem_list

    def member(self, i):
        return self.__mem_list[i]

    def member(self, i, val):
        if len(self.__mem_list) >= i:
            self.__mem_list.append(val)
        else:
            self.__mem_list[i] = val

    def typename(self):
        return self.__typename

    def mem_type(self):
        return self.__mem_type

    def mem_len(self):
        return self.__mem_len

    def mem_list(self):
        return self.__mem_list


    def serialize(self):
        """
        Utilize serialize decorator here...
        """
        if self.mem_list == None:
            raise NotInitializedException(type(self))

        # iterate through members and serialize each one
        serStream = ""
        for member in self.mem_list:
            serStream += member.serialize()

        return serStream


    def deserialize(self,data,offset):
        """
        Utilize deserialized decorator here...
        """

        self.__do_check = False
        self._deserialize('>i', data, offset)
        self.__do_check = True
        # search for value in enum_list
        # print self.__enum_dict
        # print self.val
        #
        for member in list(self.__enum_dict.keys()):
            if self.__enum_dict[member] == self.val:
                self.val = member

        if self.val not in list(self.__enum_dict.keys()):
            raise TypeRangeException(self.val)

    def __repr__(self): return 'Array'

if __name__ == '__main__':
    print("ARRAY")
    try:
        members = list([0, 1, 2, 3, 4])
        print("Members: ", members)
        val = ArrayType("SomeArray","I32", 5, members)
        buff = val.serialize()
        type_base.showBytes(buff)
        val2 = ArrayType("SomeArray","I32", 5, members)
        val2.deserialize(buff,len(buff))
        print("Deserialize: %s" % val2.val)
    except TypeException as e:
        print("Exception: %s"%e.getMsg())

