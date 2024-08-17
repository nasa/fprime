# ===============================================================================
# NAME:  abstract_parser.py
#
# DESCRIPTION: This module defines shared abstract interfaces for parsing,
#              validation, and common getter methods for component, port,
#              and assembly XML.
#
# USAGE: This module is typically used to establish an interface to XML parsing
#        and validation routines, which can be extended by concrete implementations.
#
# AUTHOR: reder
# MODIFIED AUTHOR: Hay.Banz
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: Jan 30, 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging

# Initialize loggers for output and debugging
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

class AbstractParser:
    """
    Defines common interfaces for parsing and validating component, port, 
    and assembly XML. This class is intended to be subclassed by future 
    meta-model factories that will provide specific implementations.
    """

    def __init__(self):
        """
        Constructor.
        Initializes the abstract parser. Subclasses should extend this 
        constructor to initialize their specific context.
        """
        pass  # Placeholder for potential future use

    def get(self, name):
        """
        Retrieve data content from an XML tree.
        
        @param name: Name of the XML tag to return.
        @return: A list containing the data value contents of the specified tag.
        
        This method must be implemented by subclasses.
        """
        raise NotImplementedError(
            "AbstractParser.get() - Implementation Error: you must supply your own concrete implementation of get(...)."
        )

    def getAttr(self, name):
        """
        Retrieve attribute content from an XML tree.
        
        @param name: Name of the XML tag whose attributes are to be returned.
        @return: A list of dictionaries, each containing the attributes of the specified tag.
        
        This method must be implemented by subclasses.
        """
        raise NotImplementedError(
            "AbstractParser.getAttr() - Implementation Error: you must supply your own concrete implementation of getAttr(...)."
        )
