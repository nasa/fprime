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
    
    The enumeration takes a dictionary that stores the enumeration members
    and current value as a string. The member values will have to be computed
    containing code based on C enum rules
    @param: typename = "SomeTypeName" string  Name of the type.
    @param: enum_dict = { "member":val, ...}  member value dict.
    @param: val = "member name" Optional member name for serializing only.
    """
    def __init__(self, typename="", enum_dict={"UNDEFINED":0}, val = None):
        """
        Constructor
        """
        # Check input value for member selected
        if val != None:
            if not type(val) == type(str()):
                raise TypeMismatchException(type(str()),type(val))
        else:
            val = "UNDEFINED"
        self.__val = val

        # Check type of typename
        if not type(typename) == type(str()):
            raise TypeMismatchException(type(str()), type(val))

        self.__typename = typename

        # Check enum is a dict
        if not type(enum_dict) == type(dict()):
            raise TypeMismatchException(type(dict()),type(val))

        # scan the dictionary to see if it is formatted correctly
        for member in list(enum_dict.keys()):
            # member name should be a string
            if not type(member) == type(str()):
                raise TypeMismatchException(type(str()),type(member))
            # member value should be an integer
            if not type(enum_dict[member]) == type(int()):
                raise TypeMismatchException(type(int()),enum_dict[member])

        self.__enum_dict = enum_dict
        self.__do_check = True

        self._check_val(val)

    def _check_val(self, val):
      # make sure requested value is found in enum members
      if val != "UNDEFINED" and self.__do_check:
          if val not in list(self.__enum_dict.keys()):
              raise EnumMismatchException(self.__typename, val)

    @property
    def val(self):
        return self.__val


    @val.setter
    def val(self, val):
        self._check_val(val)
        self.__val = val


    def keys(self):
        """
        Return all the enum key values.
        """
        return list(self.__enum_dict.keys())


    def typename(self):
        return self.__typename


    def enum_dict(self):
        return self.__enum_dict


    def serialize(self):
        """
        Utilize serialize decorator here...
        """
        # for enums, take the string value and convert it to
        # the numeric equivalent
        if self.val == None:
            raise NotInitializedException(type(self))
        #
        return self._serialize('>i', self.__enum_dict[self.val])


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


    def getSize(self):
        # enum value stored as unsigned int
        #return struct.calcsize('>i');
        return 4;

    def __repr__(self): return 'Enum'

if __name__ == '__main__':
    print("ENUM")
    try:
        members = { "MEMB1":0 , "MEMB2":6, "MEMB3":9 }
        print("Members: ", members)
        val = EnumType("SomeEnum",members,"MEMB3")
        print("Value: %s" % val.val)
        buff = val.serialize()
        type_base.showBytes(buff)
        val2 = EnumType("SomeEnum",members)
        val2.deserialize(buff,len(buff))
        print("Deserialize: %s" % val2.val)
    except TypeException as e:
        print("Exception: %s"%e.getMsg())

