#!/usr/bin/env python3
# ===============================================================================
# NAME: implgen.py
#
# DESCRIPTION: A tool for autocoding component XMl files to generate impl cpp/hpp files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: July 8, 2019
#
# Copyright 2019, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import logging
import os
import sys
import time
from optparse import OptionParser

from fprime_ac.generators.writers import ImplCppWriter, ImplHWriter

# Meta-model for Component only generation
from fprime_ac.models import CompFactory

# Parsers to read the XML
from fprime_ac.parsers import (
    XmlComponentParser,
    XmlParser,
    XmlPortsParser,
    XmlSerializeParser,
)
from fprime_ac.utils import ConfigManager, Logger
from fprime_ac.utils.buildroot import get_build_roots, search_for_file, set_build_roots
from utils.pathmaker import setup_fprime_autocoder_path

setup_fprime_autocoder_path()


# Generators to produce the code must exist. Check her by importing and asserting
try:
    from fprime_ac.generators import GenFactory

    assert GenFactory is not None
except (ImportError, AssertionError) as ime:
    print("[ERROR] Cheetah templates need to be generated.\n\t", ime, file=sys.stderr)
    sys.exit(1)

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

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
    id = "0.1"
    comment = "Initial prototype"


VERSION = Version()


def pinit():
    """
    Initialize the option parser and return it.
    """
    usage = "usage: %prog [options] [xml_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = "Testgen creates the Tester.cpp, Tester.hpp, GTestBase.cpp, GTestBase.hpp, TesterBase.cpp, and TesterBase.hpp test component."

    parser = OptionParser(usage, version=vers, epilog=program_longdesc)

    parser.add_option(
        "-b",
        "--build_root",
        dest="build_root_overwrite",
        type="string",
        help="Overwrite environment variable BUILD_ROOT",
        default=None,
    )

    parser.add_option(
        "-v",
        "--verbose",
        dest="verbose_flag",
        help="Enable verbose mode showing more runtime detail (def: False)",
        action="store_true",
        default=False,
    )

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
        del xml_parser_obj

    parsed_serializable_xml_list = []
    #
    # Configure the meta-model for the component
    #
    serializable_type_files_list = (
        the_parsed_component_xml.get_serializable_type_files()
    )
    for serializable_file in serializable_type_files_list:
        serializable_file = search_for_file("Serializable", serializable_file)
        xml_parser_obj = XmlSerializeParser.XmlSerializeParser(
            serializable_file
        )  # Telemetry/Params can only use generated serializable types
        # check to make sure that the serializables don't have things that channels and parameters can't have
        # can't have external non-xml members
        if len(xml_parser_obj.get_include_header_files()):
            print(
                "ERROR: Component include serializables cannot use user-defined types. file: "
                % serializable_file
            )
            sys.exit(-1)

        parsed_serializable_xml_list.append(xml_parser_obj)
        del xml_parser_obj

    model = CompFactory.CompFactory.getInstance()
    component_model = model.create(
        the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list
    )

    return component_model


def generate_impl_files(opt, component_model):
    """
    Generates impl cpp/hpp files
    """
    implFiles = []

    if VERBOSE:
        print("Generating impl files for " + component_model.get_xml_filename())

    # ComponentImpl.cpp
    implFiles.append(ImplCppWriter.ImplCppWriter())

    # ComponentImpl.h
    implFiles.append(ImplHWriter.ImplHWriter())

    #
    # The idea here is that each of these generators is used to create
    # a certain portion of each output file.
    #
    for file in implFiles:
        file.setFileName(component_model)
        if os.path.exists(file.toString()):
            print("WARNING: " + file.toString() + " already exists! Exiting...")
            sys.exit(-1)

        file.write(component_model)
        if VERBOSE:
            print("Generated %s" % file.toString())

    if VERBOSE:
        print("Generated impl files for " + component_model.get_xml_filename())


def main():
    """
    Main program.
    """
    global VERBOSE
    global BUILD_ROOT
    global DEPLOYMENT

    # Enable printing model error output to STDOUT
    Logger.connectOutputLogger(None)

    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag
    ConfigManager.ConfigManager.getInstance()

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
    if not opt.build_root_overwrite is None:
        set_build_roots(opt.build_root_overwrite)
        if VERBOSE:
            print("BUILD_ROOT set to %s" % ",".join(get_build_roots()))
    else:
        if ("BUILD_ROOT" in os.environ.keys()) == False:
            print("ERROR: Build root not set to root build path...")
            sys.exit(-1)
        set_build_roots(os.environ["BUILD_ROOT"])
        if VERBOSE:
            print("BUILD_ROOT set to %s" % ",".join(get_build_roots()))

    #
    # Write test component
    #
    if not "Ai" in xml_filename:
        print("ERROR: Missing Ai at end of file name...")
        raise OSError
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
        generate_impl_files(opt, component_model)
    else:
        print(
            "ERROR: {} is used for component XML files, not {} XML files".format(
                sys.argv[0], xml_type
            )
        )
        sys.exit(-1)

    sys.exit(0)


if __name__ == "__main__":
    main()
