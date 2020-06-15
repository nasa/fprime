'''
Created on May 29, 2020
@author: jishii
'''
from __future__ import print_function
from __future__ import absolute_import
import struct
from .type_exceptions import *
from . import type_base
from . import u32_type
from . import string_type
from . import enum_type

@type_base.serialize
@type_base.deserialize
class ArrayType(type_base.BaseType):
    """
    Representation of the Array type (comparable to the ANY type)

    The serializable type is a container for other instances of
    BaseType, including itself.

    @param param: typename = "SomeTypeName" string

    Configuration data for array type (the type of members, size of array, and format string for all members)
    @param param: config_info = (type, size, format)

    @param param: default_list = [ values... ]
    """
    def __init__(self, typename, config_info, default_list = None):
        """
        Constructor
        """

        if not type(typename) == type(str()):
            raise TypeMismatchException(type(str()),type(typename))

        self.__typename = typename
        setattr(self, "default_list", None)

        self.__arr_type = config_info[0]
        self.__arr_size = config_info[1]
        self.__arr_format = config_info[2]

        if default_list == None or len(default_list) == 0:
            return;

        if not type(default_list) == type(list()):
            raise TypeMismatchException(type(list()),type(default_list))
            
        self.__default_list = default_list

    def to_jsonable(self):
        """
        JSONable type
        """
        members = {}
        members["defaults"] = []
        for member in self.default_list:
            members["defaults"].append(member.to_jsonable())
        return members

    @property
    def mem_list(self):
        return self.__mem_list


    @default_list.setter
    def default_list(self, ml):
        """
        @todo: add arg type checking
        """
        self.__default_list = ml


    def serialize(self):
        if self.mem_list == None:
            raise NotInitializedException(type(self))

        # iterate through members and serialize each one
        arrStream = ""
        for val in self.default_list:
            arrStream += val.serialize()

        return arrStream


    def deserialize(self, data, offset):
        self.__val = []
        for arrVal in self.mem_list:
            arrVal.deserialize(data, offset)
            self.__val.append(arrVal.val)
            offset += arrVal.getSize()

    @property
    def arr_type(self):
        return self.__arr_type

    @property
    def arr_size(self):
        return self.__arr_size

    @property
    def arr_format(self):
        return self.__arr_format

    def getSize(self):
        total = 0
        for val in self.__default_list:
            total += val.getSize()
        return total

if __name__ == '__main__':
    print("Array")
    try:
        mem1 = u32_type.U32Type(100)
        mem2 = u32_type.U32Type(938)
        mem3 = u32_type.U32Type(22394)

        config_list = ("U32", 3, "%u")
        defaultList = [ mem1, mem2, mem3 ]

        arrType = SerializableType("ArrayType", config_list, defaultList)

        print("Value: %s" % repr(defaultList))
        buff = defaultType.serialize()
        type_base.showBytes(buff)
        arrType2 = ArrayType("ArrayType",defaultList)
        arrType2.deserialize(buff, len(buff))
        print("Deserialized: %s" % repr(arrType2.default_list))
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

