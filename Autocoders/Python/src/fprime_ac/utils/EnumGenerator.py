#!/usr/bin/env python3
# ===============================================================================
# NAME: EnumGenerator.py
#
# DESCRIPTION: A generator to produce serializable enum's
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : April 6, 2015
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================

import sys

from fprime_ac.parsers import XmlEnumParser, XmlParser

try:
    from fprime_ac.generators.templates.enums import enum_cpp, enum_hpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


def open_file(name, type):
    """
    Open the file for writing
    """

    filename = name + "EnumAc." + type
    fp = open(filename, "w")
    if fp is None:
        print("Could not open file %s" % filename)
        sys.exit(-1)
    return fp


def write_template(
    fp,
    c,
    name,
    namespace,
    namespace_list,
    default,
    serialize_type,
    items,
    max_value,
    comment,
):
    """
    Set up and write out templates here
    """
    c.name = name
    c.namespace = namespace
    c.namespace_list = namespace_list
    c.default = default
    c.serialize_type = serialize_type
    c.items_list = items
    c.max_value = max_value
    c.comment = comment
    fp.writelines(c.__str__())


def generate_enum(xml_file):
    """
    Produce a *Ac.hpp, *Ac.cpp, and *.py files for serializable enum's.
    Return True if xml file was an enum, otherwise return False and
    generate nothing.
    """
    xml = XmlParser.XmlParser(xml_file)
    if xml() == "enum":
        #
        # Parse enum xml here
        #
        enum_xml = XmlEnumParser.XmlEnumParser(xml_file)
        name = enum_xml.get_name()
        namespace = enum_xml.get_namespace()
        namespace_list = None
        if namespace is not None:
            namespace_list = enum_xml.get_namespace().split("::")
        default = enum_xml.get_default()
        serialize_type = enum_xml.get_serialize_type()
        items = enum_xml.get_items()
        max_value = enum_xml.get_max_value()
        comment = enum_xml.get_comment()
        #
        # Generate the hpp file
        #
        fp = open_file(name, "hpp")
        c = enum_hpp.enum_hpp()
        write_template(
            fp,
            c,
            name,
            namespace,
            namespace_list,
            default,
            serialize_type,
            items,
            max_value,
            comment,
        )
        fp.close()
        #
        # Generate the cpp file
        #
        fp = open_file(name, "cpp")
        c = enum_cpp.enum_cpp()
        write_template(
            fp,
            c,
            name,
            namespace,
            namespace_list,
            default,
            serialize_type,
            items,
            max_value,
            comment,
        )
        fp.close()
        return True
    else:
        return False


if __name__ == "__main__":
    xmlfile = sys.argv[1]
    print(generate_enum(xmlfile))
