"""
Created on Dec 18, 2014

@author: tcanham

"""
from __future__ import print_function
from __future__ import absolute_import
from .type_exceptions import TypeMismatchException
from .type_exceptions import NotInitializedException
from . import type_base


@type_base.serialize
@type_base.deserialize
class SerializableType(type_base.BaseType):
    """
    Representation of the Serializable type (comparable to the ANY type)

    The serializable type is a container for other instances of
    BaseType, including itself.

    @param param: typename = "SomeTypeName" string
    To preserve member order, the member argument is a list of members and their types:
    @param param: mem_list = [ ("member",<ref to BaseType>, format string, description), ... ]
                  OR mem_list = [ ("member",<ref to BaseType>, format string), ... ].
                  The member descriptions can be None
    """

    def __init__(self, typename, mem_list=None):
        """
        Constructor
        """

        if not type(typename) == type(str()):
            raise TypeMismatchException(type(str()), type(typename))

        self.__typename = typename
        self.__val = []
        setattr(self, "mem_list", None)

        if mem_list == None or len(mem_list) == 0:
            return

        if not type(mem_list) == type(list()):
            raise TypeMismatchException(type(list()), type(mem_list))

        # Check if the member list contains a description or not
        if len(mem_list[0]) == 3:
            new_mem_list = []
            for (memberName, memberVal, format_string) in mem_list:
                new_mem_list.append((memberName, memberVal, format_string, None))

            mem_list = new_mem_list

        # scan the list to see if it has the correct types
        for (memberName, memberVal, format_string, desc) in mem_list:
            # member name should be a string
            if not type(memberName) == type(str()):
                raise TypeMismatchException(type(str()), type(memberName))
            # member value should be a derived class of TypeBase
            if not issubclass(type(memberVal), type_base.BaseType):
                raise TypeMismatchException(type(type_base.BaseType), type(memberVal))
            # format string should be string
            if not type(format_string) == type(str()):
                raise TypeMismatchException(type(str()), type(format_string))
            # Description should be a string
            if desc != None and not type(desc) == type(str()):
                raise TypeMismatchException(type(str()), type(desc))

        self.__mem_list = mem_list

    def to_jsonable(self):
        """
        JSONable type
        """
        members = {}
        for member in self.mem_list:
            members[member[0]] = {"format": member[2], "description": member[3]}
            members[member[0]].update(member[1].to_jsonable())
        return members

    @property
    def mem_list(self):
        return self.__mem_list

    @mem_list.setter
    def mem_list(self, ml):
        """
        @todo: add arg type checking
        """
        self.__mem_list = ml

    def serialize(self):
        if self.mem_list == None:
            raise NotInitializedException(type(self))

        # iterate through members and serialize each one
        serStream = b""
        for (memberName, memberVal, format_string, desc) in self.mem_list:
            serStream += memberVal.serialize()

        return serStream

    def deserialize(self, data, offset):
        self.__val = []
        for (memberName, memberVal, format_string, desc) in self.mem_list:
            memberVal.deserialize(data, offset)
            self.__val.append(memberVal.val)
            offset += memberVal.getSize()

    @property
    def val(self):
        return self.__val

    def getSize(self):
        size = 0
        for (memberName, memberVal, format_string, desc) in self.mem_list:
            size += memberVal.getSize()
        return size
