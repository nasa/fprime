#!/usr/bin/env python
#===============================================================================
# NAME: testgen.py
#
# DESCRIPTION: A tool for autocoding component XML files into a test component
# with unit test cpp/hpp files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: July 8, 2019
#
# Copyright 2019, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
from utils.pathmaker import setup_fprime_autocoder_path
setup_fprime_autocoder_path()
import os
import sys
import time
import glob
import logging
import argparse


from optparse import OptionParser

from fprime_ac.utils import Logger
from fprime_ac.utils import ConfigManager
from fprime_ac.utils import DictTypeConverter


# Meta-model for Component only generation
from fprime_ac.models import CompFactory
from fprime_ac.models import PortFactory
from fprime_ac.models import TopoFactory
from fprime_ac.models import Serialize
from fprime_ac.models import ModelParser


# Parsers to read the XML
from fprime_ac.parsers import XmlParser
from fprime_ac.parsers import XmlComponentParser
from fprime_ac.parsers import XmlPortsParser
from fprime_ac.parsers import XmlSerializeParser

from lxml import etree

from fprime_ac.generators.writers import ComponentTestHWriter
from fprime_ac.generators.writers import ComponentTestCppWriter
from fprime_ac.generators.writers import GTestHWriter
from fprime_ac.generators.writers import GTestCppWriter
from fprime_ac.generators.writers import TestImplHWriter
from fprime_ac.generators.writers import TestImplCppWriter
from fprime_ac.generators.writers import TestMainWriter

#Generators to produce the code
from fprime_ac.generators import GenFactory

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

# Used by unit test to disable things.
TEST = False

# Configuration manager object.
CONFIG = ConfigManager.ConfigManager.getInstance()

# Build a default log file name
SYS_TIME = time.gmtime()

# Build Root environmental variable if one exists.
BUILD_ROOT = None

# Deployment name from topology XML only
DEPLOYMENT = None

# Version label for now
class Version:
    id      = "0.1"
    comment = "Initial prototype"
VERSION = Version()

def pinit():
    """
    Initialize the option parser and return it.
    """
    
    current_dir = os.getcwd()

    usage = "usage: %prog [options] [xml_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = "Testgen creates the Tester.cpp, Tester.hpp, GTestBase.cpp, GTestBase.hpp, TesterBase.cpp, and TesterBase.hpp test component."

    parser = OptionParser(usage, version=vers, epilog=program_longdesc)

    parser.add_option("-b", "--build_root", dest="build_root_overwrite", type="string", help="Overwrite environment variable BUILD_ROOT", default=None)
    
    parser.add_option("-m", "--maincpp", dest="maincpp",
        help="Autocodes main.cpp file with proper test cases",
        action="store_true", default=False)
                      
    parser.add_option("-f", "--force_tester", dest="force_tester",
        help="Forces the generation of Tester.hpp and Tester.cpp",
        action="store_true", default = False)

    parser.add_option("-v", "--verbose", dest="verbose_flag", help="Enable verbose mode showing more runtime detail (def: False)", action="store_true", default=False)

    return parser

def parse_component(the_parsed_component_xml, xml_filename, opt):
    """
    Creates a component meta-model and generates the
    component files.  Nothing is returned.
    """
    global BUILD_ROOT
    #

    parsed_port_xml_list = []

    #
    # Configure the meta-model for the component
    #
    port_type_files_list = the_parsed_component_xml.get_port_type_files()

    for port_file in port_type_files_list:
        port_file = search_for_file("Port", port_file)
        xml_parser_obj = XmlPortsParser.XmlPortsParser(port_file)
        parsed_port_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

    parsed_serializable_xml_list = []
    #
    # Configure the meta-model for the component
    #
    serializable_type_files_list = the_parsed_component_xml.get_serializable_type_files()
    for serializable_file in serializable_type_files_list:
        serializable_file = search_for_file("Serializable", serializable_file)
        xml_parser_obj = XmlSerializeParser.XmlSerializeParser(serializable_file) # Telemetry/Params can only use generated serializable types
        # check to make sure that the serializables don't have things that channels and parameters can't have
        # can't have external non-xml members
        if len(xml_parser_obj.get_include_header_files()):
            print("ERROR: Component include serializables cannot use user-defined types. file: " % serializable_file)
            sys.exit(-1)

        parsed_serializable_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

    model = CompFactory.CompFactory.getInstance()
    component_model = model.create(the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list)

    return component_model

