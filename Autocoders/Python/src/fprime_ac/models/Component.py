# ===============================================================================
# NAME: Component.py
#
# DESCRIPTION:  This is a Component meta-model sort of class.  It is
#               passed to the visitors and is the main interface
#               for code generation queries.
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
from typing import List

from .Port import Port
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
class Component:
    """
    This is a very simple component meta-model class.
    Note: associations to Port instances as needed.
    """

    def __init__(
        self,
        namespace,
        name,
        kind,
        comment=None,
        modeler=False,
        port_obj_list=None,
        command_obj_list=None,
        channel_obj_list=None,
        parameter_obj_list=None,
        event_obj_list=None,
        internal_interface_obj_list=None,
        serializable_obj_list=None,
        xml_filename=None,
        included_enums_list=None,
        kind2=None,
        base_id=None,
        base_id_window=None,
    ):
        """
        Constructor
        """
        self.__namespace = namespace
        self.__name = name
        self.__kind = kind
        self.__comment = comment
        self.__modeler = modeler
        self.__port_obj_list = port_obj_list
        self.__command_obj_list = command_obj_list
        self.__channel_obj_list = channel_obj_list
        self.__parameter_obj_list = parameter_obj_list
        self.__event_obj_list = event_obj_list
        self.__internal_interface_obj_list = internal_interface_obj_list
        self.__serializable_obj_list = serializable_obj_list
        self.__xml_filename = xml_filename
        self.__has_guarded_ports = False
        self.__included_enums_list = included_enums_list
        # Note __kind is used as type for Topology and Kind for Component generation
        # Quick Fix for Topology generation adds kind2 to specify active or passive component instance
        self.__kind2 = kind2
        #
        self.__base_id = base_id
        self.__base_id_window = base_id_window

    def get_xml_filename(self):
        return self.__xml_filename

    def get_namespace(self):
        return self.__namespace

    def get_name(self):
        return self.__name

    def get_kind(self):
        return self.__kind

    def get_kind2(self):
        return self.__kind2

    def set_kind(self, kind):
        self.__kind = kind

    def get_comment(self):
        return self.__comment

    def get_modeler(self):
        return self.__modeler

    def get_ports(self) -> List[Port]:
        return self.__port_obj_list

    def set_ports(self, ports):
        self.__port_obj_list = ports

    def get_commands(self):
        return self.__command_obj_list

    def get_channels(self):
        return self.__channel_obj_list

    def get_parameters(self):
        return self.__parameter_obj_list

    def get_events(self):
        return self.__event_obj_list

    def set_events(self, event_obj_list):
        self.__event_obj_list = event_obj_list

    def get_internal_interfaces(self):
        return self.__internal_interface_obj_list

    def get_serializables(self):
        return self.__serializable_obj_list

    def set_commands(self, commands):
        self.__command_obj_list = commands

    def set_channels(self, channels):
        self.__channel_obj_list = channels

    def set_parameters(self, parameters):
        self.__parameter_obj_list = parameters

    def set_serializables(self, serializables):
        self.__serializable_obj_list = serializables

    def set_xml_serializable_files(self, x):
        self.__xml_serializable_files = x

    def set_xml_port_files(self, x):
        self.__xml_port_files = x

    def set_c_header_files(self, x):
        self.__c_header_files = x

    def get_xml_port_files(self):
        return self.__xml_port_files

    def get_xml_serializable_files(self):
        return self.__xml_serializable_files

    def get_c_header_files(self):
        return self.__c_header_files

    def set_has_guarded_ports(self):
        self.__has_guarded_ports = True

    def get_has_guarded_ports(self):
        return self.__has_guarded_ports

    def get_base_id(self):
        return self.__base_id

    def get_base_id_window(self):
        return self.__base_id_window

    def get_included_enums(self):
        return self.__included_enums_list
