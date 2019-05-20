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
from optparse import OptionParser
from lxml import etree
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
        
        if os.path.isfile(xml_file) == False:
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            PRINT.info(stri)
            raise

        fd = open(xml_file,'r')

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd,parser=xml_parser)
        
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
        
        print self.__name, self.__namespace
        
        for enum_tag in enum:
            item = enum_tag.attrib
            if not 'comment' in item:
                item['comment'] = ""
            self.__items.append((item['name'],item['value'],item['comment']))

    def get_name(self):
        return self.__name
    
    def get_namespace(self):
        return self.__namespace
    
    def get_items(self):
        return self.__items
       

if __name__ == '__main__':
    xmlfile = sys.argv[1]
    xml = XmlParser.XmlParser(xmlfile)
    print "Type of XML is: %s" % xml()
    print "Enum XML parse test (%s)" % xmlfile
    xml_parser = XmlEnumParser(xmlfile)
    print "Enum name: %s, namespace: %s" % (xml_parser.get_name(),xml_parser.get_namespace())
    print "Items"
    for item in xml_parser.get_items():
        print "%s=%s // %s" % item





