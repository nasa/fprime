#!/bin/env python
#===============================================================================
# NAME: CosmosCommand.py
#
# DESCRIPTION: This class represents a command within COSMOS to conveniently
# store all the values necessary for the cheetah template to generate commands.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

# Contains all Cosmos utility methods and interface / protocol variable data that isnt autocoded
from fprime_ac.utils.cosmos.util import CosmosUtil

from fprime_ac.utils.cosmos.models import BaseCosmosObject

class CosmosCommand(BaseCosmosObject.BaseCosmosObject):
    """
    This class represents a command within COSMOS to conveniently store 
    all the values necessary for the cheetah template to generate command.
    """
    
    class CommandItem:
        """
        This class represents a COSMOS argument (item) attached to a command within 
        the COSMOS command config files.
        """
        def __init__(self, name, desc, bits, type, types, min_val, max_val, default):
            """
            @param name: Name of argument
            @param desc: Command description
            @param bits: Number of bits in argument
            @param type: COSMOS type for argument
            @param types: Contains enum data if is an enum
            @param min_val: Minimum value for argument
            @param max_val: Maximum value for argument
            @param default: Default value for argument
            """
            self.name = name.upper()
            self.desc = desc
            self.bits = bits
            self.type = type
            self.types = types
            self.min_val = min_val
            self.max_val = max_val
            self.default = default
            if type == 'STRING':
                self.default = '"' + self.default + '"'
            self.bit_offset = 0
    
    def __init__(self, name, opcode, comment):
        """
        @param name: Command name
        @param opcode: Opcode of command
        @param comment: Comment attached to command
        """
        super(CosmosCommand, self).__init__()
        self.opcode = opcode
        self.cmd_name = name.upper()
        self.cmd_desc = comment
        self.priority = 'N/A'
        self.sync = 'N/A'
        self.full = 'N/A'
        self.cmd_items = []
        
    def add_item(self, name, type, comment, enum, default_val=None):
        """
        Adds an item to the command packet
        @param name: Name of item
        @param type: Fprime version of argument data type (U16 as opposed to COSMOS's 16 UINT)
        @param comment: Description for item
        @param enum: Tuple with enum data
        @param default_val: Sets default value to something other than CosmosUtil default for data type
        """
        # Add an item to the command packet corresponding to the length of the following string item
        if type == 'string':
            len_size = CosmosUtil.get_bits_from_type(CosmosUtil.STRING_LEN_TYPE)
            len_item = self.CommandItem(name + "_length", "Length of String Arg", len_size, "UINT", [], \
                                        CosmosUtil.MIN_DICT[CosmosUtil.STRING_LEN_TYPE], \
                                        CosmosUtil.MAX_DICT[CosmosUtil.STRING_LEN_TYPE], \
                                        CosmosUtil.DEFAULT_DICT[CosmosUtil.STRING_LEN_TYPE])
            self.cmd_items.append(len_item)
        
        cosmos_type = CosmosUtil.TYPE_DICT[type]
        value_type = (cosmos_type[1] if not (cosmos_type[1] == "ENUM" or cosmos_type[1] == "BOOLEAN") else cosmos_type[2])
        bits = cosmos_type[0]
        
        # Handle enum
        cmd_enum_types = []
        count = 0
        if not enum == None:
            num = 0
            for item in enum[1]:
                if item[1] == None:
                    cmd_enum_types.append((item[0], num))
                else:
                    cmd_enum_types.append((item[0], int(item[1])))
                num += 1
        types = cmd_enum_types
        #
        # Add special FALSE/TRUE to "bool" types
        #
        if type == 'bool':
            types = []
            types.append(("FALSE",0))
            types.append(("TRUE",255))
        
        min_val = CosmosUtil.MIN_DICT[type]
        max_val = CosmosUtil.MAX_DICT[type]
        default = CosmosUtil.DEFAULT_DICT[type] if not default_val else default_val
        
        # Create item instance
        item = self.CommandItem(name, comment, bits, value_type, types, min_val, max_val, default)
        self.cmd_items.append(item)
                
    def set_xml_attributes(self, priority, sync, full):
        """
        Sets the Severity and Format String attributes from the XML
        @param priority: XML attribute "priority" found within XML source file
        @param sync: XML attribute "sync" found within XML source file
        @param full: XML attribute "full" found within XML source file
        """
        self.priority = priority
        self.sync = sync
        self.full = full
                
    def get_cmd_items(self):
        """
        List of items in packet
        """
        return self.cmd_items
    def get_opcode(self):
        """
        Opcode
        """
        return self.opcode
    def get_cmd_name(self):
        """
        Command name
        """
        return self.cmd_name
    def get_cmd_desc(self):
        """
        Command description
        """
        return self.cmd_desc
    def get_priority(self):
        """
        XML attribute "priority" found within XML source file
        """
        return self.priority
    def get_sync(self):
        """
        XML attribute "sync" found within XML source file
        """
        return self.sync
    def get_full(self):
        """
        XML attribute "full" found within XML source file
        """
        return self.full
