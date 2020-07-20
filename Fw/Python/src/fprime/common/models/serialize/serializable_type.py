"""
Created on Dec 18, 2014

@author: tcanham

"""
import copy
from __future__ import print_function
from __future__ import absolute_import
from .type_exceptions import TypeMismatchException
from .type_exceptions import NotInitializedException
from .type_base import BaseType, ValueType


class SerializableType(ValueType):
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

    def __init__(self, typename, mem_list=None, val=None):
        """
        Constructor
        """
        super().__init__()
        if not isinstance(typename, str):
            raise TypeMismatchException(str, type(typename))
        self.__typename = typename
        # If the member list is defined, stamp in None for any missing descriptions
        if mem_list:
            new_mem_list = [entry if len(entry) == 4 else (entry[0], entry[1], entry[2], None) for entry in mem_list]
        # Set the member list then set the value
        self.mem_list = new_mem_list
        self.val = val

    def validate(self, val=None):
        """ Validate this object including member list and values """
        # Blank member list does not validate
        if not self.mem_list:
            return
        elif not isinstance(self.mem_list, list):
            raise TypeMismatchException(list, self.mem_list)
        for member_name, member_val, format_string, description in self.mem_list:
            # Check each of these members for correct types
            if not isinstance(member_name, str):
                raise TypeMismatchException(str, type(member_name))
            elif isinstance(member_val, BaseType):
                raise TypeMismatchException(BaseType, type(member_val))
            elif not isinstance(format_string, str):
                raise TypeMismatchException(str, type(format_string))
            elif not isinstance(description, str):
                raise TypeMismatchException(str, type(description))
        # When a value is set and is not empty we need to set the member properties
        if not val:
            return
        # If a value is supplied then check each value against the member list
        for val_member, list_entry in zip(val, self.mem_list):
            _, member_list_val, _, _ = list_entry
            member_list_val.validate(val_member) # Insure that the the val_member is consistent with the existing member

    @property
    def mem_list(self):
        """ Gets the member list"""
        return self.__mem_list

    @mem_list.setter
    def mem_list(self, mem_list):
        """
        Sets the member list and validates against the current value.
        """
        self.__mem_list = mem_list
        if mem_list is None:
            self.validate(self.val)

    def serialize(self):
        """ Serializes the members of the serializable """
        if self.mem_list is None:
            raise NotInitializedException(type(self))
        return b"".join([member_val.serialize() for _, member_val, _, _ in self.mem_list])

    def deserialize(self, data, offset):
        """ Deserialize the values of each of the members """
        members = []
        for _, member_val, _, _ in self.mem_list:
            member_val.deserialize(data, offset)
            members.append(member_val.val)
            offset += member_val.getSize()
        self.val = members

    @property
    def val(self):
        """ Getter for .val """
        return super().val

    @val.setter
    def val(self, val):
        """ Setter for .val calls validate internally """
        super().val = val
        # When a value is set, we need to set the member properties
        for val_member, list_entry in zip(val, self.mem_list):
            _, member_list_val, _, _ = list_entry
            list_entry.val = val_member

    def getSize(self):
        """ The size of a struct is the size of all the members """
        return sum([member.getSize() for member in self.mem_list])

    def to_jsonable(self):
        """
        JSONable type for a serializable
        """
        members = {}
        for member_name, member_value, member_format, member_desc in self.mem_list:
            members[member_name] = {"format": member_format, "description": member_desc}
            members[member_name].update(member_value.to_jsonable())
        return members
