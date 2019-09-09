#!/bin/env python
#===============================================================================
# NAME: pymod_dictgen.py
#
# DESCRIPTION: A tool for autocoding topology XML files into GDS python dictionaries.
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
import logging

from optparse import OptionParser
from fprime_ac.models import ModelParser
from fprime_ac.utils import ConfigManager

# Meta-model for Component only generation
from fprime_ac.models import TopoFactory
from fprime_ac.models import CompFactory
from fprime_ac.parsers import XmlSerializeParser

# Parsers to read the XML
from fprime_ac.parsers import XmlParser
from fprime_ac.parsers import XmlTopologyParser
from fprime_ac.parsers import XmlPortsParser

from fprime_ac.generators.writers import InstCommandWriter
from fprime_ac.generators.writers import InstChannelWriter
from fprime_ac.generators.writers import InstEventWriter

#Generators to produce the code
from fprime_ac.generators import GenFactory

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
    
    current_dir = os.getcwd()

    usage = "usage: %prog [options] [xml_topology_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = '''This script reads F' topology XML and produces GDS Python Dictionaries. They are generated into your build dir under a py_dict directory'''

    parser = OptionParser(usage, version=vers, epilog=program_longdesc)

    parser.add_option("-b", "--build_root", dest="build_root_overwrite", type="string", help="Enable search for enviornment variable BUILD_ROOT to establish absolute XML directory path", default=None)
        
    parser.add_option("-o", "--dict_dir", dest="dict_dir", help="Output directory for dictionary. Needed for -g.", default=None)

    parser.add_option("-v", "--verbose", dest="verbose_flag", help="Enable verbose mode showing more runtime detail (def: False)", action="store_true", default=False)
                      
    return parser

def generate_pymods(the_parsed_topology_xml, xml_filename, opt):
    """
    Top level python module generation method
    """
    if the_parsed_topology_xml.get_namespace():
        if VERBOSE:
            print("Generating pymods for topology %s::%s"%(the_parsed_topology_xml.get_namespace(), the_parsed_topology_xml.get_name()))
    else:
        if VERBOSE:
            print("Generating pymods for topology %s"%(the_parsed_topology_xml.get_name()))
    model = TopoFactory.TopoFactory.getInstance()
    topology_model = model.create(the_parsed_topology_xml)
    
    #create list of used parsed component xmls
    parsed_xml_dict = {}
    for comp in the_parsed_topology_xml.get_instances():
        if comp.get_type() in topology_model.get_base_id_dict():
            parsed_xml_dict[comp.get_type()] = comp.get_comp_xml()
        else:
            if VERBOSE:
                print("Components with type {} aren't in the topology model.".format(comp.get_type()))

    #
    # Hack to set up deployment path for instanced dictionaries (if one exists remove old one)
    #
    if opt.dict_dir == None:
        os.environ["DICT_DIR"] = os.getcwd()
    else:
        os.environ["DICT_DIR"] = opt.dict_dir

    xml_list = []
    for parsed_xml_type in parsed_xml_dict:
        if parsed_xml_dict[parsed_xml_type] == None:
            print("ERROR: XML of type {} is being used, but has not been parsed correctly. Check if file exists or add xml file with the 'import_component_type' tag to the Topology file.".format(parsed_xml_type))
            raise Exception()
        xml_list.append(parsed_xml_dict[parsed_xml_type])
        temp_comp = parsed_xml_dict[parsed_xml_type].get_component()
        if VERBOSE:
            print("Generating component dicts for %s::%s"%(temp_comp.get_namespace(), temp_comp.get_name()))
        write_pymods_from_comp(parsed_xml_dict[parsed_xml_type] , opt , topology_model)
        if VERBOSE:
            print("Generated component dicts for %s::%s"%(temp_comp.get_namespace(), temp_comp.get_name()))

    topology_model.set_instance_xml_list(xml_list)

    if the_parsed_topology_xml.get_namespace():
        if VERBOSE:
            print("Generated pymods for topology %s::%s"%(the_parsed_topology_xml.get_namespace(), the_parsed_topology_xml.get_name()))
    else:
        if VERBOSE:
            print("Generated pymods for topology %s"%(the_parsed_topology_xml.get_name()))



