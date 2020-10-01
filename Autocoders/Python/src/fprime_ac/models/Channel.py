# ===============================================================================
# NAME: Port.py
#
# DESCRIPTION:  This is a Port meta-model sort of class.  It is
#               contained within a Component class as a list
#               of Port intances and is visited by the code
#               generators.
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
class Channel:
    """
    This is a very simple component meta-model class.
    Note: associations to Arg instances as needed.
    """

    def __init__(
        self,
        ids,
        name,
        ctype,
        size,
        abbrev=None,
        format_string=None,
        update=None,
        limits=(None, None, None, None),
        comment=None,
        xml_filename=None,
        component_name=None,
        component_base_name=None,
    ):
        """
        Constructor
        @param id:  Numeric ID of channel
        @param name: Name of channel
        @param ctype:   Type of channel
        @param size: Size, if string type
        @param abbrev: Abbreviation (to support AMPCS)
        @param comment:  A single or multi-line comment describing the channel
        @param xml_filename: Component XML where channel is defined
        @param component_name: Typename of component
        """
        self.__ids = ids
        self.__name = name
        self.__ctype = ctype
        self.__size = size
        self.__abbrev = abbrev
        self.__comment = comment
        self.__format_string = format_string
        self.__update = update
        self.__limits = limits
        self.__xml_filename = xml_filename
        self.__component_name = component_name
        self.__component_base_name = component_base_name
        #

    def get_ids(self):
        """
        Returns the channel id
        """
        return self.__ids

    def get_name(self):
        """
        Returns the channel name
        """
        return self.__name

    def get_type(self):
        """
        Return the channel type.
        """
        return self.__ctype

    def get_size(self):
        """
        Return the string size, if string.
        """
        return self.__size

    def get_abbrev(self):
        """
        Return the channel abbreviation
        """
        return self.__abbrev

    def set_abbrev(self, new_abbrev):
        """
        Sets the abbrev to be new_abbrev
        """
        self.__abbrev = new_abbrev

    def get_format_string(self):
        """
        Return the format specifier
        """
        return self.__format_string

    def get_update(self):
        """
        Return the update ("always" or "on_change")
        """
        return self.__update

    def get_limits(self):
        """
        Return the channel limits
        """
        return self.__limits

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
        return self.__component_base_name
