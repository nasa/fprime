'''
@brief Command Template class

Instances of this class describe a specific command type.

@data Created July 2, 2018
@author Josef Biberstein

@bug No known bugs
'''
from enum import Enum
import data_template
from models.serialize.type_base import BaseType
from models.serialize.type_exceptions import *



class CmdTemplate(data_template.DataTemplate):

    def __init__(self, opcode, mnemonic, component, arguments,
                 description=None):
        '''
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
        '''
        # Make sure correct types are passed
        if not type(component) == type(str()):
            raise TypeMismatchException(type(str()),type(component))

        if not type(mnemonic) == type(str()):
            raise TypeMismatchException(type(str()),type(mnemonic))

        if not type(opcode) == type(int()):
            raise TypeMismatchException(type(int()),type(opcode))

        if description != None and not type(description) == type(str()):
            raise TypeMismatchException(type(str()),type(description))

        if not type(arguments) == type(list()):
            raise TypeMismatchException(type(list()),type(arguments))

        for (argname,argdesc,argtype) in arguments:
            #
            if not type(argname) == type(str()):
                raise TypeMismatchException(type(int()),type(argname))
            #
            if argdesc != None and not type(argdesc) == type(str()):
                raise TypeMismatchException(type(int()),type(argdesc))
            #
            if not issubclass(type(argtype), type(BaseType())):
                raise TypeMismatchException(type(BaseType()),type(argtype))

        # Initialize command internal variables
        self.component = component
        self.mnemonic = mnemonic
        self.opcode = opcode
        self.description = description
        self.arguments = arguments


    def get_component(self):
        return self.component

    def get_mnemonic(self):
        return self.mnemonic

    def get_op_code(self):
        return self.opcode

    def get_description(self):
        return self.description

    def get_args(self):
        '''
        Returns a list of argument information

        Returns:
            A list of tuples where each tuple represents an argument. Each tuple
            in the form: (arg name, arg description, arg obj). Where arg obj is
            an object of a type derived from the class Base Type. Arg
            description may be None.
        '''
        return self.arguments
