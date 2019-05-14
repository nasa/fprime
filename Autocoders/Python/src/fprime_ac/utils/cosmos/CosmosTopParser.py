#!/bin/env python
#===============================================================================
# NAME: CosmosTopParser.py
#
# DESCRIPTION: This class parses a topology file within its parse_topology()
# method and saves the channels, events, and commands to list-instance-variables.
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
import glob

# Parsers to read the XML
from parsers import XmlParser
from parsers import XmlTopologyParser
from parsers import XmlEnumParser

from utils.cosmos.models import CosmosCommand
from utils.cosmos.models import CosmosChannel
from utils.cosmos.models import CosmosEvent

from utils.cosmos.util import CosmosUtil
from utils.cosmos.util import CheetahUtil
from docutils.parsers.rst.states import build_regexp

class CosmosTopParser():
    """
    This class takes an XML topology parser that has parsed the topology XML files
    and instantiates CosmosChannels, CosmosEvents, and CosmosCommands from the data
    in the topology parser.
    """
    
    def __init__(self, build_root):
        """
        Init
        """
        self.channels = []
        self.events = []
        self.commands = []
        self.deployment = None
        self.build_root = build_root
        
        
    def isSimpleType(self,t):
        """
        Returns True if this is type of F[32|64],U[8|16|32|64],I[8|16|32|64],string, or ENUM,
        otherwise return False
        @param: t is the name of the type of argument
        """
        if type(t) is type(tuple()):
            t = t[0][0]
    
        if (("F32" == t) or
            ("F64" == t) or
            ("U8"   == t) or
            ("U16"  == t) or 
            ("U32"  == t) or 
            ("U64"  == t) or 
            ("I8"   == t) or 
            ("I16"  == t) or 
            ("I32"  == t) or 
            ("I64"  == t) or
            ("bool" == t) or
            ("string" == t) or
            ("ENUM" == t)):
            return True
        else:
            return False

    def enumFindAndParse(self, enum_name, search_top_dir):
        """
        Returns a list of tuple's of (identifier, value, comment) if enum of enum_name is found,
        otherwise returns None object.
        
        Only searches under the search_top_dir directory!!  These are under BUILD_ROOT.
        """
        enum_list = []
        #
        # Iterate over component collections
        #
        os.chdir(self.build_root)
        #
        # Only search DiebApps for now!
        #
        for collection_dir in glob.glob(search_top_dir):
            if os.path.isdir(collection_dir):
                bdir = self.build_root + os.sep + collection_dir
                os.chdir(bdir)
                for component_dir in glob.glob('*'):
                    if os.path.isdir(component_dir):
                        dir = bdir + os.sep + component_dir
                        os.chdir(dir)
                        enum_files = glob.glob("*EnumAi.txt")
                        if len(enum_files) > 0:
                            for efile in enum_files:
                                if os.path.isfile(efile):
                                    if enum_name == efile.split('EnumAi.txt')[0]:
                                        lines = open(efile, 'r').readlines()[5:]
                                        lines = filter(lambda x: x!='\n', lines)
                                        for l in range(0,len(lines),3):
                                            l1 = lines[l].strip('\n')
                                            l2 = lines[l+1].strip('\n')
                                            l3 = lines[l+2].strip('\n')
                                            enum_list.append((l1,l2,l3))
                                        return enum_list
                        os.chdir('..')
                os.chdir(self.build_root)
        return enum_list


    def enumFindXmlAndParse(self, enum_name, search_top_dir):
        """
        Returns a list of tuple's of (identifier, value, comment) if enum of enum_name is found,
        otherwise returns None object.
        
        Only searches under the search_top_dir directory!!  These are under BUILD_ROOT.
        """
        enum_list = []
        #
        # Iterate over component collections
        #
        os.chdir(self.build_root)
        #
        # Only search DiebApps for now!
        #
        for collection_dir in glob.glob(search_top_dir):
            if os.path.isdir(collection_dir):
                bdir = self.build_root + os.sep + collection_dir
                os.chdir(bdir)
                for component_dir in glob.glob('*'):
                    if os.path.isdir(component_dir):
                        dir = bdir + os.sep + component_dir
                        os.chdir(dir)
                        enum_files = glob.glob("*EnumAi.xml")
                        if len(enum_files) > 0:
                            for efile in enum_files:
                                if os.path.isfile(efile):
                                    xml = XmlEnumParser.XmlEnumParser(efile)
                                    enum_list = xml.get_items()
                        os.chdir('..')
                os.chdir(self.build_root)
        return enum_list

                
    def parse_topology(self, xml_filename, overwrite = True):
        """
        Takes an XML File and puts all channel, event, and command
        data into CosmosChannel, CosmosEvent, and CosmosCommand model class instances
        to be passed to the Generator that creates the config files
        @param xml_filename: XML File Name, should be Topology if not quits
        @param overwrite: Flag whether to overwrite channels, events, and commands lists
        """
        bot_dir = os.getcwd()
        os.chdir(CosmosUtil.STARTING_DIRECTORY)    # Parser needs to be in Autocoders/bin directory to be able to find Topology XML
         
        print "\nUsing XmlParser and XmlTopologyParser instances"
             
        xml_type = XmlParser.XmlParser(xml_filename)()
 
        if xml_type == "assembly" or xml_type == "deployment":
             
            if CosmosUtil.VERBOSE:
                print ("Detected ISF Topology XML Files...")
            topology = XmlTopologyParser.XmlTopologyParser(xml_filename)
 
            # Name of COSMOS target to be created
            self.deployment = topology.get_deployment()
             
            if CosmosUtil.VERBOSE:
                print "\nFound assembly or deployment named: " + self.deployment + "\n"
        else:
            print "ERROR: XML File Not a Topology File"
            sys.exit(-1)
             
        # Change back
        os.chdir(bot_dir)
         
        print "Finished Reusing XmlParser and XmlTopologyParser instances\n"
         
        if overwrite:
            self.channels = []
            self.events = []
            self.commands = []
         
         
        print "Parsing Topology"
        print "Found %s components.\n" % len(topology.get_instances())
        for inst in topology.get_instances():
            comp_name = inst.get_name()
            comp_type = inst.get_type()
            base_id = inst.get_base_id()
                            
            #print comp_name, comp_type, base_id
            #
            # If base_id is not set for a component assume it has nothing
            # and skip it.
            if base_id is None:
                print "Skipping %s:%s component - has not commands or telemetry" % (comp_name, comp_type)
                continue
                         
            if '0x' in base_id:
                base_id = int(base_id, 16)
            else:
                base_id = int(base_id)
            comp_parser = inst.get_comp_xml()
            #
            # Parse command data here...
            #
            if 'get_commands' in dir(comp_parser):
                if CosmosUtil.VERBOSE:
                    print "Parsing Commands for instance: " + comp_name
                cmds = comp_parser.get_commands()
                for cmd in cmds:
                    opcode = cmd.get_opcodes()[0]
                    if '0x' in opcode:
                        opcode = int(opcode, 16)
                    else:
                        opcode = int(opcode)
                    opcode += base_id
                    n = cmd.get_mnemonic()
                    c = cmd.get_comment()
                    p = cmd.get_priority()
                    s = cmd.get_sync()
                    f = cmd.get_full()
                    source = comp_parser.get_xml_filename()
                    cosmos_cmd = CosmosCommand.CosmosCommand(n, opcode, c)
                    cosmos_cmd.set_component_attributes(comp_name, comp_type, source)
                    cosmos_cmd.set_xml_attributes(p, s, f)
                     
                    # Count strings to see if 2 (if so needs block argument)
                    string_count = 0
                    args = cmd.get_args()
                    for arg in args:
                        t = arg.get_type()
                        if t == 'string':
                            string_count += 1
                     
                    is_multi_string_command = False
                    if string_count >= 2:
                        is_multi_string_command = True
                    #
                    # Parse command arg data here...
                    #
                    num = 0
                     
                    if CosmosUtil.VERBOSE:
                        print "Command " + n + " Found"
 
                    enum = None
                    for arg in args:
                        n = arg.get_name()
                        t = arg.get_type()
                        c = arg.get_comment()
                        #
                        # Handle enum case where it is defined in ext. *EnumAi.txt or *EnumAi.xml files
                        #
                        if not self.isSimpleType(t):
                            l = self.enumFindAndParse(t, "DiebApps")
                            if len(l) == 0:
                                l = self.enumFindXmlAndParse(t, "DiebApps")
                            t = (('ENUM', t), l)
                        #
                        # Parse command enum here
                        #
                        if type(t) is type(tuple()):
                            enum = t
                            t = t[0][0]
                        else:
                            enum = None

                        num += 1
                        
                        if not is_multi_string_command:        
                            cosmos_cmd.add_item(n, t, c, enum)
                     
                    if is_multi_string_command:     
                        if CosmosUtil.VERBOSE:
                            print "Multi-string commands not supported in COSMOS at: " + cmd.get_mnemonic() + " from " + source
                        else:
                            print "Multi-string command " + cmd.get_mnemonic() + " not supported"
                    else:
                        self.commands.append(cosmos_cmd)
                 
                if CosmosUtil.VERBOSE:
                    print "Finished Parsing Commands for " + comp_name     
            #
            # Parse parameter data here...
            #        
            if 'get_parameters' in dir(comp_parser): 
                for prm in comp_parser.get_parameters():
                    enum = None
                    n = prm.get_name()
                    s = prm.get_size()
                    t = prm.get_type()
                    c = prm.get_comment()
                    d = prm.get_default()
                    source = comp_parser.get_xml_filename()
                     
                    # Parse param enum
                    if type(t) is type(tuple()):
                        enum = t
                        t = t[0][0]
                    num += 1
                     
                    # Calculate opcodes
                    set_opcode = prm.get_set_opcodes()[0]
                    if '0x' in set_opcode:
                        set_opcode = int(set_opcode, 16)
                    else:
                        set_opcode = int(set_opcode)
                    set_opcode += base_id
                    save_opcode = prm.get_save_opcodes()[0]
                    if '0x' in save_opcode:
                        save_opcode = int(save_opcode, 16)
                    else:
                        save_opcode = int(save_opcode)
                    save_opcode += base_id
                     
                    # Create models
                    cosmos_prm_set = CosmosCommand.CosmosCommand((n + "_prm_set"), set_opcode, c)
                    cosmos_prm_save = CosmosCommand.CosmosCommand((n + "_prm_save"), save_opcode, c)
                     
                    cosmos_prm_set.set_component_attributes(comp_name, comp_type, source)
                    cosmos_prm_save.set_component_attributes(comp_name, comp_type, source)
                     
                    # Add single arguments
                    cosmos_prm_set.add_item(n, t, c, enum, d)
                     
                    self.commands.append(cosmos_prm_set)
                    self.commands.append(cosmos_prm_save)
            #
            # Parse event data here...
            #
            if "get_events" in dir(comp_parser):
                if CosmosUtil.VERBOSE:
                    print "Parsing Events for " + comp_name
                evrs = comp_parser.get_events()
                for evr in evrs:
                    evr_id =evr.get_ids()[0]
                    if '0x' in evr_id:
                        evr_id = int(evr_id, 16)
                    else:
                        evr_id = int(evr_id)
                    evr_id += base_id
                    n = evr.get_name()
                    comment = evr.get_comment()
                    s = evr.get_severity()
                    f = evr.get_format_string()
                    source = comp_parser.get_xml_filename()
                    cosmos_evr = CosmosEvent.CosmosEvent(n, evr_id, comment)
                    cosmos_evr.set_component_attributes(comp_name, comp_type, source)
                    cosmos_evr.set_xml_attributes(s, f)
                         
                    if CosmosUtil.VERBOSE:
                        print "Event " + n + " Found"
                    #
                    # Parse event enums here...
                    #
                    bit_count = 0
                    args = evr.get_args()
                    for arg in args:
                        n = arg.get_name()
                        t = arg.get_type()
                        s = arg.get_size()
                        c = arg.get_comment()
                        #
                        # Handle enum case where it is define in ext. *EnumAi.txt or *EnumAi.xml files
                        #
                        if not self.isSimpleType(t):
                            l = self.enumFindAndParse(t, "DiebApps")
                            if len(l) == 0:
                                l = self.enumFindXmlAndParse(t, "DiebApps")
                            t = (('ENUM', t), l)
                        
                        enum = None
                        if type(t) is type(tuple()):
                            enum = t
                            t = t[0][0]
                         
                        cosmos_evr.add_item(n, t, c, enum)
                    self.events.append(cosmos_evr)
                if CosmosUtil.VERBOSE:
                    print "Finished Parsing Events for " + comp_name
            #
            # Parse channel data here...
            #
            if "get_channels" in dir(comp_parser):
                if CosmosUtil.VERBOSE:
                    print "Parsing Channels for " + comp_name
                channels = comp_parser.get_channels()
                for ch in channels:
                    ch_id = ch.get_ids()[0]
                    if '0x' in ch_id:
                        ch_id = int(ch_id, 16)
                    else:
                        ch_id = int(ch_id)
                    ch_id += base_id
                    n = ch.get_name()
                    t = ch.get_type()
                    enum = None
                    if type(t) is type(tuple()):
                        enum = t
                        t = t[0][0]
                    c = ch.get_comment()
                    limits = ch.get_limits()
                    source = comp_parser.get_xml_filename()
                    cosmos_ch = CosmosChannel.CosmosChannel(n, ch_id, c)
                    cosmos_ch.set_component_attributes(comp_name, comp_type, source)
                    cosmos_ch.set_item(t, enum, ch.get_format_string())
                    cosmos_ch.set_limits(limits)
                     
                    if CosmosUtil.VERBOSE:
                        print "Found channel " + n + " with argument type: " + t
                     
                    self.channels.append(cosmos_ch)
                if CosmosUtil.VERBOSE:
                    print "Finished Parsing Channels for " + comp_name
        
        # Check command and EVR packets to see if they should apply a negative offset
        # NO CMD OR TLM PACKETS SHOULD BE ADDED ONCE THIS CHECK IS DONE
        for evr in self.events:
            CheetahUtil.evr_update_variable_lengths(evr)
        for cmd in self.commands:
            CheetahUtil.cmd_update_variable_lengths(cmd)
                    
        print "Parsed Topology\n"
        
        
    def get_channels(self):
        """
        List of all channels
        """
        return self.channels
    
    def get_events(self):
        """
        List of all channels
        """
        return self.events
    
    def get_commands(self):
        """
        List of all channels
        """
        return self.commands
    
    def get_deployment(self):
        """
        Name of Topology deployment
        """
        return self.deployment