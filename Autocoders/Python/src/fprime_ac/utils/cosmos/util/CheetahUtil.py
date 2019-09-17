#!/bin/env python
#===============================================================================
# NAME: CheetahUtil.py
#
# DESCRIPTION: This class contains the methods that format data into the ways
# which Cheetah templates desire it.
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
import time
import datetime

from fprime_ac.utils.cosmos.util import CosmosUtil

"""
This class contains static data and methods should be altered in order to change the behavior of
Cheetah templates.
"""
#
# COSMOS OUTPUT STYLING
#
DATE = datetime.datetime.now().strftime("%A, %d, %B, %Y")
USER = os.environ['USER']

#
# CHANNELS
#
def convert_ch_limits(limits):
    """
    Converts the Fprime limits tuple into a Cosmos limits tuple, because they have different orderings
    and different necessary data.  Orange limits are not supported in COSMOS
    Fprime limits: (low_red, low_orange, low_yellow, high_yellow, high_orange, high_red)
    Cosmos limits: (persistence, limits_state, red_low, yellow_low, yellow_high, red_high)
    @param limits: tuple containing Fprime limits
    @return: A list containing 1 Cosmos limits tuple: list is for easy Cheetah iteration
    """
    if not limits[0] and not limits[1] and not limits[2] and not limits[3] and not limits[4] and not limits[5]:
        return []
    lr = limits[0]
    ly = limits[2]
    hy = limits[3]
    hr = limits[5]
    
    # Cosmos requires all limits to be set, so set to number out of range
    if not lr:
        lr = CosmosUtil.MIN_DICT['I64']
    if not ly:
        ly = CosmosUtil.MIN_DICT['I64']
    if not hr:
        hr = CosmosUtil.MAX_DICT['I64']
    if not hy:
        hy = CosmosUtil.MAX_DICT['I64']
    
    return [(CosmosUtil.LIMIT_PERSISTENCE, CosmosUtil.LIMIT_LIMITS_STATE, lr, ly, hy, hr)]
    
#
# COMMANDS
#
def cmd_convert_to_tuple(cmd_item):
    """
    Cheetah templates can't iterate over a list of classes, so
    converts all data into a Cheetah-friendly tuple
    (NAME, DESCRIPTION, ENUM, BIT_OFFSET, BITS, TYPE, MIN, MAX, DEFAULT)
    The class CommandItem that this method implicitly uses is located within the CommandItem class 
    """
    return (cmd_item.name, cmd_item.desc, cmd_item.types, cmd_item.bit_offset, cmd_item.bits, \
            cmd_item.type, cmd_item.min_val, cmd_item.max_val, cmd_item.default)    
    
def cmd_convert_items_to_cheetah_list(list):
    """
    Cheetah templates can't iterate over a list of classes, so
    converts all data into a Cheetah-friendly list of tuples
    (NAME, DESCRIPTION, ENUM, HAS_BIT_OFFSET, BIT_OFFSET, BITS, TYPE, MIN, MAX, DEFAULT)
    """
    temp = []
        
    for i in list:
        temp.append(cmd_convert_to_tuple(i))
        
    return temp

def cmd_update_variable_lengths(cmd):
    """
    When there are other arguments after a variable-length argument,
    determines their offset from the back of the packet instead of front.
    Only called when there is a single variable-length argument 
    (multi variable-length args not supported by COSMOS commands)
    @param cmd: Command to change
    """
    # Find location of the only variable_length argument
    reverse = False
    cmd_items = cmd.get_cmd_items()
    for arg in cmd_items:
        if arg.type == 'STRING':
            # If reverse already true, multiple variable length args so is a block
            if reverse == True:
                pass
            reverse = True
        
    count = 0
    if reverse:
        for item in reversed(cmd_items):
            if item.type == 'STRING':
                break
            count += item.bits
            item.bit_offset = count * -1

#
# EVENTS
#

def evr_convert_to_tuple(evr_item):
    """
    Cheetah templates can't iterate over a list of classes, so
    converts all data into a Cheetah-friendly tuple
    (IS_BLOCK, IS_DERIVED, NAME, DESCRIPTION, TEMPLATE_STRING, TYPES, HAS_NEG_OFFSET, NEG_OFFSET, BITS, TYPE)
    The class EventItem that this method implicitly uses is located within the CosmosEvent class 
    """
    return (evr_item.name, evr_item.desc, evr_item.template_string, evr_item.types, evr_item.bit_offset, evr_item.bits, evr_item.type)    

    
def evr_convert_items_to_cheetah_list(list):
    """
    Cheetah templates can't iterate over a list of classes, so
    converts all data into a Cheetah-friendly list of tuples
    (NAME, DESCRIPTION, ENUM, HAS_BIT_OFFSET, BIT_OFFSET, BITS, TYPE, MIN, MAX, DEFAULT)
    """
    temp = []
        
    for i in list:
        temp.append(evr_convert_to_tuple(i))
        
    return temp

def evr_fix_format_str(evr, search_index):
    """
    Enums are commonly referred to within formatted print statements in other
    languages as %d (integer) however, COSMOS saves enums as only their string
    value, and thus these %d's must be changed to %s's
    @param evr: The EVR to change
    @param search_index: The index within the item list of the enum to change
    """        
    # Fix enums from %d to %s for COSMOS
    char_indexes = [i for i, ch in enumerate(evr.format_string) if ch == "%"]
        
    ignore = False
    count = 0
    # Find count = search_index of "%" indexes in the format string and replace the following character (d in %d) with s (s in %s)
    for index in char_indexes:
        if ignore:
            ignore = False
        elif len(evr.format_string) > index + 1 and not evr.format_string[index + 1] == "%":
            if evr.format_string[index + 1] == "d" and count == search_index:
                evr.format_string = evr.format_string[:index + 1] + "s" + evr.format_string[index + 2:]
                break
        elif len(self.format_string) > index + 1:
            ignore = True
        count += 1
        
def evr_update_variable_lengths(evr):
    """
    When there are arguments after a string,
    determines their offset from the back of the packet.
    Only changes offsets when there is a single string 
    @param evr_items: The EVR to change
    """
    reverse = False
    evr_items = evr.get_evr_items()
    for item in evr_items:
        if item.type == 'STRING':
            # If reverse already true, multiple variable length args so is a block
            if reverse == True:
                evr.add_block()
                CosmosUtil.update_template_strings(evr_items)
                return
            reverse = True
        
    count = 0
    if reverse:
        for item in reversed(evr_items):
            if item.type == 'STRING':
                break
            count += item.bits
            item.bit_offset = count * -1
