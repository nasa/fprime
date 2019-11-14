#!/bin/env python
#===============================================================================
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
#===============================================================================

import os
import sys
import logging

from optparse import OptionParser

from fprime_ac.models import ModelParser
from fprime_ac.utils import ConfigManager
from fprime_ac.utils import DictTypeConverter

# Meta-model for Component only generation
from fprime_ac.models import TopoFactory
from fprime_ac.parsers import XmlSerializeParser

# Parsers to read the XML
from fprime_ac.parsers import XmlParser
from fprime_ac.parsers import XmlTopologyParser

from lxml import etree

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
    program_longdesc = '''This script reads F' topology XML and produces GDS XML Dictionaries.'''
    
    parser = OptionParser(usage, version=vers, epilog=program_longdesc)
    
    parser.add_option("-b", "--build_root", dest="build_root_overwrite", type="string", help="Overwrite environment variable BUILD_ROOT", default=None)
        
    parser.add_option("-v", "--verbose", dest="verbose_flag", help="Enable verbose mode showing more runtime detail (def: False)", action="store_true", default=None)
                      
    return parser


def generate_xml_dict(the_parsed_topology_xml, xml_filename, opt):
    """
    Generates GDS XML dictionary from parsed topology XML
    """
    if VERBOSE:
        print("Topology xml type description file: %s" % xml_filename)
    model = TopoFactory.TopoFactory.getInstance()
    topology_model = model.create(the_parsed_topology_xml)

    generator = GenFactory.GenFactory.getInstance()
    
    #uses the topology model to process the items
    #create list of used parsed component xmls
    parsed_xml_dict = {}
    for comp in the_parsed_topology_xml.get_instances():
        if comp.get_type() in topology_model.get_base_id_dict():
            parsed_xml_dict[comp.get_type()] = comp.get_comp_xml()
        else:
            PRINT.info( "Components with type {} aren't in the topology model.".format(comp.get_type()))

    #
    xml_list = []
    for parsed_xml_type in parsed_xml_dict:
        if parsed_xml_dict[parsed_xml_type] == None:
            print("ERROR: XML of type {} is being used, but has not been parsed correctly. Check if file exists or add xml file with the 'import_component_type' tag to the Topology file.".format(parsed_xml_type))
            raise Exception()
        xml_list.append(parsed_xml_dict[parsed_xml_type])

    topology_model.set_instance_xml_list(xml_list)

    topology_dict = etree.Element("dictionary")
    topology_dict.attrib["topology"] = the_parsed_topology_xml.get_name()
    # create a new XML tree for dictionary
    enum_list = etree.Element("enums")
    serializable_list = etree.Element("serializables")
    command_list = etree.Element("commands")
    event_list = etree.Element("events")
    telemetry_list = etree.Element("channels")
    parameter_list = etree.Element("parameters")
    
    for comp in the_parsed_topology_xml.get_instances():
        comp_type = comp.get_type()
        comp_name = comp.get_name()
        comp_id = int(comp.get_base_id())
        PRINT.debug("Processing %s [%s] (%s)"%(comp_name,comp_type,hex(comp_id)))

        # check for included serializable XML
        if (parsed_xml_dict[comp_type].get_serializable_type_files() != None):
            serializable_file_list = parsed_xml_dict[comp_type].get_serializable_type_files()
            for serializable_file in serializable_file_list:
                serializable_file = search_for_file("Serializable", serializable_file)
                serializable_model = XmlSerializeParser.XmlSerializeParser(serializable_file)
                if (len(serializable_model.get_includes()) != 0):
                    raise Exception("%s: Can only include one level of serializable for dictionaries"%serializable_file)
                serializable_elem = etree.Element("serializable")
                serializable_type = serializable_model.get_namespace() + "::" + serializable_model.get_name()
                serializable_elem.attrib["type"] = serializable_type
                members_elem = etree.Element("members")
                for (member_name, member_type, member_size, member_format_specifier, member_comment) in serializable_model.get_members():
                    member_elem = etree.Element("member")
                    member_elem.attrib["name"] = member_name
                    member_elem.attrib["format_specifier"] = member_format_specifier
                    if member_comment != None:
                        member_elem.attrib["description"] = member_comment
                    if type(member_type) == type(tuple()):
                        enum_value = 0
                        type_name = "%s::%s::%s"%(serializable_type,member_name,member_type[0][1])
                        # Add enum entry
                        enum_elem = etree.Element("enum")
                        enum_elem.attrib["type"] = type_name
                        # Add enum members
                        for (membername,value,comment) in member_type[1]:
                            enum_mem = etree.Element("item")
                            enum_mem.attrib["name"] = membername
                            # keep track of incrementing enum value
                            if value != None:
                                enum_value = int(value)

                            enum_mem.attrib["value"] = "%d"%enum_value
                            enum_value = enum_value + 1
                            if comment != None:
                                enum_mem.attrib["description"] = comment
                            enum_elem.append(enum_mem)
                        enum_list.append(enum_elem)
                    else:
                        type_name = member_type
                        if member_type == "string":
                            member_elem.attrib["len"] = member.get_size()
                    member_elem.attrib["type"] = type_name
                    members_elem.append(member_elem)
                serializable_elem.append(members_elem)
                
                dup = False
                for ser in serializable_list:
                    if ser.attrib["type"] == serializable_elem.attrib["type"]:
                        dup = True
                if not dup:
                    serializable_list.append(serializable_elem)


        # check for commands
        if (parsed_xml_dict[comp_type].get_commands() != None):
            for command in parsed_xml_dict[comp_type].get_commands():
                if VERBOSE:
                    print("Processing Command %s"%command.get_mnemonic())
                command_elem = etree.Element("command")
                command_elem.attrib["component"] = comp_name
                command_elem.attrib["mnemonic"] = command.get_mnemonic()
                command_elem.attrib["opcode"] = "%s"%(hex(int(command.get_opcodes()[0],base=0) + comp_id))
                if ("comment" in list(command_elem.attrib.keys())):
                    command_elem.attrib["description"] = command_elem.attrib["comment"]
                args_elem = etree.Element("args")
                for arg in command.get_args():
                    arg_elem = etree.Element("arg")
                    arg_elem.attrib["name"] = arg.get_name()
                    arg_type = arg.get_type()
                    if type(arg_type) == type(tuple()):
                        enum_value = 0
                        type_name = "%s::%s::%s"%(comp_type,arg.get_name(),arg_type[0][1])
                        # Add enum entry
                        enum_elem = etree.Element("enum")
                        enum_elem.attrib["type"] = type_name
                        # Add enum members
                        for (membername,value,comment) in arg_type[1]:
                            enum_mem = etree.Element("item")
                            enum_mem.attrib["name"] = membername
                            # keep track of incrementing enum value
                            if value != None:
                                enum_value = int(value)

                            enum_mem.attrib["value"] = "%d"%enum_value
                            enum_value = enum_value + 1
                            if comment != None:
                                enum_mem.attrib["description"] = comment
                            enum_elem.append(enum_mem)
                        enum_list.append(enum_elem)
                    else:
                        type_name = arg_type
                        if arg_type == "string":
                            arg_elem.attrib["len"] = arg.get_size()
                    arg_elem.attrib["type"] = type_name
                    args_elem.append(arg_elem)
                command_elem.append(args_elem)
                command_list.append(command_elem)

        # check for channels
        if (parsed_xml_dict[comp_type].get_channels() != None):
            for chan in parsed_xml_dict[comp_type].get_channels():
                if VERBOSE:
                    print("Processing Channel %s"%chan.get_name())
                channel_elem = etree.Element("channel")
                channel_elem.attrib["component"] = comp_name
                channel_elem.attrib["name"] = chan.get_name()
                channel_elem.attrib["id"] = "%s"%(hex(int(chan.get_ids()[0],base=0) + comp_id))
                if chan.get_format_string() != None:
                    channel_elem.attrib["format_string"] = chan.get_format_string()
                if chan.get_comment() != None:
                    channel_elem.attrib["description"] = chan.get_comment()
            
                channel_elem.attrib["id"] = "%s"%(hex(int(chan.get_ids()[0],base=0) + comp_id))
                if ("comment" in list(channel_elem.attrib.keys())):
                    channel_elem.attrib["description"] = channel_elem.attrib["comment"]
                channel_type = chan.get_type()
                if type(channel_type) == type(tuple()):
                    enum_value = 0
                    type_name = "%s::%s::%s"%(comp_type,chan.get_name(),channel_type[0][1])
                    # Add enum entry
                    enum_elem = etree.Element("enum")
                    enum_elem.attrib["type"] = type_name
                    # Add enum members
                    for (membername,value,comment) in channel_type[1]:
                        enum_mem = etree.Element("item")
                        enum_mem.attrib["name"] = membername
                        # keep track of incrementing enum value
                        if value != None:
                            enum_value = int(value)
                            
                        enum_mem.attrib["value"] = "%d"%enum_value
                        enum_value = enum_value + 1
                        if comment != None:
                            enum_mem.attrib["description"] = comment
                        enum_elem.append(enum_mem)
                    enum_list.append(enum_elem)
                else:
                    type_name = channel_type
                    if channel_type == "string":
                        channel_elem.attrib["len"] = chan.get_size()
                (lr,lo,ly,hy,ho,hr) = chan.get_limits()
                if (lr != None):
                    channel_elem.attrib["low_red"] = lr
                if (lo != None):
                    channel_elem.attrib["low_orange"] = lo
                if (ly != None):
                    channel_elem.attrib["low_yellow"] = ly
                if (hy != None):
                    channel_elem.attrib["high_yellow"] = hy
                if (ho != None):
                    channel_elem.attrib["high_orange"] = ho
                if (hr != None):
                    channel_elem.attrib["hight_red"] = hr
                    
                channel_elem.attrib["type"] = type_name
                telemetry_list.append(channel_elem)
                    
        # check for events
        if (parsed_xml_dict[comp_type].get_events() != None):
            for event in parsed_xml_dict[comp_type].get_events():
                if VERBOSE:
                    print("Processing Event %s"%event.get_name())
                event_elem = etree.Element("event")
                event_elem.attrib["component"] = comp_name
                event_elem.attrib["name"] = event.get_name()
                event_elem.attrib["id"] = "%s"%(hex(int(event.get_ids()[0],base=0) + comp_id))
                event_elem.attrib["severity"] = event.get_severity()
                format_string = event.get_format_string()
                if ("comment" in list(event_elem.attrib.keys())):
                    event_elem.attrib["description"] = event_elem.attrib["comment"]
                args_elem = etree.Element("args")
                arg_num = 0
                for arg in event.get_args():
                    arg_elem = etree.Element("arg")
                    arg_elem.attrib["name"] = arg.get_name()
                    arg_type = arg.get_type()
                    if type(arg_type) == type(tuple()):
                        enum_value = 0
                        type_name = "%s::%s::%s"%(comp_type,arg.get_name(),arg_type[0][1])
                        # Add enum entry
                        enum_elem = etree.Element("enum")
                        enum_elem.attrib["type"] = type_name
                        # Add enum members
                        for (membername,value,comment) in arg_type[1]:
                            enum_mem = etree.Element("item")
                            enum_mem.attrib["name"] = membername
                            # keep track of incrementing enum value
                            if value != None:
                                enum_value = int(value)
                        
                            enum_mem.attrib["value"] = "%d"%enum_value
                            enum_value = enum_value + 1
                            if comment != None:
                                enum_mem.attrib["description"] = comment
                            enum_elem.append(enum_mem)
                        enum_list.append(enum_elem)
                        # replace enum format string %d with %s for ground system
                        format_string = DictTypeConverter.DictTypeConverter().format_replace(format_string,arg_num,'d','s')
                    else:
                        type_name = arg_type
                        if arg_type == "string":
                            arg_elem.attrib["len"] = arg.get_size()
                    arg_elem.attrib["type"] = type_name
                    args_elem.append(arg_elem)
                    arg_num += 1
                event_elem.attrib["format_string"] = format_string
                event_elem.append(args_elem)
                event_list.append(event_elem)

        # check for parameters
        if (parsed_xml_dict[comp_type].get_parameters() != None):
            for parameter in parsed_xml_dict[comp_type].get_parameters():
                if VERBOSE:
                    print("Processing Parameter %s"%chan.get_name())
                param_default = None
                command_elem_set = etree.Element("command")
                command_elem_set.attrib["component"] = comp_name
                command_elem_set.attrib["mnemonic"] = parameter.get_name()+ "_PRM_SET"
                command_elem_set.attrib["opcode"] = "%s"%(hex(int(parameter.get_set_opcodes()[0],base=0) + comp_id))
                if ("comment" in list(command_elem.attrib.keys())):
                    command_elem_set.attrib["description"] = command_elem_set.attrib["comment"] + " parameter set"
                else:
                    command_elem_set.attrib["description"] = parameter.get_name() + " parameter set"

                args_elem = etree.Element("args")
                arg_elem = etree.Element("arg")
                arg_elem.attrib["name"] = "val"
                arg_type = parameter.get_type()
                if type(arg_type) == type(tuple()):
                    enum_value = 0
                    type_name = "%s::%s::%s"%(comp_type,arg.get_name(),arg_type[0][1])
                    # Add enum entry
                    enum_elem = etree.Element("enum")
                    enum_elem.attrib["type"] = type_name
                    # Add enum members
                    for (membername,value,comment) in arg_type[1]:
                        enum_mem = etree.Element("item")
                        enum_mem.attrib["name"] = membername
                        # keep track of incrementing enum value
                        if value != None:
                            enum_value = int(value)
                    
                        enum_mem.attrib["value"] = "%d"%enum_value
                        enum_value = enum_value + 1
                        if comment != None:
                            enum_mem.attrib["description"] = comment
                        enum_elem.append(enum_mem)
                        # assign default to be first enum member
                        if param_default == None:
                            param_default = membername
                    enum_list.append(enum_elem)
                else:
                    type_name = arg_type
                    if arg_type == "string":
                        arg_elem.attrib["len"] = arg.get_size()
                    else:
                        param_default = "0"
                arg_elem.attrib["type"] = type_name
                args_elem.append(arg_elem)
                command_elem_set.append(args_elem)
                command_list.append(command_elem_set)

                command_elem_save = etree.Element("command")
                command_elem_save.attrib["component"] = comp_name
                command_elem_save.attrib["mnemonic"] = parameter.get_name()+ "_PRM_SAVE"
                command_elem_save.attrib["opcode"] = "%s"%(hex(int(parameter.get_save_opcodes()[0],base=0) + comp_id))
                if ("comment" in list(command_elem.attrib.keys())):
                    command_elem_save.attrib["description"] = command_elem_set.attrib["comment"] + " parameter set"
                else:
                    command_elem_save.attrib["description"] = parameter.get_name() +  " parameter save"

                command_list.append(command_elem_save)

                param_elem = etree.Element("parameter")
                param_elem.attrib["component"] = comp_name
                param_elem.attrib["name"] = parameter.get_name()
                param_elem.attrib["id"] = "%s"%(hex(int(parameter.get_ids()[0],base=0) + comp_id))
                if parameter.get_default() != None:
                    param_default = parameter.get_default()
                param_elem.attrib["default"] = param_default

                parameter_list.append(param_elem)
    
    # Remove duplicates from enum list
    temp_enum_list = []
    for enum_elem in enum_list:
        temp_enum_list.append(enum_elem)
    for enum_elem in temp_enum_list:
        should_remove = False
        for temp_enum in enum_list:
            # Skip over comparisons between same exact element
            if(id(enum_elem) == id(temp_enum)):
                continue

            # Check all attributes
            if temp_enum.attrib["type"] == enum_elem.attrib["type"]:
                should_remove = True
            if not len(temp_enum.getchildren()) == len(enum_elem.getchildren()) and should_remove:
                should_remove = False
            children1 = temp_enum.getchildren()
            children2 = enum_elem.getchildren()
            if children1 and children2:
                i = 0
                while i < len(children1) and i < len(children2):
                    if not children1[i].attrib["name"] == children2[i].attrib["name"] and should_remove:
                        should_remove = False
                    i += 1
            if should_remove:
                break
        if should_remove:
            enum_list.remove(enum_elem)

    topology_dict.append(enum_list)
    topology_dict.append(serializable_list)
    topology_dict.append(command_list)
    topology_dict.append(event_list)
    topology_dict.append(telemetry_list)
    topology_dict.append(parameter_list)
    
    fileName = the_parsed_topology_xml.get_xml_filename().replace("Ai.xml","Dictionary.xml")
    if VERBOSE:
        print("Generating XML dictionary %s"%fileName)
    fd = open(fileName,"wb") #Note: binary forces the same encoding of the source files
    fd.write(etree.tostring(topology_dict,pretty_print=True))
    if VERBOSE:
        print("Generated XML dictionary %s"%fileName)
    
    return(topology_model)

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
        if VERBOSE:
            print("ERROR: %s xml specification file %s does not exist!" % (file_type,file_path))
        sys.exit(-1)

def main():
    """
    Main program.
    """
    global VERBOSE # prevent local creation of variable
    global BUILD_ROOT # environmental variable if set
    global DEPLOYMENT # deployment set in topology xml only and used to install new instance dicts
    
    CONFIG = ConfigManager.ConfigManager.getInstance()
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


if __name__ == '__main__':
    main()
