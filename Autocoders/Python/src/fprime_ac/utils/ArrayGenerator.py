#!/usr/bin/env python3
#===============================================================================
# NAME: EnumGenerator.py
#
# DESCRIPTION: A generator to produce serializable enum's
#
# AUTHOR: jishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : May 28, 2020
#
# Copyright 2020, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import sys
import os
from fprime_ac.parsers import XmlParser
from fprime_ac.parsers import XmlArrayParser
from fprime_ac.generators.templates.arrays import array_cpp
from fprime_ac.generators.templates.arrays import array_hpp
from fprime_ac.generators.templates.arrays import array_py

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
        filename = name + "ArrayAc." + type
    #
    fp = open(filename,'w')
    if fp == None:
        print("Could not open file %s" % filename)
        sys.exit(-1)
    return fp

def write_template(fp, c, name, namespace, arr_type, arr_size):
    '''
    Set up and write out templates here
    '''
    c.name = name
    c.namespace = namespace
    c.type = arr_type
    c.size = arr_size
    c.items_list = items
    fp.writelines(c.__str__())

def generate_array(xml_file):
    """
    Produce a *Ac.hpp, *Ac.cpp, and *.py files for serializable arrays.
    Return True if xml file was an array, otherwise return False and
    generate nothing.
    """
    xml = XmlParser.XmlParser(xml_file)
    if xml() == 'array':
        #
        # Parse array xml here
        #
        array_xml = XmlArrayParser.XmlArrayParser(xml_file)
        name = array_xml.get_name()
        namespace = array_xml.get_namespace()
        arr_type = array_xml.get_type()
        arr_size = array_xml.get_size()
        #
        # Generate the hpp file
        #
        fp = open_file(name, "hpp")
        c = array_hpp.array_hpp()
        write_template(fp, c, name, namespace, arr_type, arr_size)
        fp.close()
        #
        # Generate the cpp file
        #
        fp = open_file(name, "cpp")
        c = array_cpp.array_cpp()
        write_template(fp, c, name, namespace, arr_type, arr_size)
        fp.close()
        #
        # Generate the py file
        #
        fp = open_file(name, "py")
        c = array_py.array_py()
        write_template(fp, c, name, namespace, arr_type, arr_size)
        fp.close()
        return True
    else:
        return False

if __name__ == '__main__':
    xmlfile = sys.argv[1]
    print(generate_array(xmlfile))
