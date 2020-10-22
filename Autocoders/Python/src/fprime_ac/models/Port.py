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
class Port:
    """
    This is a very simple component meta-model class.
    Note: associations to Arg instances as needed.
    """

    def __init__(
        self,
        pname,
        ptype,
        direction,
        sync=None,
        priority=None,
        full=None,
        comment=None,
        xml_filename=None,
        max_number=None,
        role=None,
    ):
        """
        Constructor
        @param pname:  Name of port instance (each instance must be unique)
        @param type:   Type of port (must have supporting include xml)
        @param direction: Direction of data flow (must be input or output)
        @param sync:  Kind of port (must be one of: async, sync, or guarded)
        @param comment:  A single or multi-line comment describing the port
        """
        self.__pname = pname
        self.__ptype = ptype
        self.__direction = direction
        self.__sync = sync
        self.__priority = priority
        self.__full = full
        self.__comment = comment
        self.__role = role
        self.__namespace = ""
        self.__arg_obj_list = []
        self.__incl_list = []
        self.__if_comment = ""
        self.__xml_filename = xml_filename
        #
        self.__target_comp = None
        self.__target_port = None
        self.__target_type = None
        self.__target_direction = None
        self.__target_num = None
        self.__source_num = None
        #
        self.__max_number = max_number
        #
        self.__return_type = None
        self.__return_modifier = None

    def set(
        self,
        namespace,
        arg_obj_list,
        incl_list,
        incl_serial_list,
        incl_enum_list,
        if_comment,
    ):
        """
        Set type specific stuff for port type here.
        Not instance specific stuff.

        @param namespace:     Namespace for port type.
        @param arg_obj_list:  List of Arg objects for each argument (list of Arg objects).
        @param incl_list:     List of Include objects for each data type.
        @param if_comment:    Interface comment.
        """
        self.__namespace = namespace
        self.__arg_obj_list = arg_obj_list
        self.__incl_list = incl_list
        self.__incl_serial_list = incl_serial_list
        self.__incl_enum_list = incl_enum_list
        self.__if_comment = if_comment

    def set_return(self, t, m):
        """
        Set the optional return type if one is specified here.
        """
        self.__return_type = t
        self.__return_modifier = m

    def get_return(self):
        """
        Return a tuple of (type, modifier).  If (None,None) return None.
        """
        if (self.__return_modifier is None) and (self.__return_type is None):
            return None
        else:
            return (self.__return_type, self.__return_modifier)

    def set_role(self, role):
        """
        Sets the role
        """
        self.__role = role

    def get_role(self):
        """
        Returns role
        """
        return self.__role

    def get_xml_filename(self):
        """
        Returns the original xml filename
        """
        return self.__xml_filename

    def get_namespace(self):
        """
        Returns the port's namespace
        """
        return self.__namespace

    def get_name(self):
        """
        Returns the port's instance name.
        """
        return self.__pname

    def get_direction(self):
        """
        Return the direction for port.
        """
        return self.__direction

    def get_type(self):
        """
        Return the port's type.
        """
        return self.__ptype

    def get_priority(self):
        """
        Return the port's priority.
        """
        return self.__priority

    def get_full(self):
        """
        Return the port's queue full behavior.
        """
        return self.__full

    def get_sync(self):
        """
        Returns type of port.
        """
        return self.__sync

    def get_comment(self):
        """
        Returns comment for the port.
        """
        return self.__comment

    def get_ifcomment(self):
        """
        Return interface comment for the port.
        """
        return self.__if_comment

    def get_args(self):
        """
        Return a list of Arg objects for each argument.
        """
        return self.__arg_obj_list

    def get_includes(self):
        """
        Return a list of include files for each port type.
        These support data types within each Arg object.
        """
        return self.__incl_list

    def get_serial_includes(self):
        """"""
        return self.__incl_serial_list

    def get_enum_includes(self):
        """"""
        return self.__incl_enum_list

    def get_target_comp(self):
        return self.__target_comp

    def get_target_port(self):
        return self.__target_port

    def get_target_type(self):
        return self.__target_type

    def get_target_direction(self):
        return self.__target_direction

    def set_target_comp(self, comp):
        self.__target_comp = comp

    def set_target_port(self, port):
        self.__target_port = port

    def set_target_type(self, type):
        self.__target_type = type

    def set_target_direction(self, direction):
        self.__target_direction = direction

    def set_direction(self, direction):
        self.__direction = direction

    def get_max_number(self):
        return self.__max_number

    def get_target_num(self):
        return self.__target_num

    def set_target_num(self, n):
        self.__target_num = n

    def get_source_num(self):
        return self.__source_num

    def set_source_num(self, n):
        self.__source_num = n
