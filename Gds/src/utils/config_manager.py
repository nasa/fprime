'''
@brief Utility class to read config files and provide configuration values

After the first instance of this class is initialized, any class can obtain
the same instance by calling the static getInstance function. This allows any
part of the python program retrieve configuration data.

Based on the ConfigManager class written by Len Reder in the fprime Gse

@author R. Joseph Paetz

@date Created July 25, 2018

@lisence Copyright 2018, California Institute of Technology.
         ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
'''

import os
import sys
import glob
import ConfigParser

# Custom type modules
from models.serialize.f32_type import *
from models.serialize.f64_type import *

from models.serialize.u8_type import *
from models.serialize.u16_type import *
from models.serialize.u32_type import *
from models.serialize.u64_type import *

from models.serialize.i8_type import *
from models.serialize.i16_type import *
from models.serialize.i32_type import *
from models.serialize.i64_type import *


class ConfigBadTypeException(Exception):
    def __init__(self, config_name, type_str):
        '''
        Constructor

        Args:
            config_name (string): Name of the config containing the bad type
            type_str (string): Bad type string that caused the error
        '''
        print("Invalid type string %s read in configuration %s"%
              (type_str, config_name))


class ConfigManager(ConfigParser.SafeConfigParser):
    '''
    This class provides a single entrypoint for all configurable properties,
    '''

    __instance = None
    __prop     = None

    def __init__(self):
        '''
        Constructor

        Creates a ConfigManager object with the default configuration values

        Returns:
            An instance of the ConfigManager class. Default configurations
            will be used until the set_configs method is called!
        '''
        # Cannot use super() function since ConfigParser is an old-style class
        ConfigParser.SafeConfigParser.__init__(self)
        ConfigParser.ConfigParser.__init__(self)
        ConfigParser.RawConfigParser.__init__(self)

        # Set default properties
        self.__prop   = dict()
        self._set_defaults()


    def set_configs(self, f):
        '''
        Sets the configuration values to those in the given file

        Will raise an exception if the file does not exist.

        Args:
            f (string): Path to a file object to read
        '''
        self.readfp(open(f))


    @staticmethod
    def get_instance():
        '''
        Return instance of singleton.

        Returns:
            The current ConfigManager object for this python application
        '''
        if(ConfigManager.__instance is None):
            ConfigManager.__instance = ConfigManager()
        return ConfigManager.__instance


    def get_type(self, name):
        '''
        Retrieve a type from the config file for parsing

        It is assumed the setting is in the types section

        Args:
            name (string): Name of the type to retrieve

        Returns:
            If the name is valid, returns an object of a type derived from
            TypeBase. Otherwise, raises ConfigNonexistentException
        '''
        type_str = self.get('types', name)

        if (type_str == 'U8'):
            return U8Type()
        elif (type_str == 'U16'):
            return U16Type()
        elif (type_str == 'U32'):
            return U32Type()
        elif (type_str == 'u64'):
            return U64Type()
        elif (type_str == 'I8'):
            return I8Type()
        elif (type_str == 'I16'):
            return I16Type()
        elif (type_str == 'I32'):
            return I32Type()
        elif (type_str == 'I64'):
            return I64Type()
        elif (type_str == 'F32'):
            return F32Type()
        elif (type_str == 'F64'):
            return F64Type()
        else:
            # These are types for parsing, so they need to be number types
            # Other types can be added later
            raise ConfigNonexistentException(type_str)


    def _set_defaults(self):
        '''
        Used by the constructor to set all ConfigParser defaults

        Establishes a dictionary of sections and then a dictionary of keyword,
        value association for each section.
        '''

        self.__prop['types'] = dict()

        self.__prop['types']['msg_len'] = "U32"

        self._set_section_defaults('types')


    def _set_section_defaults(self, section):
        '''
        For a section set up the default values.

        Args:
            section: Section to set all the defaults config values for
        '''
        self.add_section(section)
        for (key,value) in self.__prop[section].items():
            self.set(section, key, str(value))


if __name__ == '__main__':
    pass

