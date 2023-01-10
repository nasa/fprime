#!/usr/bin/env python3
# ===============================================================================
# NAME: XmlPortsParser.py
#
# DESCRIPTION:  This class parses the XML port types files.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb. 4, 2013
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging
import os
import sys

from fprime_ac.utils import ConfigManager
from fprime_ac.utils.exceptions import FprimeRngXmlValidationException
from lxml import etree

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
ROOTDIR = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
#
class XmlPortsParser:
    """
    An XML parser class that uses lxml.etree to consume an XML
    port type documents.
    The class is instanced with an XML file name.
    """

    def __init__(self, xml_file=None):
        """
        Given a well formed XML file (xml_file), read it and turn it into
        a big string.
        """
        self.__root = None
        self.__include_serializable_files = []
        self.__include_enum_files = []
        self.__include_array_files = []
        self.__include_header_files = []
        #
        self.__config = ConfigManager.ConfigManager.getInstance()
        #
        self.__port = None
        self.__args = []
        self.__enum_list_items = []
        self.__modifier = None
        #
        if not os.path.isfile(xml_file):
            str = "ERROR: Could not find specified XML file %s." % xml_file
            raise OSError(str)
        fd = open(xml_file)
        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file
        #

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd, parser=xml_parser)
        fd.close()  # Close the file, which is only used for the parsing above

        # Validate against schema
        relax_file_handler = open(ROOTDIR + self.__config.get("schema", "interface"))
        relax_parsed = etree.parse(relax_file_handler)
        relax_file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            raise FprimeRngXmlValidationException(relax_compiled.error_log)

        interface = element_tree.getroot()
        if interface.tag != "interface":
            PRINT.info("%s is not an interface file" % xml_file)
            sys.exit(-1)

        print("Parsing Interface %s" % interface.attrib["name"])

        if "namespace" in interface.attrib:
            namespace_name = interface.attrib["namespace"]
        else:
            namespace_name = None

        self.__port = Interface(namespace_name, interface.attrib["name"])

        for interface_tag in interface:
            if interface_tag.tag == "comment":
                self.__port.set_comment(interface_tag.text.strip())
            elif interface_tag.tag == "include_header":
                self.__include_header_files.append(interface_tag.text)
            elif interface_tag.tag == "import_serializable_type":
                self.__include_serializable_files.append(interface_tag.text)
            elif interface_tag.tag == "import_enum_type":
                self.__include_enum_files.append(interface_tag.text)
            elif interface_tag.tag == "import_array_type":
                self.__include_array_files.append(interface_tag.text)
            elif interface_tag.tag == "args":
                for arg in interface_tag:
                    if arg.tag != "arg":
                        PRINT.info(
                            "%s: Invalid tag %s in interface args definition"
                            % (xml_file, arg.tag)
                        )
                        sys.exit(-1)
                    n = arg.attrib["name"]
                    t = arg.attrib["type"]
                    if "pass_by" in list(arg.attrib.keys()):
                        p = arg.attrib["pass_by"]
                    else:
                        p = None
                    if t in ("string", "buffer"):
                        if not "size" in list(arg.attrib.keys()):
                            PRINT.info(
                                "%s: arg %s string must specify size tag"
                                % (xml_file, arg.tag)
                            )
                            sys.exit(-1)
                        else:
                            s = arg.attrib["size"]
                    else:
                        s = None
                    arg_obj = Arg(n, t, p, s, None)
                    for arg_tag in arg:
                        # only valid tag in command args is comment
                        if arg_tag.tag == "comment":
                            arg_obj.set_comment(arg_tag.text)
                        elif arg_tag.tag == "enum" and t == "ENUM":
                            en = arg_tag.attrib["name"]
                            enum_members = []
                            for mem in arg_tag:
                                mn = mem.attrib["name"]
                                if "value" in list(mem.attrib.keys()):
                                    v = mem.attrib["value"]
                                else:
                                    v = None
                                if "comment" in list(mem.attrib.keys()):
                                    mc = mem.attrib["comment"].strip()
                                else:
                                    mc = None
                                enum_members.append((mn, v, mc))
                            arg_obj.set_type(((t, en), enum_members))
                        else:
                            PRINT.info(
                                "%s: Invalid argument tag %s in port %s argument %s"
                                % (xml_file, arg_tag.tag, interface_tag.tag, n)
                            )
                            sys.exit(-1)

                    self.__args.append(arg_obj)

            elif interface_tag.tag == "return":
                t = interface_tag.attrib["type"]
                if "pass_by" in list(interface_tag.attrib.keys()):
                    m = interface_tag.attrib["pass_by"]
                else:
                    m = "value"
                for enum_tag in interface_tag:
                    # The only tags would be enumeration declarations
                    if enum_tag.tag == "enum" and t == "ENUM":
                        en = enum_tag.attrib["name"]
                        enum_members = []
                        for mem in enum_tag:
                            mn = mem.attrib["name"]
                            if "value" in list(mem.attrib.keys()):
                                v = mem.attrib["value"]
                            else:
                                v = None
                            if "comment" in list(mem.attrib.keys()):
                                mc = mem.attrib["comment"].strip()
                            else:
                                mc = None
                            enum_members.append((mn, v, mc))
                        t = ((t, en), enum_members)
                    else:
                        PRINT.info(
                            "%s: Invalid port return value tag %s"
                            % (xml_file, enum_tag.tag)
                        )
                        sys.exit(-1)

                self.__port.set_return(t, m)

        # Check XML name for compliance here...

    #         name = self.get_interface().get_name()
    #         if (os.path.basename(xml_file)[:len(name)] != name):
    #             PRINT.info("ERROR: Port XML files must begin with name of port...")
    #             sys.exit(-1)

    def __del__(self):
        del self.__port

    def get_xml_filename(self):
        """
        Return the original XML filename parsed.
        """
        return self.__xml_filename

    def get_include_header_files(self):
        """
        Return a list of all imported .hpp or .h files.
        """
        return self.__include_header_files

    def get_includes_serial_files(self):
        """
        Return a list of all imported Serializable XML files.
        """
        return self.__include_serializable_files

    def get_include_enum_files(self):
        """
        Return a list of all imported enum XML files.
        """
        return self.__include_enum_files

    def get_include_array_files(self):
        """
        Return a list of all imported array XML files.
        """
        return self.__include_array_files

    def get_interface(self):
        """
        Returns an interface object.
        """
        return self.__port

    def get_args(self):
        """
        Returns a list of arg objects with all text and attrib needed.
        """
        return self.__args


