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



    def _set_defaults(self):
        '''
        Used by the constructor to set all ConfigParser defaults

        Establishes a dictionary of sections and then a dictionary of keyword,
        value association for each section.
        '''

        self.__prop['misc'] = dict()

        self.__prop['misc']['test'] = "test config value [DEFAULT]"

        self._set_section_defaults('misc')


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

