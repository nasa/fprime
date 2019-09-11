#!/bin/env python
#===============================================================================
# NAME: CommandLoader.py
#
# DESCRIPTION: This module contains a singleton for loading automatically
#              generated Python descriptor files.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Jan. 9, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import copy
import glob
import os
import sys

from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.common.models.common import command
from fprime.common.models.serialize import enum_type

class CommandLoader(object):
    '''
    Singleton loader class used to capture commands descriptor
    files and instance Command objects.  The objects are
    then stored in a dictionary of form { MNEMONIC : Command Instance }
    so the views can all use this for initialization.
    '''
    __instance = None

    def __init__(self):
        '''
        Constructor
        '''
        # Main dictionaries with key always mnemonic name.
        self.__dict_component = dict()
        self.__dict_opcode = dict()
        self.__dict_desc = dict()
        self.__dict_args = dict()
        self.__dict_commands = dict()
        self.__dict_commands_by_id = dict()

    def getInstance():
        """
        Return instance of singleton.
        """
        if(CommandLoader.__instance is None):
            CommandLoader.__instance = CommandLoader()
        return CommandLoader.__instance

    #define static method
    getInstance = staticmethod(getInstance)

    #@todo: refactor using the module_loader decorator here...
    def create(self, generated_command_path):
        """
        Generate all the dictionaries and instances of Command here...
        """
        # First add generated/commands to the python path
        if not os.path.isdir(generated_command_path):
            raise exceptions.GseControllerUndefinedDirectoryException(generated_command_path)
        # Copy the system path, to replace with when finished adjusting and loading
        old_paths = copy.deepcopy(sys.path)
        try:
            sys.path.append(generated_command_path)

            # Get the modules
            cmd_modules = glob.glob(generated_command_path + os.sep + '*.py')
            # Iterate over them and import them
            #
            file_list = []
            for cmd_imp in cmd_modules:
                fi = os.path.split(cmd_imp)[-1]
                if fi != "__init__.py":
                    file_list.append(fi)
            # Import the modules here
            #
            module_list = []
            for m in file_list:
                m = m[:-3]
                exec("import %s" % m)
                reload(eval(m))
                module_list.append(m)
            # Generate required dictionaries here
            #
            for m in module_list:
                m2 = eval("%s.MNEMONIC" % m)
                if m != m2:
                    raise exceptions.GseControllerMnemonicMismatchException(m, m2)
                self.__dict_component[m] = eval("%s.COMPONENT" % m)
                self.__dict_opcode[m]    = eval("%s.OP_CODE" % m)
                self.__dict_desc[m]      = eval("%s.CMD_DESCRIPTION" % m)
                self.__dict_args[m]      = eval("%s.ARGUMENTS" % m)
                #
                # Instance Command objects here...
                #
                self.__dict_commands[m] = \
                    command.Command(self.__dict_component[m], \
                                            m, \
                                            self.__dict_opcode[m], \
                                            self.__dict_desc[m], \
                                            self.__dict_args[m])

            for mnemonic in self.__dict_commands.keys():
                cmd_args = self.__dict_commands[mnemonic].getArgs()
                new_cmd_args = cmd_args
                # update the command objects if necessary
                for i in range(len(cmd_args)):
                    if type(cmd_args[i][2]) == type(enum_type.EnumType()):
                        typename = cmd_args[i][2].typename()
                        enum_dict = cmd_args[i][2].enum_dict()
                        val = cmd_args[i][2].enum_dict().keys()[0]
                        new_enum_obj = enum_type.EnumType(typename, enum_dict, val)
                        new_cmd_args[i] = (new_cmd_args[i][0],new_cmd_args[i][1],new_enum_obj)
            for name, id in self.__dict_opcode.iteritems():
                self.__dict_commands_by_id[id] = self.__dict_commands[name]
        finally:
            sys.path = old_paths

    def getComponentsDict(self):
        return self.__dict_component

    def getOpCodeDict(self):
        return self.__dict_opcode

    def getDescDict(self):
        return self.__dict_desc

    def getArgsDict(self):
        return self.__dict_args

    def getCommandDict(self):
        return self.__dict_commands

    def setCommandDict(self, cmd_dict):
        self.__dict_commands = cmd_dict

    def getCommandObj(self, mnemonic):
        return self.__dict_commands[mnemonic]

    def setCommandObj(self, mnemonic, obj):
        self.__dict_commands[mnemonic] = obj

    def getCommandObjById(self, id):
        return self.__dict_commands_by_id[id]
