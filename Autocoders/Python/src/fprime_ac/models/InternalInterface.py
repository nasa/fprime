# ===============================================================================
# NAME:     InternalInterface.py
#
# DESCRIPTION:  This is a Command meta-model sort of class.  It is
#               contained within a Component class as a list
#               of InternalInterface instances and is visited by the code
#               generators.
#
# USAGE:
#
# AUTHOR: tcanham
# EMAIL:  tcanham@jpl.nasa.gov
# DATE CREATED  : Oct. 11, 2015
#
# Copyright 2015, California Institute of Technology.
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
class InternalInterface:
    """
    This is a very simple component meta-model class.
    Note: associations to Arg instances as needed.
    """

    def __init__(
        self,
        name,
        priority,
        full,
        args,
        comment=None,
        xml_filename=None,
        component_name=None,
    ):
        """
        Constructor
        @param pname:  Name of command instance (each instance must be unique)
        @param type:   Type of command (must have supporting include xml)
        @param direction: Direction of data flow (must be input or output)
        @param sync:  Kind of command (must be one of: async, sync, or guarded)
        @param comment:  A single or multi-line comment describing the command
        """
        self.__name = name
        self.__priority = priority
        self.__full = full
        self.__arg_obj_list = args
        self.__comment = comment
        self.__xml_filename = xml_filename
        self.__component_name = component_name
        #

    #         self.__target_comp = None
    #         self.__target_command = None
    #         self.__target_type = None

    def get_xml_filename(self):
        """
        Returns the original xml filename
        """
        return self.__xml_filename

    def get_name(self):
        """
        Returns the interface name
        """
        return self.__name

    def get_priority(self):
        """
        Returns priority of the interface.
        """
        return self.__priority

    def get_full(self):
        """
        Returns queue full behavior of the interface.
        """
        return self.__full

    def get_comment(self):
        """
        Returns comment for the interface.
        """
        return self.__comment

    def get_args(self):
        """
        Return a list of Arg objects for each argument.
        """
        return self.__arg_obj_list

    def get_component_name(self):
        return self.__component_name
