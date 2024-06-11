# ===============================================================================
# NAME: Arg.py
#
# DESCRIPTION:  This is an Arg (argument) meta-model sort of class.  It is
#               instanced as an association to Port instances and visited
#               by the code generators.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
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
# Python extension modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
class Arg:
    """
    This is a very simple arg meta-model class.
    Note: This class is a container of basic argument information.
    @todo: Serializables might need to be incorporated into this.
    """

    def __init__(self, name, atype, modifier, size=None, comment=None):
        """
        Constructor
        @param name:  Name of arg (each instance must be unique).
        @param type:  Type of arg (must have supporting include xml)
        @param modifier: Modification of the argument (i.e. pointer or reference)
        @param size: size of array of type (string or buffer)
        @param comment:  A single or multiline comment
        """
        self.__name = name
        self.__type = atype
        self.__modifier = modifier
        self.__size = size
        self.__comment = comment

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_modifier(self):
        return self.__modifier

    def get_size(self):
        return self.__size

    def get_comment(self):
        return self.__comment

    def __eq__(self, other):
        """Equality checker"""
        if type(other) != type(self):
            return False
        return (
            self.__name == other.__name
            and self.__type == other.__type
            and self.__modifier == other.__modifier
            and self.__size == other.__size
            and self.__comment == other.__comment
        )
