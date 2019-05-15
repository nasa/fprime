#!/bin/env python
#===============================================================================
# NAME: CheetahUtil.py
#
# DESCRIPTION: This class provides a single entrypoint for all configurable
# deployment servers / protocols.
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
if sys.version_info >= (3,):
    import configparser as ConfigParser
else:
    import ConfigParser

class CosmosConfigManager(ConfigParser.SafeConfigParser):
    """
    This class provides a single entry point for all configurable deployment servers / protocols.
    """
    
    __instance = None
    __prop     = None
    
    def __init__(self, deployment):
        """
        Constructor.
        """
        ConfigParser.SafeConfigParser.__init__(self)
        ConfigParser.ConfigParser.__init__(self)
        ConfigParser.RawConfigParser.__init__(self)
        self.__prop   = dict()
        self._setProps()
       
        # Check for ini file in current working directory with name as deployment
        util_dir = os.environ['BUILD_ROOT'] + "/Autocoders/Python/src/fprime_ac/utils/cosmos/util/"
        ls = os.listdir(util_dir)
        
        file = None
        for fl in ls:
            if fl[0:len(deployment)].lower() == deployment.lower() and fl[len(deployment):len(fl)].lower() == '.ini':
                file = fl
                
        if file is None:
            print("ERROR: Could not find configuration file for deployment: %s, \ndefine a *.ini file in %s directory." % (deployment, util_dir))
            sys.exit(-1)
        
        self.read(util_dir + file)
    
    def getInstance(deployment):
        """
        Return instance of singleton.
        """
        if(CosmosConfigManager.__instance is None):
            CosmosConfigManager.__instance = CosmosConfigManager(deployment)
        return CosmosConfigManager.__instance
    
    #define static method
    getInstance = staticmethod(getInstance)
    
    
    def _setProps(self):
        """
        Used only by constructor to set all ConfigParser defaults. Establishes
        a dictionary of sections and then a dictionary of keyword, value
        association for each section.
        @params None
        """
        ################################################################
        # General parameters here. This is the DEFAULT section.
        ################################################################
        # self.__prop['DEFAULT'] = dict()
        
        #
        # 'DEFAULT' section is baked in for Python 2.6x
        # the call below will raise ValueError
        #
        
        # This sets the defaults within a section.
        # self._setSectionDefaults('DEFAULT')
        ################################################################
        # schema parameters here.
        ################################################################
        #When using these, make sure to either prepend the build root env var or the appropriate amount of "../"
        self.__prop['deployment'] = dict()
        self.__prop['deployment']["write_port"]               = 5000
        self.__prop['deployment']["read_port"]                 = 5000
        self.__prop['deployment']["read_timeout"]              = 10
        self.__prop['deployment']["write_timeout"]              = 10
        self.__prop['deployment']["protocol_name_w"]               = 'FPrimeProtocol'
        self.__prop['deployment']["protocol_name_r"]             = 'FPrimeProtocol'
        self.__prop['deployment']["len_bit_offset_w"]    = 32
        self.__prop['deployment']["len_bit_offset_r"]              = 72
        self.__prop['deployment']["len_bit_size_w"]              = 32
        self.__prop['deployment']["len_bit_size_r"]              = 32
        self.__prop['deployment']["len_val_offset_w"]              = 8
        self.__prop['deployment']["len_val_offset_r"]              = 13
        self.__prop['deployment']["bytes_per_count_w"]              = 1
        self.__prop['deployment']["bytes_per_count_r"]              = 1
        self.__prop['deployment']["endianness_w"]              = 'BIG_ENDIAN'
        self.__prop['deployment']["endianness_r"]              = 'BIG_ENDIAN'
        self.__prop['deployment']["discard_leading_w"]              = 0
        self.__prop['deployment']["discard_leading_r"]              = 0
        self.__prop['deployment']["sync_w"]              = '5A5A5A5A'
        self.__prop['deployment']["sync_r"]              = '413541352047554920'
        self.__prop['deployment']["has_max_length_w"]              = 'nil'
        self.__prop['deployment']["has_max_length_r"]              = 'nil'
        self.__prop['deployment']["fill_ls_w"]              = 'true'
        self.__prop['deployment']["fill_ls_r"]              = 'true'
        self._setSectionDefaults('deployment')
    
    
    def _setSectionDefaults(self, section):
        """
        For a section set up the default values.
        """
        self.add_section(section)
        for (key,value) in self.__prop[section].items():
            self.set(section, key, "%s" % value)
