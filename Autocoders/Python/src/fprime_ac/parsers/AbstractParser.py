# ===============================================================================
# NAME:  abstract_face.py
#
# DESCRIPTION: The abstract parser defines the some shared interfaces
#                for parsing, validation and in common
#              getter methods for component, port and assembly
#              XML.
#
# USAGE: Normally this is used to establish interface
#        to XML parsing and validation routines.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Jan 30, 2013
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
# Module class or classes go here.


class AbstractParser:
    """
    Defines the common interfaces for component, port and assembly XML
    parsing, validation.   This is intended to be used by future
    meta-model factories to configure it.
    """

    def __init__(self):
        """
        Constructor.
        """

    def get(self, name):
        """
        Get data content from XML tree.
        @param name: Name of tag to return.
        @return: Tag data value contents. A list.
        """
        raise Exception(
            "AbstractFace.__call__() - Implementation Error: you must supply your own concrete implementation of get(...)."
        )

    def getAttr(self, name):
        """
        Get attributes content from XML tree.
        @param name: Name of tag to return.
        @return: Tag attribute dict contents.  A list of dict.
        """
        raise Exception(
            "AbstractFace.__call__() - Implementation Error: you must supply your own concrete implementation of getAttr(...)."
        )
