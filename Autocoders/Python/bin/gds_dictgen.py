#!/usr/bin/env python3
# ===============================================================================
# NAME: gds_dictgen.py
#
# DESCRIPTION: A tool for generating GDS xml dictionaries.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: July 6, 2019
#
# Copyright 2019, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================

import logging
import os
import sys
from optparse import OptionParser

from lxml import etree

# Meta-model for Component only generation
from fprime_ac.models import TopoFactory

# Parsers to read the XML
from fprime_ac.parsers import XmlParser, XmlTopologyParser
from fprime_ac.utils import ConfigManager, TopDictGenerator
from fprime_ac.utils.buildroot import get_build_roots, set_build_roots
from fprime_ac.utils.version import get_fprime_version

# Generators to produce the code
try:
    from fprime_ac.generators import GenFactory
except ImportError as ime:
    print("[ERROR] Cheetah templates need to be generated.\n\t", ime, file=sys.stderr)
    sys.exit(1)

# Needs to be initialized to create the other parsers
CONFIG = ConfigManager.ConfigManager.getInstance()

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

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
    usage = "usage: %prog [options] [xml_topology_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = (
        """This script reads F' topology XML and produces GDS XML Dictionaries."""
    )

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
        default=None,
    )

    return parser


def generate_xml_dict(the_parsed_topology_xml, xml_filename, opt):
    """
    Generates GDS XML dictionary from parsed topology XML
    """
    if VERBOSE:
        print("Topology xml type description file: %s" % xml_filename)
    model = TopoFactory.TopoFactory.getInstance()
    topology_model = model.create(the_parsed_topology_xml)

    GenFactory.GenFactory.getInstance()

    # uses the topology model to process the items
    # create list of used parsed component xmls
    parsed_xml_dict = {}
    for comp in the_parsed_topology_xml.get_instances():
        if comp.get_type() in topology_model.get_base_id_dict():
            parsed_xml_dict[comp.get_type()] = comp.get_comp_xml()
        else:
            PRINT.info(
                "Components with type {} aren't in the topology model.".format(
                    comp.get_type()
                )
            )

    #
    xml_list = []
    for parsed_xml_type in parsed_xml_dict:
        if parsed_xml_dict[parsed_xml_type] is None:
            print(
                "ERROR: XML of type {} is being used, but has not been parsed correctly. Check if file exists or add xml file with the 'import_component_type' tag to the Topology file.".format(
                    parsed_xml_type
                )
            )
            raise Exception()
        xml_list.append(parsed_xml_dict[parsed_xml_type])

    topology_model.set_instance_xml_list(xml_list)

    topology_dict = etree.Element("dictionary")
    topology_dict.attrib["topology"] = the_parsed_topology_xml.get_name()
    topology_dict.attrib["framework_version"] = get_fprime_version()

    top_dict_gen = TopDictGenerator.TopDictGenerator(
        parsed_xml_dict, print if VERBOSE else lambda _: None
    )
    for comp in the_parsed_topology_xml.get_instances():
        comp_type = comp.get_type()
        comp_name = comp.get_name()
        comp_id = int(comp.get_base_id())
        PRINT.debug(
            "Processing {} [{}] ({})".format(comp_name, comp_type, hex(comp_id))
        )

        top_dict_gen.set_current_comp(comp)

        top_dict_gen.check_for_serial_xml()
        top_dict_gen.check_for_commands()
        top_dict_gen.check_for_channels()
        top_dict_gen.check_for_events()
        top_dict_gen.check_for_parameters()

    top_dict_gen.remove_duplicate_enums()

    topology_dict.append(top_dict_gen.get_enum_list())
    topology_dict.append(top_dict_gen.get_serializable_list())
    topology_dict.append(top_dict_gen.get_command_list())
    topology_dict.append(top_dict_gen.get_event_list())
    topology_dict.append(top_dict_gen.get_telemetry_list())
    topology_dict.append(top_dict_gen.get_parameter_list())

    fileName = the_parsed_topology_xml.get_xml_filename().replace(
        "Ai.xml", "Dictionary.xml"
    )
    if VERBOSE:
        print("Generating XML dictionary %s" % fileName)
    fd = open(
        fileName, "wb"
    )  # Note: binary forces the same encoding of the source files
    fd.write(etree.tostring(topology_dict, pretty_print=True))
    if VERBOSE:
        print("Generated XML dictionary %s" % fileName)

    return topology_model


def main():
    """
    Main program.
    """
    global VERBOSE  # prevent local creation of variable
    global DEPLOYMENT  # deployment set in topology xml only and used to install new instance dicts

    ConfigManager.ConfigManager.getInstance()
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag

    #
    #  Parse the input Topology XML filename
    #
    if len(args) == 0:
        print("Usage: %s [options] xml_filename" % sys.argv[0])
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

    if not "Ai" in xml_filename:
        print("ERROR: Missing Ai at end of file name...")
        raise OSError

    xml_type = XmlParser.XmlParser(xml_filename)()

    if xml_type == "assembly" or xml_type == "deployment":
        if VERBOSE:
            print("Detected Topology XML so Generating Topology C++ Files...")
        the_parsed_topology_xml = XmlTopologyParser.XmlTopologyParser(xml_filename)
        DEPLOYMENT = the_parsed_topology_xml.get_deployment()
        print("Found assembly or deployment named: %s\n" % DEPLOYMENT)
        generate_xml_dict(the_parsed_topology_xml, xml_filename, opt)
    else:
        PRINT.info("Invalid XML found...this format not supported")
        sys.exit(-1)

    sys.exit(0)


if __name__ == "__main__":
    main()