def write_pymods_from_comp(the_parsed_component_xml , opt , topology_model):
    """
    Writes python modules for a component xml
    "the_parsed_component_xml"
    """
    global BUILD_ROOT
    global DEPLOYMENT
    global VERBOSE
    
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
        parsed_port_xml_list.append(xml_parser_obj)
        del(xml_parser_obj)

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

    instChannelWriter = InstChannelWriter.InstChannelWriter()
    instCommandWriter = InstCommandWriter.InstCommandWriter()
    instEventWriter = InstEventWriter.InstEventWriter()

    if opt.dict_dir == None:
        if VERBOSE:
            print("Dictionary output directory not specified!, defaulting to cwd")
        opt.dict_dir = os.getcwd()
    os.environ["DICT_DIR"] = opt.dict_dir
    default_dict_generator = GenFactory.GenFactory.getInstance()
    # iterate through command instances
    for command_model in component_model.get_commands():
        if VERBOSE:
            print("Generating command dict %s"%command_model.get_mnemonic())
        instCommandWriter.DictStartWrite(command_model , topology_model)
        instCommandWriter.DictHeaderWrite(command_model , topology_model)
        instCommandWriter.DictBodyWrite(command_model , topology_model)
    
    for parameter_model in component_model.get_parameters():
        if VERBOSE:
            print("Generating parameter dict %s"%parameter_model.get_name())
        instCommandWriter.DictStartWrite(parameter_model , topology_model)
        instCommandWriter.DictHeaderWrite(parameter_model , topology_model)
        instCommandWriter.DictBodyWrite(parameter_model , topology_model)
        
    default_dict_generator = GenFactory.GenFactory.getInstance()
    # iterate through command instances
    for event_model in component_model.get_events():
        if VERBOSE:
            print("Generating event dict %s"%event_model.get_name())
        instEventWriter.DictStartWrite(event_model , topology_model)
        instEventWriter.DictHeaderWrite(event_model , topology_model)
        instEventWriter.DictBodyWrite(event_model , topology_model)

    default_dict_generator = GenFactory.GenFactory.getInstance()
    # iterate through command instances
    for channel_model in component_model.get_channels():
        if VERBOSE:
            print("Generating channel dict %s"%channel_model.get_name())
        instChannelWriter.DictStartWrite(channel_model , topology_model)
        instChannelWriter.DictHeaderWrite(channel_model , topology_model)
        instChannelWriter.DictBodyWrite(channel_model , topology_model)

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
            if VERBOSE:
                print("%s xml type description file: %s" % (file_type,file_path))
            return checker
    else:
        print("ERROR: %s xml specification file %s does not exist!" % (file_type,file_path))
        sys.exit(-1)



def main():
    """
    Main program.
    """
    global VERBOSE
    global DEPLOYMENT
    
    Parser = pinit()
    (opt, args) = Parser.parse_args()
    VERBOSE = opt.verbose_flag
    CONFIG = ConfigManager.ConfigManager.getInstance()
    
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

    if not "Ai" in xml_filename:
        print("ERROR: Missing Ai at end of file name...")
        raise IOError
    #
    # Create python dictionaries
    #
        
    xml_type = XmlParser.XmlParser(xml_filename)()
    
    # Only Topologies can be inputted
    if xml_type == "assembly" or xml_type == "deployment":
        if VERBOSE:
            print("Detected Topology XML so Generating Topology C++ Files...")
        the_parsed_topology_xml = XmlTopologyParser.XmlTopologyParser(xml_filename)
        DEPLOYMENT = the_parsed_topology_xml.get_deployment()
        if VERBOSE:
            print("Found assembly or deployment named: %s\n" % DEPLOYMENT)
        generate_pymods(the_parsed_topology_xml, xml_filename, opt)
    else:
        print("ERROR: Invalid XML found...this format not supported")
        sys.exit(-1)

    sys.exit(0)

if __name__ == '__main__':
    main()
