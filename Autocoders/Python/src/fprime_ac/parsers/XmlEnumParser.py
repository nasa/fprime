#!/bin/env python
#===============================================================================
# NAME: XmlEnumParser.py
#
# DESCRIPTION: This class parses the XML Enum types files. 
#
# USAGE: 
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : April 6, 2018
#
# Copyright 2018, California Institute of Technology.
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
class XmlEnumParser(object):
    """
    An XML parser class that uses lxml.etree to consume an XML
    enum type documents.  The class is instanced with an XML file name.
    """

    def __init__(self, xml_file=None):
        """
        Given a well formed XML file (xml_file), read it and turn it into
        a big string.
        """
        self.__name = ""
        self.__namespace = None

        self.__xml_filename = xml_file
        self.__items = []
        
        self.Config = ConfigManager.ConfigManager.getInstance()

        if os.path.isfile(xml_file) == False:
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            PRINT.info(stri)
            raise
        fd = open(xml_file,'r')
        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file
        self.__items = []

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd,parser=xml_parser)
        
        #Validate against current schema. if more are imported later in the process, they will be reevaluated
        relax_file_handler = open(ROOTDIR + self.Config.get('schema' , 'enum') , 'r')
        relax_parsed = etree.parse(relax_file_handler)
        relax_file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)
        
        self.validate_xml(xml_file, element_tree, 'schematron', 'enum_value')
        
        self.check_enum_values(element_tree)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            msg = "XML file {} is not valid according to schema {}.".format(xml_file , ROOTDIR + self.Config.get('schema' , 'enum'))
            PRINT.info(msg)
            print(element_tree)
            raise Exception(msg)
        
        enum = element_tree.getroot()
        if enum.tag != "enum":
            PRINT.info("%s is not a enum definition file" % xml_file)            
            sys.exit(-1)
            
        print("Parsing Enum %s" %enum.attrib['name'])
        self.__name = enum.attrib['name']
            
        if 'namespace' in enum.attrib:
            self.__namespace = enum.attrib['namespace']
        else:
            self.__namespace = None
        
        print(self.__name + " " + self.__namespace)
        
        for enum_tag in enum:
            item = enum_tag.attrib
            if not 'comment' in item:
                item['comment'] = ""
            
            if not 'value' in item:
                item['value'] = ""
            
            self.__items.append((item['name'],item['value'],item['comment']))

    def validate_xml(self, dict_file, parsed_xml_tree, validator_type, validator_name):
        # Check that validator is valid
        if not validator_type in self.Config or not validator_name in self.Config[validator_type]:
            msg = "XML Validator type " + validator_type + " not found in ConfigManager instance"
            PRINT.info(msg)
            print(msg)
            raise Exception(msg)
                                
        # Create proper xml validator tool
        validator_file_handler = open(ROOTDIR + self.Config.get(validator_type, validator_name), 'r')
        validator_parsed = etree.parse(validator_file_handler)
        validator_file_handler.close()
        if validator_type == 'schema':
            validator_compiled = etree.RelaxNG(validator_parsed)
        elif validator_type == 'schematron':
            validator_compiled = isoschematron.Schematron(validator_parsed)
                                                
        # Validate XML file
        if not validator_compiled.validate(parsed_xml_tree):
            if validator_type == 'schema':
                msg = "XML file {} is not valid according to {} {}.".format(dict_file, validator_type, ROOTDIR + self.Config.get(validator_type, validator_name))
                PRINT.info(msg)
                print(parsed_xml_tree)
                raise Exception(msg)
            elif validator_type == 'schematron':
                msg = "WARNING: XML file {} is not valid according to {} {}.".format(dict_file, validator_type, ROOTDIR + self.Config.get(validator_type, validator_name))
                PRINT.info(msg)
                print(parsed_xml_tree)

    def check_enum_values(self, element_tree):
        """
        Raises exception in case that enum items are inconsistent
        in whether they include attribute 'value'
        """
        if not self.is_attribute_consistent(element_tree, 'value'):
            msg = "If one enum item has a value attribute, all items should have a value attribute"
            PRINT.info(msg)
            print(element_tree)
            raise Exception(msg)
        
    def is_attribute_consistent(self, element_tree, val_name):
        """
        Returns true if either all or none of the enum items
        contain a given value
        """
        has_value = 0
        total = 0
        for enum_item in element_tree.iter():
            if enum_item.tag == 'item':
                total += 1
                if val_name in enum_item.keys():
                    has_value += 1
        
        is_consistent = True
        if not (has_value == 0 or has_value == total):
            is_consistent = False

        return is_consistent
    
    def get_max_value(self):
        # Assumes that items have already been checked for consistency,
        # self.__items stores a list of tuples with index 1 being the value
        if not self.__items[0][1] == "":
            max_value = self.__items[0][1]

            for item in self.__items:
                max_value = max(max_value, item[1])

        else:
            max_value = str(len(self.__items) - 1)

        return max_value

    def get_name(self):
        return self.__name
    
    def get_namespace(self):
        return self.__namespace
    
    def get_items(self):
        return self.__items
       

if __name__ == '__main__':
    xmlfile = sys.argv[1]
    xml = XmlParser.XmlParser(xmlfile)
    print("Type of XML is: %s" % xml())
    print("Enum XML parse test (%s)" % xmlfile)
    xml_parser = XmlEnumParser(xmlfile)
    print("Enum name: %s, namespace: %s" % (xml_parser.get_name(),xml_parser.get_namespace()))
    print("Items")
    for item in xml_parser.get_items():
        print("%s=%s // %s" % item)





