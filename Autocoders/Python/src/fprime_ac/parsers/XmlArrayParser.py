#!/usr/bin/env python3
#===============================================================================
# NAME: XmlArrayParser.py
#
# DESCRIPTION: This class parses the XML Array types files. 
#
# USAGE: 
#
# AUTHOR: jishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : May 28, 2020
#
# Copyright 2020, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import logging
import os
import sys
import time
from fprime_ac.utils import ConfigManager
from optparse import OptionParser
from lxml import etree
from lxml import isoschematron
from fprime_ac.parsers import XmlParser
from fprime_ac.utils.exceptions import FprimeXmlException, FprimeRngXmlValidationException
#
# Python extention modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
ROOTDIR = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
#
class XmlArrayParser(object):
    """
    An XML parser class that uses lxml.etree to consume an XML
    array type documents.  The class is instanced with an XML file name.
    """

    def __init__(self, xml_file=None):
        """
        Given a well formed XML file (xml_file), read it and turn it into
        a big string.
        """
        self.__name = ""
        self.__namespace = None

        self.__xml_filename = xml_file
        self.__type = "U32"
        self.__size = 4
        
        self.Config = ConfigManager.ConfigManager.getInstance()

        if os.path.isfile(xml_file) == False:
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            raise IOError(stri)
        fd = open(xml_file,'r')
        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd,parser=xml_parser)
        
        #Validate against current schema. if more are imported later in the process, they will be reevaluated
        relax_file_handler = open(ROOTDIR + self.Config.get('schema', 'array') , 'r')
        relax_parsed = etree.parse(relax_file_handler)
        relax_file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            raise FprimeRngXmlValidationException(relax_compiled.error_log)
        
        array = element_tree.getroot()
        if array.tag != "array":
            PRINT.info("%s is not an array definition file" % xml_file)            
            sys.exit(-1)
            
        print("Parsing Array %s" %array.attrib['name'])
        self.__name = array.attrib['name']
            
        if 'namespace' in array.attrib:
            self.__namespace = array.attrib['namespace']
        else:
            self.__namespace = None
        
        print(self.__name + " " + self.__namespace)

        self.__type = array.attrib["type"]
        self.__size = array.attrib["size"]

    def validate_xml(self, dict_file, parsed_xml_tree, validator_type, validator_name):
        # Check that validator is valid
        if not validator_type in self.Config or not validator_name in self.Config[validator_type]:
            msg = "XML Validator type " + validator_type + " not found in ConfigManager instance"
            raise FprimeXmlException(msg)
                                
        # Create proper xml validator tool
        validator_file_handler = open(ROOTDIR + self.Config.get(validator_type, validator_name), 'r')
        validator_parsed = etree.parse(validator_file_handler)
        validator_file_handler.close()
        if validator_type == 'schema':
            validator_compiled = etree.RelaxNG(validator_parsed)
                                                
        # Validate XML file
        if not validator_compiled.validate(parsed_xml_tree):
            if validator_type == 'schema':
                msg = "XML file {} is not valid according to {} {}.".format(dict_file, validator_type, ROOTDIR + self.Config.get(validator_type, validator_name))
                raise FprimeXmlException(msg)

    def get_name(self):
        return self.__name
    
    def get_namespace(self):
        return self.__namespace

    def get_type(self):
        return self.__type
    
    def get_size(self):
        return self.__size


if __name__ == '__main__':
    xmlfile = sys.argv[1]
    xml = XmlParser.XmlParser(xmlfile)
    print("Type of XML is: %s" % xml())
    print("Array XML parse test (%s)" % xmlfile)
    xml_parser = XmlEnumParser(xmlfile)
    print("Array name: %s, namespace: %s" % (xml_parser.get_name(),xml_parser.get_namespace()))
    print("Items")
    for item in xml_parser.get_items():
        print("%s=%s // %s" % item)

