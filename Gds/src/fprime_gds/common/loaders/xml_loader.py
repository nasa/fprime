"""
@brief Base class for all loaders that load dictionaries from xml dictionaries

The XmlLoader class inherits from the DictLoader base class and is intended
to be inherited by loader classes that find its helper functions useful for
parsing xml dictionaries.

This class does not implement any core loader functions, it just adds common
helper functions

@date Created July 19, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

import os
from copy import deepcopy

from lxml import etree

from fprime.common.models.serialize.array_type import ArrayType

# Custom type modules
from fprime.common.models.serialize.bool_type import BoolType
from fprime.common.models.serialize.enum_type import EnumType
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
from fprime.common.models.serialize.serializable_type import SerializableType
from fprime.common.models.serialize.string_type import StringType
from fprime_gds.common.data_types import exceptions
from fprime_gds.version import (
    MINIMUM_SUPPORTED_FRAMEWORK_VERSION,
    MAXIMUM_SUPPORTED_FRAMEWORK_VERSION,
)

# Custom Python Modules
from . import dict_loader


class XmlLoader(dict_loader.DictLoader):
    """Class to help load xml based dictionaries"""

    ENUM_SECT = "enums"
    ENUM_TYPE_TAG = "type"
    ENUM_ELEM_NAME_TAG = "name"
    ENUM_ELEM_VAL_TAG = "value"
    ENUM_ELEM_DESC_TAG = "description"

    # Xml section names and tags for serializable types
    SER_SECT = "serializables"
    SER_TYPE_TAG = "type"
    SER_MEMB_SECT = "members"
    SER_MEMB_NAME_TAG = "name"
    SER_MEMB_FMT_STR_TAG = "format_specifier"
    SER_MEMB_DESC_TAG = "description"
    SER_MEMB_TYPE_TAG = "type"

    # Xml section names and tags for array types
    ARR_SECT = "arrays"
    ARR_NAME_TAG = "name"
    ARR_TYPE_TAG = "type"
    ARR_SIZE_TAG = "size"
    ARR_FORMAT_TAG = "format"
    ARR_DEFAULT_TAG = "defaults"
    ARR_DEFAULT_VALUE_TAG = "value"

    # Xml section names and tags for argument sections
    ARGS_SECT = "args"
    ARG_NAME_TAG = "name"
    ARG_DESC_TAG = "description"
    ARG_TYPE_TAG = "type"

    STR_LEN_TAG = "len"

    def __init__(self):
        """
        Constructor

        Returns:
            An initialized loader object
        """
        super().__init__()

        # These dicts hold already parsed enum objects so things don't need
        # to be parsed multiple times
        self.enums = dict()
        self.serializable_types = dict()
        self.array_types = dict()

    @staticmethod
    def get_xml_tree(path):
        """
        Reads the xml file at the given path and parses it using lxml

        Args:
            path (string): Path to the xml dictionary file

        Returns:
            An lxml etree root object containing the parsed xml file
            information. Raises an exception if there is an error.
        """
        # Check that dictionary path exists
        if not os.path.isfile(path):
            raise exceptions.GseControllerUndefinedFileException(path)

        # Create xml parser
        xml_parser = etree.XMLParser(remove_comments=True)

        with open(path) as fd:
            # Parse xml and get element tree object we can retrieve data from
            element_tree = etree.parse(fd, parser=xml_parser)
        root = element_tree.getroot()

        # Check version of the XML before continuing. Versions weren't published before 1.5.4.  Only check major minor
        # and point versions to allow for development versions to be allowed.
        dict_version_string = root.attrib.get("framework_version", "1.5.4")
        digits = []
        # Process through the tokens of the version until we hit something that is not an int
        for token in dict_version_string.split("."):
            try:
                digits.append(int(token))
            except ValueError:
                break
        dict_version = tuple(digits)
        if (
            dict_version < MINIMUM_SUPPORTED_FRAMEWORK_VERSION
            or dict_version > MAXIMUM_SUPPORTED_FRAMEWORK_VERSION
        ):
            raise UnsupportedDictionaryVersionException(dict_version)
        return root

    @staticmethod
    def get_xml_section(section_name, xml_root):
        """
        Retrieve the given section in the xml tree if it exists

        Args:
            section_name (string): Section to retrieve
            xml_root (lxml etree root): xml object to search in

        Returns:
            The xml object of the desired section if found, or None if not
        """
        for section in xml_root:
            if section.tag == section_name:
                return section

        return None

    def get_args_list(self, xml_obj, xml_tree):
        """
        Parses and returns a standard xml dict arguments section:
          Section name: "args"
          Object tags: "arg"
          Object fields: "name", "type", "description"(optional)

        Args:
            xml_obj (lxml etree root): xml object containing the args section
                                       to parse.
            xml_tree (lxml etree root): Main xml tree object containing info on
                                        Enums and serializables.

        Returns:
            List of arguments where each argument is a tuple of the form:
            (arg name [string], arg description [string or None], arg obj
            [python object derived from TypeBase]). If there is no args section
            or there are no arguments in the args section, [] is returned.
        """
        args = []
        args_section = self.get_xml_section(self.ARGS_SECT, xml_obj)

        if args_section is not None:
            for arg in args_section:
                arg_dict = arg.attrib

                arg_name = arg_dict[self.ARG_NAME_TAG]
                arg_type_name = arg_dict[self.ARG_TYPE_TAG]
                arg_typ_obj = self.parse_type(arg_type_name, arg, xml_tree)

                arg_desc = None
                if self.ARG_DESC_TAG in arg_dict:
                    arg_desc = arg_dict[self.ARG_DESC_TAG]

                args.append((arg_name, arg_desc, arg_typ_obj))

        return args

    def get_enum_type(self, enum_name, xml_obj):
        """
        Parses and returns an Enum object for the given enum name.

        Looks in the enums section of the xml dict.

        Args:
            enum_name (string): Name of the enum to parse
            xml_obj (lxml etree root): Parsed Xml object to find the enum in

        Returns:
            If the enum name could be found in the xml_obj, a corresponding
            object of type EnumType is returned. Otherwise, None is returned.
            The caller will hold the only reference to the object.
        """
        # Check if there is an already parsed version of this enum
        if enum_name in self.enums:
            # Return a copy, so that the objects are not shared
            #  (Could cause nasty issues if two places try to deserialize)
            return deepcopy(self.enums[enum_name])

        # Check if the dictionary has an enum section
        enum_section = self.get_xml_section(self.ENUM_SECT, xml_obj)
        if enum_section is None:
            return None

        for enum in enum_section:
            # Check enum name
            if enum.get(self.ENUM_TYPE_TAG) == enum_name:
                # Go through all possible values of the enum
                members = dict()
                for item in enum:
                    item_name = item.get(self.ENUM_ELEM_NAME_TAG)
                    item_val = int(item.get(self.ENUM_ELEM_VAL_TAG))
                    members[item_name] = item_val

                enum_obj = EnumType(enum_name, members)

                self.enums[enum_name] = enum_obj
                return enum_obj

        return None

    def get_serializable_type(self, type_name, xml_obj):
        """
        Parses and returns a serializable type object for the given type name.

        Looks in the serializables section of the xml dict.

        Args:
            type_name (string): Name of the type to parse
            xml_obj (lxml etree root): Parsed Xml object to find type in

        Returns:
            If the type name could be found in the xml_obj, a corresponding
            object of a type derived from SerializableType is returned.
            Otherwise, None is returned. The caller will hold the only reference
            to the object.
        """
        # Check if there is already a parsed version of this serializable
        if type_name in self.serializable_types:
            # Return a copy, so that the objects are not shared
            return deepcopy(self.serializable_types[type_name])

        # Check if the dictionary has an enum section
        ser_section = self.get_xml_section(self.SER_SECT, xml_obj)
        if ser_section is None:
            return None

        for ser_type in ser_section:
            # Check if this serializable matches the type name
            if ser_type.get(self.SER_TYPE_TAG) == type_name:
                # Go through members
                memb_section = self.get_xml_section(self.SER_MEMB_SECT, ser_type)

                # If there is no member section, this type is invalid
                if memb_section is None:
                    return None

                members = []
                for memb in memb_section:
                    name = memb.get(self.SER_MEMB_NAME_TAG)
                    fmt_str = memb.get(self.SER_MEMB_FMT_STR_TAG)
                    desc = memb.get(self.SER_MEMB_DESC_TAG)
                    memb_type_name = memb.get(self.SER_MEMB_TYPE_TAG)
                    type_obj = self.parse_type(memb_type_name, memb, xml_obj)

                    members.append((name, type_obj, fmt_str, desc))

                ser_obj = SerializableType(type_name, members)

                self.serializable_types[type_name] = ser_obj
                return ser_obj

        return None

    def get_array_type(self, type_name, xml_obj):
        """
        Parses and returns an array type object for the given type name.

        Looks in the arrays section of the xml dict.

        Args:
            type_name (string): Name of the type to parse
            xml_obj (lxml etree root): Parsed Xml object to find type in

        Returns:
            If the type name could be found in the xml_obj, a corresponding
            object of a type derived from ArrayType is returned.
            Otherwise, None is returned. The caller will hold the only reference
            to the object.
        """

        # Check if there is already a parsed version of this array
        if type_name in self.array_types:
            # Return a copy, so that the objects are not shared
            return deepcopy(self.array_types[type_name])

        # Check if the dictionary has an array section
        arr_section = self.get_xml_section(self.ARR_SECT, xml_obj)
        if arr_section is None:
            return None

        for arr_memb in arr_section:
            # Check if this array matches the name name
            if arr_memb.get(self.ARR_NAME_TAG) == type_name:
                # Go through default members
                default_section = self.get_xml_section(self.ARR_DEFAULT_TAG, arr_memb)

                # If there is no default member section, this type is invalid
                if default_section is None:
                    return None

                # Make config
                arr_type = arr_memb.get(self.ARR_TYPE_TAG)
                type_obj = self.parse_type(arr_type, arr_memb, xml_obj)
                arr_format = arr_memb.get(self.ARR_FORMAT_TAG)
                arr_size = arr_memb.get(self.ARR_SIZE_TAG)

                arr_obj = ArrayType(type_name, (type_obj, int(arr_size), arr_format))

                self.array_types[type_name] = arr_obj
                return arr_obj

        return None

    def parse_type(self, type_name, xml_item, xml_tree):
        """
        Parses the given type string and returns a type object.

        Args:
            type_name (string): Name of the type in the xml
            xml_item (lxml etree root): Parsed xml object for the item
                      containing the type name being parsed. This is used to get
                      meta data such as string length.
            xml_tree (lxml etree root): Parsed Xml object containing enum and
                                        serializable type info (may not be used)

        Returns:
            Object of a class derived from the TypeBase class if successful,
            Raises an exception if the parsing fails. The caller will hold the
            only reference to the object.
        """

        if type_name == "I8":
            return I8Type()
        elif type_name == "I16":
            return I16Type()
        elif type_name == "I32":
            return I32Type()
        elif type_name == "I64":
            return I64Type()
        elif type_name == "U8":
            return U8Type()
        elif type_name == "U16":
            return U16Type()
        elif type_name == "U32":
            return U32Type()
        elif type_name == "U64":
            return U64Type()
        elif type_name == "F32":
            return F32Type()
        elif type_name == "F64":
            return F64Type()
        elif type_name == "bool":
            return BoolType()
        elif type_name == "string":
            if self.STR_LEN_TAG not in xml_item.attrib:
                print(
                    "Trying to parse string type, but found %s field" % self.STR_LEN_TAG
                )
                return None
            return StringType(max_string_len=int(xml_item.get(self.STR_LEN_TAG), 0))
        else:
            # First try Serialized types:
            result = self.get_serializable_type(type_name, xml_tree)
            if result is not None:
                return result

            # Now try enums:
            result = self.get_enum_type(type_name, xml_tree)
            if result is not None:
                return result

            # Now try arrays:
            result = self.get_array_type(type_name, xml_tree)
            if result is not None:
                return result

            # Abandon all hope
            raise exceptions.GseControllerParsingException(
                "Could not find type %s" % type_name
            )


class UnsupportedDictionaryVersionException(Exception):
    """ Dictionary is of unsupported version """

    def __init__(self, version):
        """ Create a dictionary of a specific version """

        def pretty(version_tuple):
            """ Pretty print version """
            return ".".join([str(item) for item in version_tuple])

        super().__init__(
            "Dictionary version {} is not in supported range: {}-{}. Please upgrade fprime-gds.".format(
                pretty(version),
                pretty(MINIMUM_SUPPORTED_FRAMEWORK_VERSION),
                pretty(MAXIMUM_SUPPORTED_FRAMEWORK_VERSION),
            )
        )
