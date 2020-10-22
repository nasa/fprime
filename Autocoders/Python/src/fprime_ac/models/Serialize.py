# ===============================================================================
# NAME: Serialize.py.py
#
# DESCRIPTION:  This is an Serialize meta-model class.  It is
#               instanced for the generation of Serializable classes.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : June 4, 2013
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
class Serialize:
    """
    This is a simple meta-model representation for generation of Serializable class types.
    """

    def __init__(
        self,
        xml_filename,
        name,
        namespace,
        comment,
        xml_includes_list,
        c_includes_list,
        members_list,
        type_id,
    ):
        """
        Constructor
        @param xml_filename: name of original xml source file
        @param name: name of Serializable class
        @param namespace: optional namespace name
        @param comment: option comment test string block
        @param xml_includes_list: optional list of XML serializable includes
        @param c_includes_list: optional list of C serializable includes
        @param members_list: list of members (name, type, optional comment)
        """
        self.__xml_filename = xml_filename
        self.__name = name
        self.__namespace = namespace
        self.__comment = comment
        self.__xml_includes_list = xml_includes_list
        self.__c_includes_list = c_includes_list
        self.__members_list = members_list
        self.__type_id = type_id

    def get_xml_filename(self):
        return self.__xml_filename

    def get_name(self):
        return self.__name

    def get_namespace(self):
        return self.__namespace

    def get_comment(self):
        return self.__comment

    def get_xml_includes(self):
        return self.__xml_includes_list

    def get_c_includes(self):
        return self.__c_includes_list

    def get_members(self):
        return self.__members_list

    def get_typeid(self):
        return self.__type_id
