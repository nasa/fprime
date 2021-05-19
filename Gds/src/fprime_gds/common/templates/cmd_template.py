"""
@brief Command Template class

Instances of this class describe a specific command type.

@data Created July 2, 2018
@author Josef Biberstein

@bug No known bugs
"""

import copy

from fprime.common.models.serialize.type_base import BaseType
from fprime.common.models.serialize.type_exceptions import (
    ArgLengthMismatchException,
    TypeMismatchException,
)

from . import data_template


class CmdTemplate(data_template.DataTemplate):
    def __init__(self, opcode, mnemonic, component, arguments, description=None):
        """
        Constructor

        Args:
            opcode: The cmd's opcode (integer)
            mnemonic: The mnemonic used to refer to the cmd (string)
            component: The name of the component that produces the cmd (string)
            arguments: List of arguments in tuple form. Each tuple should be:
                       (arg name, arg description, arg obj). Where arg obj is an
                       object of a type derived from the class Base Type.
                       Arg description may be None.
            description: (Optional) Description of the cmd (string)
        """
        super().__init__()
        # Make sure correct types are passed
        if not isinstance(component, str):
            raise TypeMismatchException(str, type(component))

        if not isinstance(mnemonic, str):
            raise TypeMismatchException(str, type(mnemonic))

        if not isinstance(opcode, int):
            raise TypeMismatchException(int, type(opcode))

        if description is not None and not isinstance(description, str):
            raise TypeMismatchException(str, type(description))

        if not isinstance(arguments, list):
            raise TypeMismatchException(list, type(arguments))

        for (argname, argdesc, argtype) in arguments:
            #
            if not isinstance(argname, str):
                raise TypeMismatchException(int, type(argname))
            #
            if argdesc is not None and not isinstance(argdesc, str):
                raise TypeMismatchException(int, type(argdesc))
            #
            if not isinstance(argtype, BaseType):
                raise TypeMismatchException(BaseType, type(argtype))

        # Initialize command internal variables
        self.comp_name = component
        self.mnemonic = mnemonic
        self.opcode = opcode
        self.description = description
        self.arguments = arguments
        self.descriptor = None
        self.sec = None
        self.usec = None

    def get_full_name(self):
        """
        Get the full name of this event

        Returns:
            The full name (component.channel) for this event
        """
        return "{}.{}".format(self.comp_name, self.mnemonic)

    def get_comp_name(self):
        return self.comp_name

    def get_mnemonic(self):
        return self.mnemonic

    def get_op_code(self):
        return self.opcode

    def get_id(self):
        """
        Get the Command's id, which is its opcode.

        This function is implemented to maintain the sys_data interface

        Returns:
            The command's opcode
        """
        return self.opcode

    def get_name(self):
        """
        Get the Command's name, which is its mnemonic

        This function is implemented to maintain the sys_data interface

        Returns:
            The command's mnemonic
        """
        return self.mnemonic

    def get_description(self):
        return self.description

    def get_args(self):
        """
        Returns a list of argument information

        Returns:
            A list of tuples where each tuple represents an argument. Each tuple
            in the form: (arg name, arg description, arg obj). Where arg obj is
            an object of a type derived from the class Base Type. Arg
            description may be None.
        """
        return self.arguments

    # Methods to keep compatibility with the names used in
    # Gds/src/fprime_gds/common/encoders/seq_writer. This allows sharing
    # of the code.

    def setArgs(self, values):

        if len(values) != len(self.arguments):
            raise ArgLengthMismatchException(len(self.arguments), len(values))

        # set the values of the arguments
        new_arg_list = list()
        for value, (arg_name, arg_desc, arg_value) in zip(values, self.arguments):
            new_value = copy.deepcopy(arg_value)
            new_value.val = value
            new_arg_list.append((arg_name, arg_desc, new_value))

        self.arguments = new_arg_list

    def setDescriptor(self, desc):
        self.descriptor = desc

    def setSeconds(self, sec):
        self.sec = sec

    def setUseconds(self, usec):
        self.usec = usec

    def getOpCode(self):
        return self.opcode

    def getDescriptor(self):
        """
        Returns the time descriptor, relative or absolute

        Returns:
            A integer that indicates whether the command is relative or absolute
        """
        return self.descriptor

    def getSeconds(self):
        return self.sec

    def getUseconds(self):
        return self.usec

    def getArgs(self):
        return self.get_args()
