#!/bin/env python
#===============================================================================
# NAME: XmlSerializeParser.py
#
# DESCRIPTION:  This class parses the XML serializable types files.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : June 4, 2013
#
# Copyright 2013, California Institute of Technology.
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
import hashlib
from fprime_ac.utils import ConfigManager
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

format_dictionary = {
            "U8":"%u",
            "I8":"%d",
            "U16":"%u",
            "I16":"%d",
            "U32":"%u",
            "I32":"%d",
            "U64":"%lu",
            "I64":"%ld",
            "F32":"%g",
            "F64":"%g",
            "bool":"%s",
            "string":"%s",
            "ENUM":"%d"
                     }
#
class XmlSerializeParser(object):
    """
    An XML parser class that uses lxml.etree to consume an XML
    serializable type documents.  The class is instanced with
    an XML file name.
    """
    def __init__(self, xml_file=None):
        """
        Given a well formed XML file (xml_file), read it and turn it into
        a big string.
        """
        self.__root = None
        self.__name = ""
        self.__namespace = None
        # List of C++ include files for serializable *.hpp file
        self.__include_header_files = []
        # List of XML serializable description dependencies
        self.__includes = []
        # List of XML enum type files
        self.__include_enum_files = []
        # Comment block of text for serializable
        self.__comment = ""
        # List of (name, type, comment) tuples
        self.__members = []
        # Type ID for serialized type
        self.__type_id = None
        #
        if os.path.isfile(xml_file) == False:
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            PRINT.info(stri)
            raise
        fd = open(xml_file,'r')
#        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file

        self.__config       = ConfigManager.ConfigManager.getInstance()

        #

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd,parser=xml_parser)

        #Validate new imports using their root tag as a key to find what schema to use
        for possible in [os.environ.get('BUILD_ROOT'), os.environ.get('FPRIME_CORE_DIR',"")]:
            rng_file = os.path.join(possible, self.__config.get('schema' , element_tree.getroot().tag.lower()).lstrip("/"))
            if os.path.isfile(rng_file) == True:
                break
        else:
            stri = "ERROR: Could not find specified RNG file %s." % rng_file
            PRINT.info(stri)
            raise IOError(stri)
        file_handler = open(rng_file, 'r')
        relax_parsed = etree.parse(file_handler)
        file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            msg = "XML file {} is not valid according to schema {}.".format(xml_file , os.environ["BUILD_ROOT"] +self.__config.get('schema' , element_tree.getroot().tag.lower()))
            PRINT.info(msg)
            print(element_tree)
            raise Exception(msg)

        serializable = element_tree.getroot()
        if serializable.tag != "serializable":
            PRINT.info("%s is not a serializable definition file"%xml_file)
            sys.exit(-1)

        print(("Parsing Serializable %s" %serializable.attrib['name']))

        self.__name = serializable.attrib['name']

        if 'namespace' in serializable.attrib:
            self.__namespace = serializable.attrib['namespace']
        else:
            self.__namespace = None

        if 'typeid' in serializable.attrib:
            self.__type_id = serializable.attrib['typeid']
        else:
            self.__type_id = None

        for serializable_tag in serializable:
            if serializable_tag.tag == 'comment':
                self.__comment = serializable_tag.text.strip()
            elif serializable_tag.tag == 'include_header':
                self.__include_header_files.append(serializable_tag.text)
            elif serializable_tag.tag == 'import_serializable_type':
                self.__includes.append(serializable_tag.text)
            elif serializable_tag.tag == 'import_enum_type':
                self.__include_enum_files.append(serializable_tag.text)
            elif serializable_tag.tag == 'members':
                for member in serializable_tag:
                    if member.tag != 'member':
                        PRINT.info("%s: Invalid tag %s in serializable member definition"%(xml_file,member.tag))
                        sys.exit(-1)
                    n = member.attrib['name']
                    t = member.attrib['type']
                    if 'size' in list(member.attrib.keys()):
                        if t == "ENUM":
                            PRINT.info("%s: Member %s: arrays of enums not supported yet!"%(xml_file,n))
                            sys.exit(-1)
                        s = member.attrib['size']
                        if not s.isdigit():
                            PRINT.info("%s: Member %s: size must be a number"%(xml_file,n))
                            sys.exit(-1)
                    else:
                        s = None
                    if 'format' in list(member.attrib.keys()):
                        f = member.attrib['format']
                    else:
                        if t in list(format_dictionary.keys()):
                            f = format_dictionary[t]
                        else: # Must be included type, which will use toString method
                            f = "%s"
                    if t == 'string':
                        if s == None:
                            PRINT.info("%s: member %s string must specify size tag"%(xml_file,member.tag))
                            sys.exit(-1)

                    if 'comment' in list(member.attrib.keys()):
                        c = member.attrib['comment']
                    else:
                        c = None

                    for member_tag in member:
                        if member_tag.tag == 'enum' and t == 'ENUM':
                            en = member_tag.attrib['name']
                            enum_members = []
                            for mem in member_tag:
                                mn = mem.attrib['name']
                                if "value" in list(mem.attrib.keys()):
                                    v = mem.attrib['value']
                                else:
                                    v = None
                                if "comment" in list(mem.attrib.keys()):
                                    mc = mem.attrib['comment'].strip()
                                else:
                                    mc = None
                                enum_members.append((mn,v,mc))
                            t = ((t,en),enum_members)
                        else:
                            PRINT.info("%s: Invalid member tag %s in serializable member %s"%(xml_file,member_tag.tag,n))
                            sys.exit(-1)


                    self.__members.append((n, t, s, f, c))


        #
        # Generate a type id here using SHA256 algorithm and XML stringified file.
        #

        if not 'typeid' in serializable.attrib:
            s = etree.tostring(element_tree.getroot())
            h = hashlib.sha256(s)
            n = h.hexdigest()
            self.__type_id = "0x" + n.upper()[-8:]


    def is_serializable(self):
        """
        Return true if serializable tag found, else return false
        """
        return self.__in_serialize

    def get_typeid(self):
        """
        Return a generated type ID from contents of XML file.
        """
        return self.__type_id

    def get_xml_filename(self):
        """
        Return the original XML filename parsed.
        """
        return self.__xml_filename

    def get_name(self):
        return self.__name

    def get_namespace(self):
        return self.__namespace

    def get_include_header_files(self):
        """
        Return a list of all imported Port type XML files.
        """
        return self.__include_header_files


    def get_includes(self):
        """
        Returns a list of all imported XML serializable files.
        """
        return self.__includes
    
    def get_include_enums(self):
        """
        Returns a list of all imported XML enum files.
        """
        return self.__include_enum_files


    def get_comment(self):
        """
        Return text block string of comment for serializable class.
        """
        return self.__comment


    def get_members(self):
        """
        Returns a list of member (name, type, optional size, optional format, optional comment) needed.
        """
        return self.__members


if __name__ == '__main__':
    xmlfile = sys.argv[1]
    print("Ports XML parse test (%s)" % xmlfile)
    xml_parser = XmlSerializeParser(xmlfile)
    print("Serializable: %s" % xml_parser.is_serializable())
    print("File: %s" % xml_parser.get_xml_filename())
    print("Namespace: %s" % xml_parser.get_namespace())
    print("Name: %s" % xml_parser.get_name())
    print("Comment: %s" % xml_parser.get_comment())
    for i in xml_parser.get_include_header_files():
        print("C Include: %s" % i)
    for i in xml_parser.get_includes():
        print("XML Include: %s" % i)
    print("Members:")
    for (n,t,c) in xml_parser.get_members():
        print("Name: %s, Type: %s, Comment: %s" % (n,t,c))




