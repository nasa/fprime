# ===============================================================================
# NAME: Event.py
#
# DESCRIPTION:  This is a Eve t meta-model sort of class.  It is
#               contained within a Component class as a list
#               of Event intances and is visited by the code
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
class Event:
    """
    This is a very simple component meta-model class.
    Note: associations to Arg instances as needed.
    """

    def __init__(
        self,
        ids,
        name,
        severity,
        format_string,
        throttle,
        args,
        comment=None,
        xml_filename=None,
        component_name=None,
        component_base_name=None,
    ):
        """
        Constructor
        @param id:  Event identifier (integer)
        @param name:  name of event (e.g. TEMP5OV)
        @param severity: Severity of event (FATAL, WARNING, INFO, DEBUG)
        @param format_string:  format string to display event
        @param args:  Arguments to the event
        """
        self.__ids = ids
        self.__name = name
        self.__severity = severity
        self.__format_string = format_string
        self.__throttle = throttle
        self.__arg_obj_list = args
        self.__comment = comment
        self.__xml_filename = xml_filename
        self.__component_name = component_name
        self.__component_base_name = component_base_name

    def get_ids(self):
        """
        Returns the event id
        """
        return self.__ids

    def get_name(self):
        """
        Returns the event name
        """
        return self.__name

    def get_severity(self):
        """
        Returns severity of event.
        """
        return self.__severity

    def get_format_string(self):
        """
        Returns format string for displaying event.
        """
        return self.__format_string

    def get_throttle(self):
        """
        returns throttle value for event
        """
        return self.__throttle

    def get_comment(self):
        """
        Returns comment for the event.
        """
        return self.__comment

    def get_args(self):
        """
        Return a list of Arg objects for each argument.
        """
        return self.__arg_obj_list

    def get_xml_filename(self):
        """
        Returns the original xml filename
        """
        return self.__xml_filename

    def get_component_name(self):
        return self.__component_name

    def get_component_base_name(self):
        return self.__component_base_name
