# ===============================================================================
# NAME: Topology.py
#
# DESCRIPTION:  This is a Topology meta-model sort of class.  It is
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
class Topology:
    """
    This is a very simple topology meta-model class.
    """

    def __init__(
        self,
        namespace,
        comment=None,
        comp_list=None,
        name=None,
        base_id_list=None,
        prepend_instance_name=None,
    ):
        """
        Constructor
        """
        self.__namespace = namespace
        self.__name = name
        self.__comment = comment
        self.__comp_list = comp_list
        self.__base_id_list = base_id_list  # [name of component , base id , window id , instance object] The Instance object is from XmlTopologyParser.py. To get type of the instance (the generic component name) use inst.inst.get_type()
        self.__base_id_dict = None
        self.__instance_xml_list = []
        self.__instance_header_dict = (
            {}
        )  # The key is a val from the __instance_xml_list, the associated value is either a path to where the header file is located or a None. If the value is a None, "predict" where the file loc will be
        self.__prepend_instance_name = prepend_instance_name

        self.is_ptr = False
        self.connect_only = False

    def get_namespace(self):
        return self.__namespace

    def get_name(self):
        return self.__name

    def get_comment(self):
        return self.__comment

    def get_comp_list(self):
        return self.__comp_list

    def get_base_id_list(self):
        return self.__base_id_list

    def get_base_id_dict(self):
        """
        Converts the __base_id_list into a dictionary, where the base component name corresponds to a list of tuples
        """
        if self.__base_id_dict is None:
            self.__base_id_dict = {}
            for base_id_tuple in self.__base_id_list:
                inst = base_id_tuple[3]
                if inst.get_type() not in list(self.__base_id_dict.keys()):
                    self.__base_id_dict[inst.get_type()] = []
                self.__base_id_dict[inst.get_type()].append(base_id_tuple)
        return self.__base_id_dict

    def set_comps(self, comps):
        self.__comp_list = comps

    def set_comment(self, temp):
        self.__comment = temp

    def set_instance_xml_list(self, xml_list):
        self.__instance_xml_list = xml_list

    def get_instance_xml_list(self):
        return self.__instance_xml_list

    def set_instance_header_dict(self, instance_header_dict):
        """
        Sets the header dict (key = xml name, val = comp header location)
        If the val is none, a comp header location is computed, making assumptions
        """

        """self.__instance_header_dict = {}
        for xml_name in instance_header_dict:
            if instance_header_dict[xml_name] is not None:
                self.__instance_header_dict[xml_name] = instance_header_dict[xml_name]
            else:
                temp_name = xml_name
                temp_name = temp_name.strip("i.xml")
                temp_name = temp_name + "c.hpp"
                self.__instance_header_dict[xml_name] = temp_name"""

        self.__instance_header_dict = instance_header_dict

    def get_instance_header_dict(self):
        return self.__instance_header_dict

    def set_single_instance_prepend_instance_name(self, prepend_instance_name):
        """
        Only sets this variable if instance_dict is running.
        This is used to turn off prepending instance names onto chan/event/commands
        """
        self.__prepend_instance_name = prepend_instance_name

    def get_prepend_instance_name(self):
        """
        Returns true if instances name should be prepended to components even if the type only has one instance
        """
        return self.__prepend_instance_name
