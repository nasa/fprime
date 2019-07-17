#!/usr/bin/env python
#===============================================================================
# NAME: cosmosgen.py
#
# DESCRIPTION: A tool for autocoding component XML files into unit test cpp/hpp files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: July 8, 2019
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import os
import sys
import time
import glob
import logging


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

#Generators to produce the code
from fprime_ac.generators import GenFactory

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

# Used by unit test to disable things.
TEST = False

# After catching exception this is set True
# so a clean up routine deletes *_ac_*.[ch]
# and *_ac_*.xml files within module.
ERROR = False

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

    parser = OptionParser(usage, version=vers)

    parser.add_option("-b", "--build_root", dest="build_root_flag",
        help="Enable search for enviornment variable BUILD_ROOT to establish absolute XML directory path",
        action="store_true", default=False)
        
    parser.add_option("-p", "--path", dest="work_path", type="string",
        help="Switch to new working directory (def: %s)." % current_dir,
        action="store", default=current_dir)

    parser.add_option("-v", "--verbose", dest="verbose_flag",
        help="Enable verbose mode showing more runtime detail (def: False)",
        action="store_true", default=False)

    return parser

def parse_component(the_parsed_component_xml, xml_filename, opt , topology_model = None):
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
            PRINT.info("ERROR: Component include serializables cannot use user-defined types. file: " % serializable_file)
            sys.exit(-1)

        parsed_serializable_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

    generator = CompFactory.CompFactory.getInstance()
    component_model = generator.create(the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list)

    if not "Ai" in xml_filename:
        PRINT.info("Missing Ai at end of file name...")
        raise IOError

    return component_model

def generate_tests(opt, component_model):
    unitTestFiles = []
    print("Generating test files for " + component_model.get_xml_filename())
    # h_instance_test_name ComponentTestHWriter
    unitTestFiles.append(ComponentTestHWriter.ComponentTestHWriter())
    
    # cpp_instance_test_name ComponentTestCppWriter
    unitTestFiles.append(ComponentTestCppWriter.ComponentTestCppWriter())
    
    # h_instance_gtest_name GTestHWriter
    unitTestFiles.append(GTestHWriter.GTestHWriter())
    
    # cpp_instance_gtest_name GTestCppVisitor
    unitTestFiles.append(GTestCppWriter.GTestCppWriter())
    
    # h_instance_test_impl_name TestImplHVisitor
    unitTestFiles.append(TestImplHWriter.TestImplHWriter())
    
    # cpp_instance_test_impl_name TestImplCppVisitor
    unitTestFiles.append(TestImplCppWriter.TestImplCppWriter())
    
    #
    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    for file in unitTestFiles:
        file.write(component_model)
        if VERBOSE:
            print("Generated %s"%file.toString())

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
        PRINT.info("ERROR: %s xml specification file %s does not exist!" % (file_type,file_path))
        sys.exit(-1)

def main():
    """
    Main program.
    """
    global VERBOSE
    global ERROR
    global DEPLOYMENT
    
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag
    CONFIG = ConfigManager.ConfigManager.getInstance()
    ERROR = False
    
    # Check that the specified working directory exists. Remember, the
    # default working directory is the current working directory which
    # always exists. We are basically only checking for when the user
    # specifies an alternate working directory.
    
    if os.path.exists(opt.work_path) == False:
        Parser.error('Specified path does not exist (%s)!' % opt.work_path)
    
    working_dir = opt.work_path
    
    #
    # Handle command line arguments
    #
    
    # Get the current working directory so that we can return to it at end
    STARTING_DIRECTORY = os.getcwd()
    os.chdir(working_dir)
    
    # Check for BUILD_ROOT env. variable
    if ('BUILD_ROOT' in os.environ.keys()) == False:
        print("ERROR: Build root not set to root build path...")
        sys.exit(-1)
    else:
        # Handle BUILD_ROOT
        BUILD_ROOT = os.environ['BUILD_ROOT']
        ModelParser.BUILD_ROOT = BUILD_ROOT
        if VERBOSE:
            print("BUILD_ROOT set to %s in environment" % BUILD_ROOT)
    
    #
    #  Parse the input Topology XML filename
    #
    if len(args) == 0:
        print("ERROR: Usage: %s [options] xml_filename" % sys.argv[0])
        return
    else:
        xml_filenames = args[0:]

    #
    # Check for BUILD_ROOT variable for XML port searches
    #
    if opt.build_root_flag == True:
        # Check for BUILD_ROOT env. variable
        if ('BUILD_ROOT' in list(os.environ.keys())) == False:
            PRINT.info("ERROR: The -b command option requires that BUILD_ROOT environmental variable be set to root build path...")
            sys.exit(-1)
        else:
            BUILD_ROOT = os.environ['BUILD_ROOT']
            ModelParser.BUILD_ROOT = BUILD_ROOT
    
    #
    # Write tests
    #
    for xml_filename in xml_filenames:
        #
        # Create python dictionaries
        #
        
        xml_filename = os.path.basename(xml_filename)
        xml_type = XmlParser.XmlParser(xml_filename)()
        
        # Only Components can be inputted
        if xml_type == "component":
            DEBUG.info("Detected Component XML so Generating Component C++ Files...")
            the_parsed_component_xml = XmlComponentParser.XmlComponentParser(xml_filename)
            component_model = parse_component(the_parsed_component_xml, xml_filename, opt)
            print("\nGenerating tests...")
            generate_tests(opt, component_model)
        else:
            PRINT.info("Invalid XML found...this format not supported")
            ERROR=True

    # Always return to directory where we started.
    os.chdir(STARTING_DIRECTORY)

    if ERROR == True:
        sys.exit(-1)
    else:
        sys.exit(0)


if __name__ == '__main__':
    main()
