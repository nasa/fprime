"""
Created on May 29, 2020
@author: jishii
"""
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

    @param param: value_list = [ values... ] 
    Mandatory, array type must include a default

    @param param: type_data = (value_typename, format_string)
    """

    def __init__(self, typename, value_list, type_data):
        """
        Constructor
        """

        if not type(typename) == type(str()):
            raise TypeMismatchException(type(str()), type(typename))

        self.__typename = typename
        self.__val = []
        setattr(self, "value_list", None)

        self.__value_typename = type_data[0]
        self.__fmt_str = type_data[2]

        if value_list == None or len(value_list) == 0:
            return

        if not type(value_list) == type(list()):
            raise TypeMismatchException(type(list()), type(value_list))

        self.__value_list = value_list

    def to_jsonable(self):
        """
        JSONable type
        """
        members = []
        for val in self.__value_list:
            members.append(val.to_jsonable())
        return members

    @property
    def value_list(self):
        return self.__value_list

    @value_list.setter
    def value_list(self, vl):
        self.__value_list = vl

    @property
    def val(self):
        return self.__val

    def serialize(self):
        if self.__value_list == None:
            raise NotInitializedException(type(self))

        # iterate through members and serialize each one
        arrStream = b""
        for val in self.value_list:
            arrStream += val.serialize()

        return arrStream

    def deserialize(self, data, offset):
        self.__val = []
        for arrVal in self.value_list:
            arrVal.deserialize(data, offset)
            self.__val.append(arrVal.val)
            offset += arrVal.getSize()

    @property
    def fmt_str(self):
        return self.__fmt_str

    @property
    def value_typename(self):
        return self.__value_typename

    def getSize(self):
        total = 0
        for val in self.__value_list:
            total += val.getSize()
        return total