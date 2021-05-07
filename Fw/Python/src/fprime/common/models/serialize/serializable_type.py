"""
Created on Dec 18, 2014

@author: tcanham

"""
import copy

from .type_base import BaseType, ValueType
from .type_exceptions import NotInitializedException, TypeMismatchException


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

    def __init__(self, typename, mem_list=None):
        """
        Constructor
        """
        super().__init__()
        if not isinstance(typename, str):
            raise TypeMismatchException(str, type(typename))
        self.__typename = typename

        new_mem_list = []
        # If the member list is defined, stamp in None for any missing descriptions
        if mem_list:
            new_mem_list = [
                entry if len(entry) == 4 else (entry[0], entry[1], entry[2], None)
                for entry in mem_list
            ]
        # Set the member list then set the value
        self.mem_list = new_mem_list

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
            elif not isinstance(member_val, BaseType):
                raise TypeMismatchException(BaseType, type(member_val))
            elif not isinstance(format_string, str):
                raise TypeMismatchException(str, type(format_string))
            elif description is not None and not isinstance(description, str):
                raise TypeMismatchException(str, type(description))
        # When a value is set and is not empty we need to set the member properties
        if not val:
            return
        # If a value is supplied then check each value against the member list
        for val_member, list_entry in zip(val, self.mem_list):
            _, member_list_val, _, _ = list_entry
            member_list_val.validate(
                val_member
            )  # Insure that the the val_member is consistent with the existing member

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
            self.validate(self.mem_list)

    def serialize(self):
        """ Serializes the members of the serializable """
        if self.mem_list is None:
            raise NotInitializedException(type(self))
        return b"".join(
            [member_val.serialize() for _, member_val, _, _ in self.mem_list]
        )

    def deserialize(self, data, offset):
        """ Deserialize the values of each of the members """
        new_member_list = []
        for entry1, member_val, entry3, entry4 in self.mem_list:
            cloned = copy.copy(member_val)
            cloned.deserialize(data, offset)
            new_member_list.append((entry1, cloned, entry3, entry4))
            offset += member_val.getSize()
        self.mem_list = new_member_list

    @property
    def val(self) -> dict:
        """
        The .val property typically returns the python-native type. This the python native type closes to a serializable
        without generating full classes would be a dictionary (anonymous object). This returns such an object.

        :return dictionary of member names to python values of member keys
        """
        return {
            member_name: member_val.val
            for member_name, member_val, _, _ in self.mem_list
        }

    @val.setter
    def val(self, val: dict):
        """
        The .val property typically returns the python-native type. This the python native type closes to a serializable
        without generating full classes would be a dictionary (anonymous object). This takes such an object and sets the
        member val list from it.

        :param val: dictionary containing python types to key names. This
        """
        values_list = [val[name] for name, _, _, _ in self.mem_list]
        # Member list is the explicit store for storing these values
        for val_member, list_entry in zip(values_list, self.mem_list):
            _, member_list_val, _, _ = list_entry
            member_list_val.val = val_member

    def getSize(self):
        """ The size of a struct is the size of all the members """
        return sum([mem_type.getSize() for _, mem_type, _, _ in self.mem_list])

    def to_jsonable(self):
        """
        JSONable type for a serializable
        """
        members = {}
        for member_name, member_value, member_format, member_desc in self.mem_list:
            members[member_name] = {"format": member_format, "description": member_desc}
            members[member_name].update(member_value.to_jsonable())
        return members