def generate_tests(opt, component_model):
    """
    Generates test component cpp/hpp files
    """
    unitTestFiles = []
    
    if VERBOSE:
        print("Generating test files for " + component_model.get_xml_filename())
    
    # TesterBase.hpp
    unitTestFiles.append(ComponentTestHWriter.ComponentTestHWriter())
    
    # TesterBase.cpp
    unitTestFiles.append(ComponentTestCppWriter.ComponentTestCppWriter())
    
    # GTestBase.hpp
    unitTestFiles.append(GTestHWriter.GTestHWriter())
    
    # GTestbase.cpp
    unitTestFiles.append(GTestCppWriter.GTestCppWriter())

    if not os.path.exists("Tester.hpp") or opt.force_tester:
        if os.path.exists("Tester.hpp"):
            os.remove("Tester.hpp")
            os.remove("Tester.cpp")

        # Tester.hpp
        unitTestFiles.append(TestImplHWriter.TestImplHWriter())
    
        # Tester.cpp
        unitTestFiles.append(TestImplCppWriter.TestImplCppWriter())
    
    #
    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    for file in unitTestFiles:
        file.write(component_model)
        if VERBOSE:
            print("Generated %s"%file.toString())

    time.sleep(3)

    ############################################################
    # Parses Tester.hpp file and uses all method definitions
    # between "// Tests" comment and "private:" descriptor to
    # generate TEST stubs in TestMain.cpp class
    
    if opt.maincpp:
        testhpp = open("Tester.hpp", "r")

        find_tests = False
        test_cases = []
        override_dict = {}
        override_name = None # // @Testname: decorator comment
        for line in testhpp:
            if "// Tests" in line:
                find_tests = True
            elif "private:" in line:
                find_tests = False
            
            if find_tests:
                if "// @Testname:" in line:
                    # Remove whitespace
                    comment = "".join(line.split())
                    override_name = line[line.index("// @Testname:") + 13:].strip()
                    # Store location of all names to override
                    override_dict[override_name] = len(test_cases)
                elif "void " in line:
                    # Parse method name out of definition
                    name = line[line.index("void ") + 5:].replace("(void);", "").strip()
                    test_cases.append(name)

        if len(test_cases) > 1:
            test_cases.remove("toDo")
    
        main_writer = TestMainWriter.TestMainWriter()
        
        main_writer.add_test_cases(test_cases)
        main_writer.override_names(override_dict)
        main_writer.write(component_model)
        if VERBOSE:
            print("Generated TestMain.cpp")
    else:
        if not os.path.exists("TestMain.cpp"):
            TestMainWriter.TestMainWriter().write(component_model)
            if VERBOSE:
                print("Generated TestMain.cpp")

    if VERBOSE:
        print("Generated test files for " + component_model.get_xml_filename())


def search_for_file(file_type, file_path):
    '''
    Searches for a given included port or serializable by looking in three places:
    - The specified BUILD_ROOT
    - The F Prime core
    - The exact specified path
    @param file_type: type of file searched for
    @param file_path: path to look for based on offset
    @return: full path of file
    '''
    core = os.environ.get("FPRIME_CORE_DIR", BUILD_ROOT)
    for possible in [BUILD_ROOT, core, None]:
        if not possible is None:
            checker = os.path.join(possible, file_path)
        else:
            checker = file_path
        if os.path.exists(checker):
            DEBUG.debug("%s xml type description file: %s" % (file_type,file_path))
            return checker
    else:
        print("ERROR: %s xml specification file %s does not exist!" % (file_type,file_path))
        sys.exit(-1)

def main():
    """
    Main program.
    """
    global VERBOSE
    global BUILD_ROOT
    global DEPLOYMENT
    
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag
    CONFIG = ConfigManager.ConfigManager.getInstance()
    
    #
    # Handle command line arguments
    #
    
    #
    #  Parse the input Topology XML filename
    #
    if len(args) == 0:
        print("ERROR: Usage: %s [options] xml_filename" % sys.argv[0])
        return
    elif len(args) == 1:
        xml_filename = args[0]
    else:
        print("ERROR: Too many filenames, should only have one")
        return

    #
    # Check for BUILD_ROOT variable for XML port searches
    #
    if not opt.build_root_overwrite == None:
        BUILD_ROOT = opt.build_root_overwrite
        ModelParser.BUILD_ROOT = BUILD_ROOT
        if VERBOSE:
            print("BUILD_ROOT set to %s" % BUILD_ROOT)
    else:
        if ('BUILD_ROOT' in os.environ.keys()) == False:
            print("ERROR: Build root not set to root build path...")
            sys.exit(-1)
        BUILD_ROOT = os.environ['BUILD_ROOT']
        ModelParser.BUILD_ROOT = BUILD_ROOT
        if VERBOSE:
            print("BUILD_ROOT set to %s in environment" % BUILD_ROOT)
    
    #
    # Write test component
    #
    if not "Ai" in xml_filename:
        print("ERROR: Missing Ai at end of file name...")
        raise IOError
    #
    # Create python dictionaries
    #
    print(xml_filename)
    xml_type = XmlParser.XmlParser(xml_filename)()
    
    # Only Components can be inputted
    if xml_type == "component":
        if VERBOSE:
            print("Detected Component XML so GeneratingComponent C++ Files...")
        the_parsed_component_xml = XmlComponentParser.XmlComponentParser(xml_filename)
        component_model = parse_component(the_parsed_component_xml, xml_filename, opt)
        if VERBOSE:
            print("\nGenerating tests...")
        generate_tests(opt, component_model)
    else:
        print("ERROR: Invalid XML found...this format not supported")
        sys.exit(-1)

    sys.exit(0)


if __name__ == '__main__':
    main()
