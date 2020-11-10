# ===============================================================================
# NAME: Port.py
#
# DESCRIPTION:  This is a Command meta-model sort of class.  It is
#               contained within a Component class as a list
#               of Command intances and is visited by the code
#               generators.
#
# USAGE:
#
# AUTHOR: tcanham
# EMAIL:  tcanham@jpl.nasa.gov
# DATE CREATED  : Feb. 11, 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging

#
# Python extention modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
class Command:
    """
    This is a very simple component meta-model class.
    Note: associations to Arg instances as needed.
    """

    def __init__(
        self,
        mnemonic,
        opcodes,
        args,
        sync=None,
        priority=None,
        comment=None,
        xml_filename=None,
        component_name=None,
        component_base_name=None,
        base_opcode=None,
        full=None,
    ):
        """
        Constructor
        @param mnemonic:  Name of command instance (each instance must be unique)
        @param opcodes:   list of opcodes for the command (if multiple instances)
        @param arg: list of command arguments
        @param sync:  Kind of command (must be one of: async, sync, or guarded)
        @param priority: priority of async command
        @param comment:  A single or multi-line comment describing the command
        @param xml_filename: XML filename where command is defined
        @param component_name: name of component type containing command
        """
        self.__mnemonic = mnemonic
        self.__opcodes = opcodes
        self.__arg_obj_list = args
        self.__sync = sync
        self.__priority = priority
        self.__comment = comment
        self.__xml_filename = xml_filename
        self.__component_name = component_name
        self.__component_base_name = component_base_name
        self.__base_opcode = base_opcode
        self.__full = full
        #

    #         self.__target_comp = None
    #         self.__target_command = None
    #         self.__target_type = None

    def get_xml_filename(self):
        """
        Returns the original xml filename
        """
        return self.__xml_filename

    def get_mnemonic(self):
        """
        Returns the command mnemonic
        """
        return self.__mnemonic

    def get_opcodes(self):
        """
        Returns the command opcode
        """
        return self.__opcodes

    def get_sync(self):
        """
        Returns type of command.
        """
        return self.__sync

    def get_priority(self):
        """
        Returns priority of command.
        """
        return self.__priority

    def get_full(self):
        """
        Returns queue full behavior of command.
        """
        return self.__full

    def get_comment(self):
        """
        Returns comment for the command.
        """
        return self.__comment

    def get_args(self):
        """
        Return a list of Arg objects for each argument.
        """
        return self.__arg_obj_list

    def get_component_name(self):
        return self.__component_name

    def get_component_base_name(self):
        return self.__component_base_name

    def get_base_opcode(self):
        return self.__base_opcode


#     def get_target_comp(self):
#         return self.__target_comp
#
#     def get_target_command(self):
#         return self.__target_command
#
#     def get_target_type(self):
#         return self.__target_type
#
#     def set_target_comp(self, comp):
#         self.__target_comp = comp
#
#     def set_target_command(self, command):
#         self.__target_command = command
#
#     def set_target_type(self, type):
#         self.__target_type = type