class Interface:
    """
    Data container for an interface.
    Note in the context of this architecture
    a port has just one interface and this is
    it.
    """

    def __init__(self, namespace, name, comment=None):
        """
        Constructor
        """
        self.__namespace = namespace
        self.__name = name
        self.__comment = comment
        self.__return_type = None
        self.__return_modifier = None

    def get_namespace(self):
        return self.__namespace

    def get_name(self):
        return self.__name

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def set_return(self, t, m):
        """
        Set a return type and modifier.
        """
        self.__return_type = t
        self.__return_modifier = m

    def get_return_type(self):
        return self.__return_type

    def get_return_modifier(self):
        return self.__return_modifier


class Arg:
    """
    Data container for all the port name, type, etc. associated with component.
    """

    def __init__(self, name, atype, modifier, size=None, comment=None):
        """
        Constructor
        @param name:  Name of arg (each instance must be unique).
        @param type:  Type of arg (must have supporting include xml)
        @param modifier:  Whether argument is passed by value, reference, or pointer
        @param size:  size of array for string and buffer
        @param comment:  A single or multiline comment
        """
        self.__name = name
        self.__type = atype
        self.__modifier = modifier
        self.__size = size
        self.__comment = comment

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_modifier(self):
        return self.__modifier

    def get_size(self):
        return self.__size

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def set_type(self, type):
        self.__type = type
