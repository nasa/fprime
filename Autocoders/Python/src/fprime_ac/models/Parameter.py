# ===============================================================================
# NAME: Port.py
#
# DESCRIPTION:  This is a Parameter meta-model sort of class.  It is
#               contained within a Component class as a list
#               of Parameter intances and is visited by the code
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
class Parameter:
    """
    This is a very simple component meta-model class.
    Note: associations to Arg instances as needed.
    """

    def __init__(
        self,
        ids,
        name,
        ctype,
        set_opcodes,
        save_opcodes,
        default,
        size,
        comment=None,
        xml_filename=None,
        component_name=None,
        base_setop=None,
        base_saveop=None,
    ):
        """
        Constructor
        @param id:  Numeric ID of parameter
        @param name: Name of parameter
        @param ctype:   Type of parameter
        @param default: default value for parameter if unable to load
        @param comment:  A single or multi-line comment describing the parameter
        @param size: size of string, if string
        """
        self.__ids = ids
        self.__name = name
        self.__ctype = ctype
        self.__set_opcodes = set_opcodes
        self.__save_opcodes = save_opcodes
        self.__default = default
        self.__size = size
        self.__comment = comment
        self.__xml_filename = xml_filename
        self.__component_name = component_name
        self.__base_setop = base_setop
        self.__base_saveop = base_saveop
        #

    def get_ids(self):
        """
        Returns the parameter id
        """
        return self.__ids

    def get_name(self):
        """
        Returns the parameter name
        """
        return self.__name

    def get_type(self):
        """
        Return the parameter type.
        """
        return self.__ctype

    def get_set_opcodes(self):
        """
        Return the parameter set opcode
        """
        return self.__set_opcodes

    def get_save_opcodes(self):
        """
        Return the parameter save opcode
        """
        return self.__save_opcodes

    def get_default(self):
        """
        Return the parameter default value.
        """
        return self.__default

    def get_size(self):
        """
        Return the parameter size, if string.
        """
        return self.__size

    def get_comment(self):
        """
        Returns comment for the command.
        """
        return self.__comment

    def get_xml_filename(self):
        """
        Returns the original xml filename
        """
        return self.__xml_filename

    def get_component_name(self):
        return self.__component_name

    def get_component_base_name(self):
        """
        Tries to isolate the base component name by finding the last part of the string that is joined by ::
        """
        base_name_list = self.__component_name.split("::")
        return base_name_list[-1]

    def get_base_saveop(self):
        return self.__base_saveop

    def get_base_setop(self):
        return self.__base_setop
