"""
Created on Jan 5, 2015

@author: tcanham
"""
from __future__ import print_function
from __future__ import absolute_import

# Import the types this way so they do not need prefixing for execution.
# Import only types that you will use to avoid pylint warnings
from type_exceptions import TypeMismatchException
from type_exceptions import TypeException
from type_exceptions import ArgLengthMismatchException
from type_exceptions import ArgNotFoundException

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize.type_base import BaseType

from fprime.common.models.serialize.f32_type import F32Type
from fprime.common.models.serialize.u32_type import U32Type

from enum import Enum

import copy

Descriptor = Enum(value="Descriptor", names="ABSOLUTE RELATIVE")


class Command(object):
    """
    classdocs
    """

    def __init__(
        self,
        component,
        mnemonic,
        opcode,
        description,
        arguments,
        seconds=0,
        useconds=0,
        descriptor=Descriptor.RELATIVE,
    ):
        """
        Constructor
        """

        ## Make sure correct types are passed

        if not type(component) == type(str()):
            raise TypeMismatchException(type(str()), type(component))

        if not type(mnemonic) == type(str()):
            raise TypeMismatchException(type(str()), type(mnemonic))

        if not type(opcode) == type(int()):
            raise TypeMismatchException(type(int()), type(opcode))

        if not type(description) == type(str()):
            raise TypeMismatchException(type(str()), type(description))

        if not type(arguments) == type(list()):
            raise TypeMismatchException(type(list()), type(arguments))

        for (argname, argdesc, argtype) in arguments:
            #
            if not type(argname) == type(str()):
                raise TypeMismatchException(type(int()), type(argname))
            #
            if not type(argdesc) == type(str()):
                raise TypeMismatchException(type(int()), type(argdesc))
            #
            if not issubclass(type(argtype), type(BaseType())):
                raise TypeMismatchException(type(BaseType()), type(argtype))

        # Initialize command internal variables
        self.__component = component
        self.__mnemonic = mnemonic
        self.__opcode = opcode
        self.__description = description
        self.__arguments = arguments

        # If part of a sequence we need to set these as well
        self.setSeconds(seconds)
        self.setUseconds(useconds)
        self.setDescriptor(descriptor)

    def serialize(self):
        """
        Serializes command arguments
        """
        ## first, serialize opcode
        opcode = U32Type(self.__opcode)
        ser_data = opcode.serialize()

        ## then, serialize arguments
        for (arg_name, arg_desc, arg_type) in self.__arguments:
            ser_data += arg_type.serialize()
        return ser_data

    def getComponent(self):
        return self.__component

    def getMnemonic(self):
        return self.__mnemonic

    def getOpCode(self):
        return self.__opcode

    def getDescription(self):
        return self.__description

    def getSeconds(self):
        return self.__secs

    def getUseconds(self):
        return self.__usecs

    def getDescriptor(self):
        return self.__desc

    def getArgs(self):
        return self.__arguments

    def setSeconds(self, seconds):
        if not type(seconds) == type(int()):
            raise TypeMismatchException(type(int()), type(seconds))
        self.__secs = seconds

    def setUseconds(self, useconds):
        if not type(useconds) == type(int()):
            raise TypeMismatchException(type(int()), type(useconds))
        self.__usecs = useconds

    def setDescriptor(self, descriptor):
        if not type(descriptor) == type(Descriptor.ABSOLUTE):
            raise TypeMismatchException(type(Descriptor.ABSOLUTE), type(descriptor))
        self.__desc = descriptor

    ## value needs to be the Python class, not the raw value
    def setArg(self, arg_name, arg_type):
        """
        Method to set up an argument value of any
        of the types specified in serialize package.
        @param arg_name: string name of the argument.
        @param arg_type: object type to store arugment value in.
        """
        ### double check argument types
        if not type(arg_name) == type(str()):
            raise TypeMismatchException(type(str()), type(arg_name))

        if not issubclass(type(arg_type), type(BaseType())):
            raise TypeMismatchException(type(BaseType()), type(arg_type))

        new_arg_list = list()
        found = False
        ### search for argument
        for (arg, arg_desc, arg_value) in self.__arguments:
            if arg_name == arg:
                arg_value = arg_type
                found = True
            new_arg_list.append((arg, arg_desc, arg_value))

        if not found:
            raise ArgNotFoundException(arg_name)
        self.__arguments = new_arg_list

    def setArgs(self, values):
        """
      Given a list of values for the arguments, set the command arguments in order.
      @param values: raw python values (floats, ints, and strings) that will be converted
      into arg_types and stored as the command arg
      """

        # Make sure that the correct number of arguments is given
        if len(values) != len(self.__arguments):
            raise ArgLengthMismatchException(len(self.__arguments), len(values))

        # Set the new arguments by converting each value to an type:
        new_arg_list = list()
        for value, (arg_name, arg_desc, arg_value) in zip(values, self.__arguments):
            new_value = copy.deepcopy(arg_value)
            new_value.val = value
            new_arg_list.append((arg_name, arg_desc, new_value))
        self.__arguments = new_arg_list


if __name__ == "__main__":

    arglist = [
        ("arg1", "some test argument", U32Type(0)),
        ("arg2", "some test argument2", F32Type(0.0)),
    ]

    try:
        testCommand = Command(
            "SomeComponent", "TEST_CMD", 0x123, "Test Command", arglist
        )
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
    t = U32Type(3)
    t2 = F32Type(123.456)
    try:
        testCommand.setArg("arg1", t)
        testCommand.setArg("arg2", t2)
    except TypeException as e:
        print("Exception: %s" % e.getMsg())

    data = testCommand.serialize()
    type_base.showBytes(data)
