#!/bin/env python
#===============================================================================
# NAME: CosmosChannel.py
#
# DESCRIPTION: This class represents a channel within COSMOS to conveniently
# store all the values necessary for the cheetah template to generate channels.
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

class CosmosChannel(BaseCosmosObject.BaseCosmosObject):
    """
    This class represents a channel within COSMOS to conveniently store 
    all the values necessary for the Cheetah template to generate channels.
    """
    
    def __init__(self, name, ch_id, comment):
        """
        @param name: Channel name
        @param ch_id: ID of channel
        @param comment: Channel description
        """
        super(CosmosChannel, self).__init__()
        self.id = ch_id
        self.ch_name = name.upper()
        self.ch_desc = comment
        self.value_bits = 0
        self.value_type = "ERROR: Value item type not set"
        self.format_string = "ERROR: Value item not set"
        self.types = []
        self.limits = (None, None, None, None, None, None)
    
    def set_item(self, type, enum, format_string=None):
        """
        Changes the COSMOS item "VALUE" attached to the channel instance
        @param type: Fprime version of argument data type (U16 as opposed to COSMOS's 16 UINT)
        @param enum: tuple containing name of enum as well as all name / value pairs, None if doesn't exist
        @param format_string: Optional formatting attachment for COSMOS "VALUE" argument
        """
        cosmos_type = CosmosUtil.TYPE_DICT[type]
        self.value_bits = cosmos_type[0]
        self.value_type = (cosmos_type[1] if not (cosmos_type[1] == "ENUM") else cosmos_type[2])
        
        if format_string == None:
            format_string = ""
        self.format_string = format_string
        
        # Handle enum
        channel_enum_types = []
        count = 0
        if not enum == None:
            num = 0
            for item in enum[1]:
                if item[1] == None:
                    channel_enum_types.append((item[0], num))
                else:
                    channel_enum_types.append((item[0], int(item[1])))
                num += 1
        self.types = channel_enum_types

    def set_limits(self, limits):
        """
        Adds limits to the Channel
        (low_red, low_orange, low_yellow, high_yellow, high_orange, high_red)
        Orange limits not supported in COSMOS, but kept inside of model for debugging and
        removed only when added to templates
        """
        if not type(limits) == tuple and not len(limits) == 6:
            print("ERROR: limits formatted incorrectly, should be Tuple length 6")
            self.limits = (None, None, None, None, None, None)
        else:
            self.limits = limits

    def get_id(self):
        """
        Channel ID
        """
        return self.id
    def get_ch_name(self):
        """
        Channel name
        """
        return self.ch_name
    def get_ch_desc(self):
        """
        Channel description
        """
        return self.ch_desc
    def get_value_bits(self):
        """
        Number of bits in COSMOS "VALUE" argument
        """
        return self.value_bits
    def get_value_type(self):
        """
        COSMOS type of "VALUE" argument
        """
        return self.value_type
    def get_format_string(self):
        """
        Optional format string of "VALUE" argument
        """
        return self.format_string
    def get_types(self):
        """
        Enum's items
        """
        return self.types
    def get_limits(self):
        """
        Channel Limits
        """
        return self.limits
