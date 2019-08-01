#!/bin/env python
#===============================================================================
# NAME: DeploymentUtil.py
#
# DESCRIPTION: This class contains all constant data fields which may need altering if
# Fprime's packet header items change or if the user wants to change port numbers
# for read and writer.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

"""
This class contains static data and methods that should be changed for each individual deployment
including header sizes, port numbers, etc.
"""

#
# DEPLOYMENT VARIABLES
#

# String-length item size and type
STRING_LEN_TYPE = 'U16'
    
# Total number of bits in the Event header items (Alter the file _TARGETNAME_tlm_evr_hdr.txt in the events directory as well)
EVR_HEADER_SIZE_BITS = 256
    
# Endianness for commands and telemetry
CMD_TLM_ENDIANNESS = 'BIG_ENDIAN'

#
# COSMOS LIMITS
#
LIMIT_PERSISTENCE = 1
LIMIT_LIMITS_STATE = 'ENABLED'
    
#
# COSMOS-SPECIFIC VALUES
#
TYPE_DICT = {}
MIN_DICT = {}
MAX_DICT = {}
DEFAULT_DICT = {}
    
def get_bits_from_type(type):
    """
    @param type: Fprime type
    @return: Number of bits in given Fprime type
    """
    if type == 'F32':
        return 32
    elif type == 'F64':
        return 64
    elif type == 'U8':
        return 8
    elif type == 'U16':
        return 16
    elif type == 'U32':
        return 32
    elif type == 'U64':
        return 64
    elif type == 'I8':
        return 8
    elif type == 'I16':
        return 16
    elif type == 'I32':
        return 32
    elif type == 'I64':
        return 64
    elif type == 'bool':
        return 8
    elif type == 'string':
        return 0
    elif type == 'ENUM':
        return 32
    else:
        # This gets passed back up to COSMOS if there's ever an error
        return "\"UNSUPPOPRTED DATA TYPE IN CosmosTopParser.py\""
    
    
def fill_cosmos_dicts():
    """
    Initializes the FPrime type -> COSMOS type dict TYPE_DICTs,
    and each of the min, max, and default value dicts (STORED IN BaseCosmosObject)
    TYPE_DICT: Number of bits and name of COSMOS for Fprime types
    MIN_DICT: Minimum values for each Fprime type
    MAX_DICT: Maximum values for each Fprime type
    DEFAULT_DICT: Default values for each Fprime type
    
    CHANGE THESE VALUES IF YOU WANT TO ALTER THE MIN/MAX VALUES FOR ALL COSMOS CMD/TLM CONFIG FILES GENERATED
    """
    TYPE_DICT['F32'] = (get_bits_from_type('F32'), 'FLOAT')
    TYPE_DICT['F64'] = (get_bits_from_type('F64'), 'FLOAT')
    TYPE_DICT['U8'] = (get_bits_from_type('U8'), 'UINT')
    TYPE_DICT['U16'] = (get_bits_from_type('U16'), 'UINT')
    TYPE_DICT['U32'] = (get_bits_from_type('U32'), 'UINT')
    TYPE_DICT['U64'] = (get_bits_from_type('U64'), 'UINT')
    TYPE_DICT['I8'] = (get_bits_from_type('I8'), 'INT')
    TYPE_DICT['I16'] = (get_bits_from_type('I16'), 'INT')
    TYPE_DICT['I32'] = (get_bits_from_type('I32'), 'INT')
    TYPE_DICT['I64'] = (get_bits_from_type('I64'), 'INT')
    TYPE_DICT['bool'] = (get_bits_from_type('bool'), 'UINT')
    TYPE_DICT['string'] = (get_bits_from_type('string'), 'STRING')
    TYPE_DICT['ENUM'] = (get_bits_from_type('ENUM'), 'ENUM', 'UINT')
        
    MIN_DICT['F32'] = -3.4e+38
    MIN_DICT['F64'] = -1.7e+308
    MIN_DICT['U8'] = 0
    MIN_DICT['U16'] = 0
    MIN_DICT['U32'] = 0
    MIN_DICT['U64'] = 0
    MIN_DICT['I8'] = -128
    MIN_DICT['I16'] = -32768
    MIN_DICT['I32'] = -2147483648
    MIN_DICT['I64'] = -9223372036854775808
    MIN_DICT['bool'] = 0
    MIN_DICT['string'] = ''
    MIN_DICT['ENUM'] = MIN_DICT['U32']
        
    MAX_DICT['F32'] = 3.4e+38
    MAX_DICT['F64'] = 1.7e+308
    MAX_DICT['U8'] = 255
    MAX_DICT['U16'] = 65535
    MAX_DICT['U32'] = 4294967295
    MAX_DICT['U64'] = 18446744073709551615
    MAX_DICT['I8'] = 127
    MAX_DICT['I16'] = 32767
    MAX_DICT['I32'] = 2147483647
    MAX_DICT['I64'] = 9223372036854775807
    MAX_DICT['bool'] = 255
    MAX_DICT['string'] = '' # Dont use this, should be set elsewhere to value from topology
    MAX_DICT['ENUM'] = MAX_DICT['U32']
        
    DEFAULT_DICT['F32'] = 0.0
    DEFAULT_DICT['F64'] = 0.0
    DEFAULT_DICT['U8'] = 0
    DEFAULT_DICT['U16'] = 0
    DEFAULT_DICT['U32'] = 0
    DEFAULT_DICT['U64'] = 0
    DEFAULT_DICT['I8'] = 0
    DEFAULT_DICT['I16'] = 0
    DEFAULT_DICT['I32'] = 0
    DEFAULT_DICT['I64'] = 0
    DEFAULT_DICT['bool'] = 0
    DEFAULT_DICT['string'] = 'String' # Dont use this, should be set elsewhere to value from topology
    DEFAULT_DICT['ENUM'] = 0    

fill_cosmos_dicts()
#
# EVENTS
#
        
def update_template_strings(evr_items):
    """
    Calculates the template string for each item in packet with multiple strings
    
    Template String Example For Third Argument(caps are real strings,
    lowercase w/ underscores are variable integer values):
    'bits_from_start_of_packet START arg1_bits arg1_data_type arg2_bits
    arg2_data_type arg3_bits arg3_data_type'
    """

    total_pre_item_bits = EVR_HEADER_SIZE_BITS
    aggregate = str(total_pre_item_bits) + ' START'
    for item in evr_items:
        if not item.type == 'BLOCK':
            item.template_string = aggregate + ' ' + str(item.bits) + ' ' + item.type
            aggregate = item.template_string

# Global variable set in cosmosgen.py, accessed statically from this module in all other locations
VERBOSE = False
STARTING_DIRECTORY = ""
