#!/bin/env python
#===============================================================================
# NAME: cosmosgen.py
#
# DESCRIPTION: A tool for autocoding topology XML files into GDS python dictionaries.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import os
import sys
import logging

from optparse import OptionParser
from fprime_ac.models import ModelParser
from fprime_ac.utils import ConfigManager

from fprime_ac.generators.writers import DictGenerator

from fprime_ac.generators.writers import ChannelWriter
from fprime_ac.generators.writers import EventWriter
from fprime_ac.generators.writers import CommandWriter
from fprime_ac.generators.writers import InstCommandWriter
from fprime_ac.generators.writers import InstChannelWriter
from fprime_ac.generators.writers import InstEventWriter

# Needs to be initialized to create the other parsers
CONFIG = ConfigManager.ConfigManager.getInstance()

# Flag to indicate verbose mode.
VERBOSE = False

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

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
    
    # Format build root for program description
    if ('BUILD_ROOT' in os.environ.keys()) == True:
        root_dir = os.environ["BUILD_ROOT"]
    else:
        print("ERROR: Build root not set to root build path...")
        sys.exit(-1)

    usage = "usage: %prog [options] [xml_topology_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = '''
        This script reads F' topology XML and produces .txt and ruby files that configure a COSMOS target.
        These can be used to send commands and receive telemetry within the COSMOS system. Full documentation for this tool can be found at %sAutocoders/Python/src/fprime_ac/utils/cosmos''' % root_dir
    program_license = "Copyright 2018 user_name (California Institute of Technology) \
    ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged."

    parser = OptionParser(usage, version=vers, epilog=program_longdesc,description=program_license)

    parser.add_option("-v", "--verbose", dest="verbose_flag",
        help="Enable verbose mode showing more runtime detail (def: False)",
        action="store_true", default=False)
                      
    return parser

def generate_topology(the_parsed_topology_xml, xml_filename, opt):
    DEBUG.debug("Topology xml type description file: %s" % xml_filename)
    generator = TopoFactory.TopoFactory.getInstance()
    topology_model = generator.create(the_parsed_topology_xml)
    
    #create list of used parsed component xmls
    parsed_xml_dict = {}
    for comp in the_parsed_topology_xml.get_instances():
        if comp.get_type() in topology_model.get_base_id_dict():
            parsed_xml_dict[comp.get_type()] = comp.get_comp_xml()
        #comp.set_component_object(comp.)
        else:
            PRINT.info( "Components with type {} aren't in the topology model.".format(comp.get_type()))

    #
    # Hack to set up deployment path for instanced dictionaries (if one exists remove old one)
    #
    if opt.default_topology_dict:
        os.environ["DICT_DIR"] = os.environ.get("FPRIME_CORE_DIR", BUILD_ROOT) + os.sep + DEPLOYMENT + os.sep + "py_dict"
        dict_dir = os.environ["DICT_DIR"]
        PRINT.info("Removing old instanced topology dictionaries in: %s", dict_dir)
        import shutil
        if os.path.exists(dict_dir):
            shutil.rmtree(dict_dir)
        PRINT.info("Overriding for instanced topology dictionaries the --dict_dir option with xml derived path: %s", dict_dir)

    xml_list = []
    for parsed_xml_type in parsed_xml_dict:
        if parsed_xml_dict[parsed_xml_type] == None:
            PRINT.info("XML of type {} is being used, but has not been parsed correctly. Check if file exists or add xml file with the 'import_component_type' tag to the Topology file.".format(parsed_xml_type))
            raise Exception()
        xml_list.append(parsed_xml_dict[parsed_xml_type])
        generate_component_instance_dictionary(parsed_xml_dict[parsed_xml_type] , opt , topology_model)

    topology_model.set_instance_xml_list(xml_list)



def generate_component_instance_dictionary(the_parsed_component_xml , opt , topology_model):
    global BUILD_ROOT
    global DEPLOYMENT
    
    parsed_port_xml_list = []
    parsed_serializable_xml_list = []
    #uses the topology model to process the items
    #checks if the topology model exists
    if topology_model == None:
        PRINT.info("Topology model was not specified. Please also input a topology model when running this command.")
        raise IOError
    
    port_type_files_list = the_parsed_component_xml.get_port_type_files()

    for port_file in port_type_files_list:
        port_file = search_for_file("Port", port_file)
        xml_parser_obj = XmlPortsParser.XmlPortsParser(port_file)
        #print xml_parser_obj.get_args()
        parsed_port_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

    serializable_type_files_list = the_parsed_component_xml.get_serializable_type_files()
    for serializable_file in serializable_type_files_list:
        serializable_file = search_for_file("Serializable", serializable_file)
        xml_parser_obj = XmlSerializeParser.XmlSerializeParser(serializable_file) # Telemetry/Params can only use generated serializable types
        # check to make sure that the serializables don't have things that channels and parameters can't have
        # can't have external non-xml members
        if len(xml_parser_obj.get_include_header_files()):
            PRINT.info("ERROR: Component include serializables cannot use user-defined types. file: " % serializable_file)
            sys.exit(-1)
        
        #print xml_parser_obj.get_args()
        parsed_serializable_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)


    generator = CompFactory.CompFactory.getInstance()
    component_model = generator.create(the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_xml_list)

    if opt.default_topology_dict:
        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor("Commands","InstanceCommandVisitor",True,True)
        for command_model in component_model.get_commands():
            DEBUG.info("Processing command %s"%command_model.get_mnemonic())
            defaultStartCmd = default_dict_generator.create("InstanceDictStart")
            defaultCmdHeader = default_dict_generator.create("InstanceDictHeader")
            defaultCmdBody = default_dict_generator.create("InstanceDictBody")
            
            defaultStartCmd(command_model , topology_model)
            defaultCmdHeader(command_model , topology_model)
            defaultCmdBody(command_model , topology_model)
    
        for parameter_model in component_model.get_parameters():
            DEBUG.info("Processing parameter %s"%parameter_model.get_name())
            defaultStartCmd = default_dict_generator.create("InstanceDictStart")
            defaultCmdHeader = default_dict_generator.create("InstanceDictHeader")
            defaultCmdBody = default_dict_generator.create("InstanceDictBody")
            
            defaultStartCmd(parameter_model , topology_model)
            defaultCmdHeader(parameter_model , topology_model)
            defaultCmdBody(parameter_model , topology_model)
        
        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor("Events","InstanceEventVisitor",True,True)
        for event_model in component_model.get_events():
            DEBUG.info("Processing event %s"%event_model.get_name())
            defaultStartEvent = default_dict_generator.create("InstanceDictStart")
            defaultEventHeader = default_dict_generator.create("InstanceDictHeader")
            defaultEventBody = default_dict_generator.create("InstanceDictBody")
            
            defaultStartEvent(event_model , topology_model)
            defaultEventHeader(event_model , topology_model)
            defaultEventBody(event_model , topology_model)
        
        default_dict_generator = GenFactory.GenFactory.getInstance()
        # iterate through command instances
        default_dict_generator.configureVisitor("Channels","InstanceChannelVisitor",True,True)
        for channel_model in component_model.get_channels():
            DEBUG.info("Processing channel %s"%channel_model.get_name())
            defaultStartChannel = default_dict_generator.create("InstanceDictStart")
            defaultChannelHeader = default_dict_generator.create("InstanceDictHeader")
            defaultChannelBody = default_dict_generator.create("InstanceDictBody")
            
            defaultStartChannel(channel_model , topology_model)
            defaultChannelHeader(channel_model , topology_model)
            defaultChannelBody(channel_model , topology_model)


def main():
    """
    Main program.
    """
    
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag
    CONFIG = ConfigManager.ConfigManager.getInstance()
    
    #
    # Handle command line arguments
    #
    
    # Get the current working directory so that we can return to it at end
    STARTING_DIRECTORY = os.getcwd()
    
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
    # Create XML Parser and write COSMOS files for Topology
    #
    for xml_filename in xml_filenames:
        #
        # Create python dictionaries
        #
        
        xml_filename = os.path.basename(xml_filename)
        xml_type = XmlParser.XmlParser(xml_filename)()
        
        # Only Topologies can be inputted
        if xml_type == "assembly" or xml_type == "deployment":
            DEBUG.info("Detected Topology XML so Generating Topology C++ Files...")
            the_parsed_topology_xml = XmlTopologyParser.XmlTopologyParser(xml_filename)
            DEPLOYMENT = the_parsed_topology_xml.get_deployment()
            print("Found assembly or deployment named: %s\n" % DEPLOYMENT)
            generate_dictionaries(the_parsed_topology_xml, xml_filename, opt)
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
