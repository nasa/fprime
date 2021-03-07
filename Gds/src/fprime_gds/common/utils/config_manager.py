"""
@brief Utility class to read config files and provide configuration values

After the first instance of this class is initialized, any class can obtain
the same instance by calling the static getInstance function. This allows any
part of the python program retrieve configuration data.

Based on the ConfigManager class written by Len Reder in the fprime Gse

@author R. Joseph Paetz

@date Created July 25, 2018

@license Copyright 2018, California Institute of Technology.
         ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
"""
import configparser

# Custom type modules
from fprime.common.models.serialize.numerical_types import (
    I8Type,
    I16Type,
    I32Type,
    I64Type,
    U8Type,
    U16Type,
    U32Type,
    U64Type,
    F32Type,
    F64Type,
)


class ConfigBadTypeException(Exception):
    def __init__(self, config_name, type_str):
        """
        Constructor

        Args:
            config_name (string): Name of the config containing the bad type
            type_str (string): Bad type string that caused the error
        """
        print(
            "Invalid type string {} read in configuration {}".format(
                type_str, config_name
            )
        )


class ConfigManager(configparser.ConfigParser):
    """
    This class provides a single entrypoint for all configurable properties,
    """

    __instance = None
    __prop = None

    def __init__(self):
        """
        Constructor

        Creates a ConfigManager object with the default configuration values

        Returns:
            An instance of the ConfigManager class. Default configurations
            will be used until the set_configs method is called!
        """
        # Cannot use super() function since ConfigParser is an old-style class
        configparser.ConfigParser.__init__(self)

        # Set default properties
        self.__prop = dict()
        self._set_defaults()
        self.file_path = None

    def set_configs(self, f):
        """
        Sets the configuration values to those in the given file

        Will raise an exception if the file does not exist.

        Args:
            f (string): Path to a file object to read
        """
        self.file_path = f
        self.read_file(open(f))

    @staticmethod
    def get_instance():
        """
        Return instance of singleton.

        Returns:
            The current ConfigManager object for this python application
        """
        if ConfigManager.__instance is None:
            ConfigManager.__instance = ConfigManager()
        return ConfigManager.__instance

    def get_type(self, name):
        """
        Retrieve a type from the config file for parsing

        It is assumed the setting is in the types section

        Args:
            name (string): Name of the type to retrieve

        Returns:
            If the name is valid, returns an object of a type derived from
            TypeBase. Otherwise, raises ConfigBadTypeException
        """
        type_str = self.get("types", name)

        if type_str == "U8":
            return U8Type()
        elif type_str == "U16":
            return U16Type()
        elif type_str == "U32":
            return U32Type()
        elif type_str == "u64":
            return U64Type()
        elif type_str == "I8":
            return I8Type()
        elif type_str == "I16":
            return I16Type()
        elif type_str == "I32":
            return I32Type()
        elif type_str == "I64":
            return I64Type()
        elif type_str == "F32":
            return F32Type()
        elif type_str == "F64":
            return F64Type()
        else:
            # These are types for parsing, so they need to be number types
            # Other types can be added later
            raise ConfigBadTypeException(name, type_str)

    def get_file_path(self):
        """
        Return file loaded for this configuration

        :return: file path
        """
        return self.file_path

    def _set_defaults(self):
        """
        Used by the constructor to set all ConfigParser defaults

        Establishes a dictionary of sections and then a dictionary of keyword,
        value association for each section.
        """

        ########################## TYPES ###########################
        # These configs give the types of fields in the binary data

        self.__prop["types"] = dict()

        self.__prop["types"]["msg_len"] = "U32"
        self.__prop["types"]["msg_desc"] = "U32"
        self.__prop["types"]["ch_id"] = "U32"
        self.__prop["types"]["event_id"] = "U32"
        self.__prop["types"]["op_code"] = "U32"
        self.__prop["types"]["pkt_id"] = "U16"
        self.__prop["types"]["key_val"] = "U16"
        self._set_section_defaults("types")

        ######################### COLORS ###########################
        # Colors are hex codes in BGR format

        self.__prop["colors"] = dict()

        self.__prop["colors"]["warning_lo"] = "0x00BCED"
        self.__prop["colors"]["warning_hi"] = "0x0073E5"
        self.__prop["colors"]["fatal"] = "0x0000FF"
        self.__prop["colors"]["command"] = "0xFF0000"
        self.__prop["colors"]["red"] = "0x0000FF"
        self.__prop["colors"]["orange"] = "0x0073E5"
        self.__prop["colors"]["yellow"] = "0x00BCED"

        self._set_section_defaults("colors")

        self.__prop["framing"] = dict()
        self.__prop["framing"]["use_key"] = "False"
        self.__prop["framing"]["key_val"] = "0x0"
        self._set_section_defaults("framing")

    def _set_section_defaults(self, section):
        """
        For a section set up the default values.

        Args:
            section: Section to set all the defaults config values for
        """
        self.add_section(section)
        for (key, value) in self.__prop[section].items():
            self.set(section, key, str(value))
