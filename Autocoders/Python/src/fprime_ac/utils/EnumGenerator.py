#!/bin/env python
#===============================================================================
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
#===============================================================================

import sys
import os
from fprime_ac.parsers import XmlParser
from fprime_ac.parsers import XmlEnumParser
from fprime_ac.generators.templates.enums import enum_cpp
from fprime_ac.generators.templates.enums import enum_hpp
from fprime_ac.generators.templates.enums import enum_py

def open_file(name, type):
    '''
    Open the file for writing
    '''
    #
    gse_serializable_install_dir = "DefaultDict" + os.sep + "serializable"
    if type == "py":
        filename = name + ".py"
        #
        # Put Gse serializable is correct place for make system
        #
        if not os.path.exists(gse_serializable_install_dir):
            os.makedirs(gse_serializable_install_dir)
        os.chdir(gse_serializable_install_dir)
    else:
        filename = name + "EnumAc." + type
    #
    fp = open(filename,'w')
    if fp == None:
        print("Could not open file %s" % filename)
        sys.exit(-1)
    return fp

def write_template(fp, c, name, namespace, items, max_value):
    '''
    Set up and write out templates here
    '''
    c.name = name
    c.namespace = namespace
    c.items_list = items
    c.max_value = max_value
    fp.writelines(c.__str__())

def generate_enum(xml_file):
    """
    Produce a *Ac.hpp, *Ac.cpp, and *.py files for serializable enum's.
    Return True if xml file was an enum, othersise return False and
    generate nothing.
    """
    xml = XmlParser.XmlParser(xml_file)
    if xml() == 'enum':
        #
        # Parse enum xml here
        #
        enum_xml = XmlEnumParser.XmlEnumParser(xml_file)
        name = enum_xml.get_name()
        namespace = enum_xml.get_namespace()
        items = enum_xml.get_items()
        max_value = enum_xml.get_max_value()
        #
        # Generate the hpp file
        #
        fp = open_file(name, "hpp")
        c = enum_hpp.enum_hpp()
        write_template(fp, c, name, namespace, items, max_value)
        fp.close()
        #
        # Generate the cpp file
        #
        fp = open_file(name, "cpp")
        c = enum_cpp.enum_cpp()
        write_template(fp, c, name, namespace, items, max_value)
        fp.close()
        #
        # Generate the py file
        #
        fp = open_file(name, "py")
        c = enum_py.enum_py()
        write_template(fp, c, name, namespace, items, max_value)
        fp.close()
        return True
    else:
        return False

if __name__ == '__main__':
    xmlfile = sys.argv[1]
    print(generate_enum(xmlfile))
    
