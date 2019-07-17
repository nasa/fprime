#!/bin/env python
#===============================================================================
# NAME: CosmosEvent.py
#
# DESCRIPTION: This class represents an event within COSMOS to conveniently
# store all the values necessary for the cheetah template to generate events.
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
from fprime_ac.utils.cosmos.util import CheetahUtil

from fprime_ac.utils.cosmos.models import BaseCosmosObject

class CosmosEvent(BaseCosmosObject.BaseCosmosObject):
    """
    This class represents an event within COSMOS to conveniently store 
    all the values necessary for the cheetah template to generate command.
    """
    
    class EventItem:
        """
        This class represents a COSMOS argument (item) attached to an event within 
        the COSMOS event config files.
        """
        def __init__(self, name, desc, bits, type, types):
            """
            @param name: Name of argument
            @param desc: Command description
            @param bits: Number of bits in argument
            @param type: COSMOS type for argument
            @param types: Contains enum data if is an enum
            """
            self.name = name.upper()
            self.desc = desc
            self.bits = bits
            self.type = type
            self.types = types
            self.bit_offset = 0
            self.template_string = ""
    
    def __init__(self, name, evr_id, comment):
        """
        @param name: Event name
        @param evr_id: Event ID
        @param comment: Event description
        """
        super(CosmosEvent, self).__init__()
        self.id = evr_id
        self.evr_name = name.upper()
        self.evr_desc = comment
        self.evr_items = []
        self.names = []
        self.non_len_names = []
        self.severity = 'N/A'
        self.format_string = ""
        
    def add_block(self):
        """
        Adds a COSMOS item of type BLOCK to packet
        """
        item = self.EventItem("name", "desc", 0, "BLOCK", [])
        self.evr_items.insert(0, item)
        
    def add_item(self, name, type, desc, enum):
        """
        Adds an item to the event packet
        @param name: Name of item
        @param desc: Comment of item
        @param type: Fprime version of argument data type (U16 as opposed to COSMOS's 16 UINT)
        @param enum: Tuple with enum data
        """
        # Add the length item into the packet for the following string
        if type == 'string':
            len_item = self.EventItem(name + "_length", "Length of String Arg", 16, "UINT", [])
            self.evr_items.append(len_item)
        
        cosmos_type = CosmosUtil.TYPE_DICT[type]
        value_type = (cosmos_type[1] if not (cosmos_type[1] == "ENUM" or cosmos_type[1] == "BOOLEAN") else cosmos_type[2])
        bits = cosmos_type[0]
        
        # Handle enum
        event_enum_types = []
        count = 0
        if not enum == None:
            num = 0
            for item in enum[1]:
                if item[1] == None:
                    event_enum_types.append((item[0], num))
                else:
                    event_enum_types.append((item[0], int(item[1])))
                num += 1
        types = event_enum_types
        
        item = self.EventItem(name, desc, bits, value_type, types)
            
        # Append item to proper lists
        self.non_len_names.append(name)
        self.names.append(name)
        self.evr_items.append(item)
        
        # Fix format string for enums (in COSMOS %d for enum must be %s)
        if not enum == None:
            CheetahUtil.evr_fix_format_str(self, len(self.non_len_names) - 1)
                
    def set_xml_attributes(self, severity, format_string):
        """
        Sets the Severity and Format String attributes from the XML
        @param severity: XML attribute "severity" found within XML source file
        @param format_string: XML attribute "format_string" found within XML source file
        """
        self.severity = severity
        self.format_string = format_string
                
    def get_evr_items(self):
        """
        List of items in packet
        """
        return self.evr_items
    def get_names(self):
        """
        Event names
        """
        return self.names
    def get_nonlen_names(self):
        """
        List of items other than the length items appended before strings
        """
        return self.non_len_names
    def get_id(self):
        """
        Event ID
        """
        return self.id
    def get_evr_name(self):
        """
        Event name
        """
        return self.evr_name
    def get_evr_desc(self):
        """
        Event description
        """
        return self.evr_desc
    def get_severity(self):
        """
        XML attribute "severity" found within XML source file
        """
        return self.severity
    def get_format_string(self):
        """
        XML attribute "format_string" found within XML source file
        """
        return self.format_string
