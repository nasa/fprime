#!/usr/bin/env python3
# ===============================================================================
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
# ===============================================================================
#
# Python standard modules
#
import hashlib
import logging
import os
import sys

from fprime_ac.utils import ConfigManager
from fprime_ac.utils.buildroot import (
    BuildRootCollisionException,
    BuildRootMissingException,
    locate_build_root,
)
from fprime_ac.utils.exceptions import FprimeXmlException
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

format_dictionary = {
    "U8": "%u",
    "I8": "%d",
    "U16": "%u",
    "I16": "%d",
    "U32": "%u",
    "I32": "%d",
    "U64": "%lu",
    "I64": "%ld",
    "F32": "%g",
    "F64": "%g",
    "bool": "%s",
    "string": "%s",
    "ENUM": "%d",
}
#
class XmlSerializeParser:
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
        # List of XML array type files
        self.__include_array_files = []
        # Comment block of text for serializable
        self.__comment = ""
        # List of (name, type, comment) tuples
        self.__members = []
        # Type ID for serialized type
        self.__type_id = None
        #
        if not os.path.isfile(xml_file):
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            raise OSError(stri)
        fd = open(xml_file)
        xml_file = os.path.basename(xml_file)
        #        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file

        self.__config = ConfigManager.ConfigManager.getInstance()

        #

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd, parser=xml_parser)
        fd.close()  # Close the file, which is only used for the parsing above

        # Validate new imports using their root tag as a key to find what schema to use
        rng_file = self.__config.get(
            "schema", element_tree.getroot().tag.lower()
        ).lstrip("/")
        try:
            rng_file = locate_build_root(rng_file)
        except (BuildRootMissingException, BuildRootCollisionException) as bre:
            stri = "ERROR: Could not find specified RNG file {}. {}".format(
                rng_file,
                str(bre),
            )
            raise OSError(stri)
        file_handler = open(rng_file)
        relax_parsed = etree.parse(file_handler)
        file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            msg = "XML file {} is not valid according to schema {}.".format(
                xml_file, rng_file
            )
            raise FprimeXmlException(msg)

        serializable = element_tree.getroot()
        if serializable.tag != "serializable":
            PRINT.info("%s is not a serializable definition file" % xml_file)
            sys.exit(-1)

        print("Parsing Serializable %s" % serializable.attrib["name"])

        self.__name = serializable.attrib["name"]

        if "namespace" in serializable.attrib:
            self.__namespace = serializable.attrib["namespace"]
        else:
            self.__namespace = None

        if "typeid" in serializable.attrib:
            self.__type_id = serializable.attrib["typeid"]
        else:
            self.__type_id = None

        for serializable_tag in serializable:
            if serializable_tag.tag == "comment":
                self.__comment = serializable_tag.text.strip()
            elif serializable_tag.tag == "include_header":
                self.__include_header_files.append(serializable_tag.text)
            elif serializable_tag.tag == "import_serializable_type":
                self.__includes.append(serializable_tag.text)
            elif serializable_tag.tag == "import_enum_type":
                self.__include_enum_files.append(serializable_tag.text)
            elif serializable_tag.tag == "import_array_type":
                self.__include_array_files.append(serializable_tag.text)
            elif serializable_tag.tag == "members":
                for member in serializable_tag:
                    if member.tag != "member":
                        PRINT.info(
                            "%s: Invalid tag %s in serializable member definition"
                            % (xml_file, member.tag)
                        )
                        sys.exit(-1)
                    n = member.attrib["name"]
                    t = member.attrib["type"]
                    if "array_size" in list(member.attrib.keys()):
                        if t == "ENUM":
                            PRINT.info(
                                "%s: Member %s: arrays of enums not supported yet!"
                                % (xml_file, n)
                            )
                            sys.exit(-1)
                        array_size = member.attrib["array_size"]
                        if not array_size.isdigit():
                            PRINT.info(
                                "{}: Member {}: array_size must be a number".format(
                                    xml_file, n
                                )
                            )
                            sys.exit(-1)
                    else:
                        array_size = None

                    if "size" in list(member.attrib.keys()):
                        if t == "ENUM":
                            PRINT.info(
                                "%s: Member %s: arrays of enums not supported yet!"
                                % (xml_file, n)
                            )
                            sys.exit(-1)
                        size = member.attrib["size"]
                        if not size.isdigit():
                            PRINT.info(
                                "{}: Member {}: array_size must be a number".format(
                                    xml_file, n
                                )
                            )
                            sys.exit(-1)
                        if t != "string":
                            PRINT.info(
                                "%s: Member %s: size is only valid for string members"
                                % (xml_file, n)
                            )
                            sys.exit(-1)

                    else:
                        size = None

                    if "format" in list(member.attrib.keys()):
                        f = member.attrib["format"]
                    else:
                        if t in list(format_dictionary.keys()):
                            f = format_dictionary[t]
                        else:  # Must be included type, which will use toString method
                            f = "%s"
                    if t == "string":
                        if size is None:
                            PRINT.info(
                                "%s: member %s string must specify size tag"
                                % (xml_file, member.tag)
                            )
                            sys.exit(-1)

                    if "comment" in list(member.attrib.keys()):
                        c = member.attrib["comment"]
                    else:
                        c = None

                    d = None

                    for member_tag in member:
                        if member_tag.tag == "enum" and t == "ENUM":
                            en = member_tag.attrib["name"]
                            enum_members = []
                            for mem in member_tag:
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
                        elif member_tag.tag == "default":
                            d = member_tag.text
                        else:
                            PRINT.info(
                                "%s: Invalid member tag %s in serializable member %s"
                                % (xml_file, member_tag.tag, n)
                            )
                            sys.exit(-1)

                    self.__members.append((n, t, array_size, size, f, c, d))

        #
        # Generate a type id here using SHA256 algorithm and XML stringified file.
        #

        if not "typeid" in serializable.attrib:
            s = etree.tostring(element_tree.getroot())
            h = hashlib.sha256(s)
            n = h.hexdigest()
            self.__type_id = "0x" + n.upper()[-8:]

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

    def get_include_arrays(self):
        """
        Returns a list of all imported XML array files.
        """
        return self.__include_array_files

    def get_comment(self):
        """
        Return text block string of comment for serializable class.
        """
        return self.__comment

    def get_members(self):
        """
        Returns a list of member (name, type, optional array size, optional size, optional format, optional comment) needed.
        """
        return self.__members
