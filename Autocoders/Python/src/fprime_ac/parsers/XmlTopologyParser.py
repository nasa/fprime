#!/usr/bin/env python3
# ===============================================================================
# NAME: XmlTopologyParser.py
#
# DESCRIPTION:  This class parses the XML serializable types files.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : July 2, 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging
import os

from lxml import etree, isoschematron

from fprime_ac.parsers import XmlComponentParser
from fprime_ac.utils import ConfigManager
from fprime_ac.utils.buildroot import (
    BuildRootCollisionException,
    BuildRootMissingException,
    locate_build_root,
)
from fprime_ac.utils.exceptions import (
    FprimeRngXmlValidationException,
    FprimeXmlException,
)

# from builtins import file
#
# Python extention modules and custom interfaces
#
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)

#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
ROOTDIR = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
#
class XmlTopologyParser:
    def __init__(self, xml_file=None):
        self.__root = None
        self.__is_topology_xml = False
        self.__in_connection = False
        self.__namespace = None
        self.__name = None
        self.__deployment = None
        self.__comment = ""
        if os.path.isfile(xml_file) == False:
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            raise OSError(stri)

        fd = open(xml_file)
        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file
        self.__instances = []
        self.__connections = []
        self.__comp_type_files = []
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.__comp_type_file_header = (
            {}
        )  # used to gather header file locations. Optional arg
        #
        self.Config = self.__config
        self.__base_id = None
        self.__base_id_window = None

        self.__prepend_instance_name = False  # Used to turn off prepending instance name in the situation where instance dicts are being generated and only one instance of an object is created
        element_tree = etree.parse(fd)

        # Validate against schema
        relax_file_handler = open(ROOTDIR + self.__config.get("schema", "assembly"))
        relax_parsed = etree.parse(relax_file_handler)
        relax_file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            raise FprimeRngXmlValidationException(relax_compiled.error_log)

        self.validate_xml(xml_file, element_tree, "schematron", "top_unique")

        for e in element_tree.iter():
            c = None
            if e.tag == "assembly" or e.tag == "deployment":
                self.__is_topology_xml = True
                if "namespace" in e.attrib:
                    self.__namespace = e.attrib["namespace"]
                else:
                    self.__namespace = None
                #
                if "name" in e.attrib:
                    self.__name = e.attrib["name"]
                else:
                    self.__name = None
                #
                if "base_id" in e.attrib:
                    self.__base_id = e.attrib["base_id"]
                else:
                    self.__base_id = None
                #
                if "base_id_window" in e.attrib:
                    self.__base_id_window = e.attrib["base_id_window"]
                elif "base_id_range" in e.attrib:
                    self.__base_id_window = e.attrib["base_id_range"]
                else:
                    self.__base_id_window = None

                if "prepend_instance_name" in e.attrib:
                    if e.attrib["prepend_instance_name"].upper() == "TRUE":
                        self.__prepend_instance_name = True
                #
                # The deployment attribute added so that instance
                # dictionaries are install in the correct place.
                #
                if "deployment" in e.attrib:
                    self.__deployment = e.attrib["deployment"]
                else:
                    self.__deployment = self.__name
                #
            elif e.tag == "import_component_type":
                self.__comp_type_files.append(e.text)
                if "header" in e.attrib:
                    self.__comp_type_file_header[e.text] = e.attrib["header"]
                else:
                    self.__comp_type_file_header[e.text] = None
                #
            elif e.tag == "connection":
                self.__in_connection = True
                n = e.attrib["name"]
                d = ""
                if e.tag == "type":
                    d = e.attrib["type"]
                else:
                    d = None
                connection = Connection(n, d)
                self.__connections.append(connection)
                #
            elif e.tag == "instance":
                x = e.attrib["name"]
                y = e.attrib["type"]
                z = e.attrib["namespace"]
                if "kind" in list(e.attrib.keys()):
                    k = e.attrib["kind"]
                else:
                    k = "active"
                k = k.lower()
                if "base_id" in e.attrib:
                    b = e.attrib["base_id"]
                else:
                    b = None
                #
                if "base_id_window" in e.attrib:
                    bw = e.attrib["base_id_window"]
                elif "base_id_range" in e.attrib:
                    bw = e.attrib["base_id_range"]
                else:
                    bw = None
                # Short name - Used to override instance name for dict generation
                if "dict_short_name" in e.attrib:
                    sn = e.attrib["dict_short_name"]
                else:
                    sn = None
                instance_obj = Instance(x, y, z, k, b, bw, sn)
                self.__instances.append(instance_obj)
            elif e.tag == "source":
                x = e.attrib["component"]
                y = e.attrib["port"]
                z = e.attrib["type"]
                if "num" in list(e.attrib.keys()):
                    n = int(e.attrib["num"])
                else:
                    n = 0
                self.__connections[-1].set_source(x, y, z, n)
            elif e.tag == "target":
                x = e.attrib["component"]
                y = e.attrib["port"]
                z = e.attrib["type"]
                if "num" in list(e.attrib.keys()):
                    n = int(e.attrib["num"])
                else:
                    n = 0
                self.__connections[-1].set_target(x, y, z, n)
            elif e.tag == "comment":
                c = e.text
                if self.__in_connection:
                    self.__connections[-1].set_comment(c)
                else:
                    self.__comment = c
            elif e.tag == etree.Comment:
                pass
            else:
                PRINT.info("WARNING: Unrecognized topology tag! %s" % (e.tag))
            # print "%s %s %s" % (e.tag, e.text, e.attrib)
        #
        # For each component xml file specified assign it to a component instance object
        PRINT.info("\nSearching for component XML files")
        for xml_file in self.__comp_type_files:
            try:
                xml_file = locate_build_root(xml_file)
            except BuildRootMissingException:
                PRINT.info("WARNING: Could not find XML file: %s" % xml_file)
            except BuildRootCollisionException as bre:
                stri = "ERROR: Could not find specified dictionary XML file. {}. Error: {}".format(
                    xml_file, str(bre)
                )
                raise OSError(stri)
            if os.path.exists(xml_file) == True:
                PRINT.info("Found component XML file: %s" % xml_file)
                xml_parsed = XmlComponentParser.XmlComponentParser(xml_file)
                for inst in self.get_instances():
                    if inst.get_type() == xml_parsed.get_component().get_name():
                        PRINT.info(
                            "Populating instance %s of type %s with parser tree"
                            % (inst.get_name(), inst.get_type())
                        )
                        DEBUG.info(
                            "Populating instance %s of type %s with parser tree"
                            % (inst.get_name(), inst.get_type())
                        )
                        inst.set_comp_xml(xml_parsed)
        #
        # For each instance determine if it is active or passive here...
        # Determine a maximum base id window required for each instance here
        PRINT.info(
            "\nDetermining kind of component and maximum reqired ID's for component instances"
        )
        for inst in self.get_instances():
            type = inst.get_type()
            name = inst.get_name()
            kind = inst.get_kind()
            xml_file = inst.get_comp_xml()

            if xml_file is None:
                PRINT.info(
                    "WARNING: No component XML present setting instance named: %s to kind: %s, maximum ID's used is unknown"
                    % (name, kind)
                )
            else:
                k = xml_file.get_component().get_kind()
                inst.set_kind(k)
                id_window = self.__max_id_window(inst)
                inst.set_base_max_id_window(id_window)
                DEBUG.info(
                    "Setting Instance name: %s, type: %s, kind: %s, maximum ID space required: %s"
                    % (name, type, inst.get_kind(), inst.get_base_max_id_window())
                )

    def validate_xml(self, dict_file, parsed_xml_tree, validator_type, validator_name):
        # Check that validator is valid
        if not self.Config.has_option(validator_type, validator_name):
            msg = (
                "XML Validator type "
                + validator_type
                + " not found in ConfigManager instance"
            )
            raise FprimeXmlException(msg)

        # Create proper xml validator tool
        validator_file_handler = open(
            ROOTDIR + self.Config.get(validator_type, validator_name)
        )
        validator_parsed = etree.parse(validator_file_handler)
        validator_file_handler.close()
        if validator_type == "schema":
            validator_compiled = etree.RelaxNG(validator_parsed)
        elif validator_type == "schematron":
            validator_compiled = isoschematron.Schematron(validator_parsed)

        # Validate XML file
        if not validator_compiled.validate(parsed_xml_tree):
            if validator_type == "schema":
                msg = "XML file {} is not valid according to {} {}.".format(
                    dict_file,
                    validator_type,
                    ROOTDIR + self.Config.get(validator_type, validator_name),
                )
                raise FprimeXmlException(msg)
            elif validator_type == "schematron":
                msg = "WARNING: XML file {} is not valid according to {} {}.".format(
                    dict_file,
                    validator_type,
                    ROOTDIR + self.Config.get(validator_type, validator_name),
                )
                PRINT.info(msg)

    def get_root(self):
        """
        Return root directory
        """
        return self.__root

    def is_topology(self):
        """
        Returns true if topology xml or false if other type.
        """
        return self.__is_topology_xml

    def get_xml_filename(self):
        """
        Return the original XML filename parsed.
        """
        return self.__xml_filename

    def get_namespace(self):
        """
        Return namespace
        """
        return self.__namespace

    def get_name(self):
        """
        Return name
        """
        return self.__name

    def get_deployment(self):
        """
        Return deployment for instanced topology dictionaries
        """
        return self.__deployment

    def get_comment(self):
        """
        Return top level topology comment string
        """
        return self.__comment

    def get_instances(self):
        """
        Returns a topology object.
        """
        return self.__instances

    def get_connections(self):
        """
        Returns a topology object.
        """
        return self.__connections

    def get_base_id(self):
        """
        Return base id of topology
        """
        return self.__base_id

    def get_component_includes(self):
        """
        Return component includes
        """
        return self.__comp_type_files

    def get_base_id_window(self):
        """
        Return base id window size of topology
        """
        return self.__base_id_window

    def __max_id_window(self, inst):
        """
        For an instance of a component compute the maximum space required for ID's
        """
        x = inst.get_comp_xml()
        cmd_ids = len(x.get_commands())
        ch_ids = len(x.get_channels())
        ev_ids = len(x.get_events())
        par_ids = len(x.get_parameters())
        return max([cmd_ids, ch_ids, ev_ids, par_ids])

    def get_comp_type_file_header_dict(self):
        return self.__comp_type_file_header

    def get_prepend_instance_name(self):
        return self.__prepend_instance_name


class Connection:
    """
    Storage for connection data
    """

    def __init__(self, name, type):
        """
        Constructor
        """
        self.__name = name
        self.__type = type
        self.__comment = None
        self.__source_comp = None
        self.__source_port = None
        self.__source_type = None
        self.__source_num = None
        self.__target_comp = None
        self.__target_port = None
        self.__target_type = None
        self.__target_num = None

    def set_comment(self, c):
        """
        Set comment string here...
        """
        self.__comment = c

    def set_source(self, comp, port, type, num):
        """
        Set source data here...
        """
        self.__source_comp = comp
        self.__source_port = port
        self.__source_type = type
        self.__source_num = num

    def set_target(self, comp, port, type, num):
        """
        Set target data here...
        """
        self.__target_comp = comp
        self.__target_port = port
        self.__target_type = type
        self.__target_num = num

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_source(self):
        return (
            self.__source_comp,
            self.__source_port,
            self.__source_type,
            self.__source_num,
        )

    def get_target(self):
        return (
            self.__target_comp,
            self.__target_port,
            self.__target_type,
            self.__target_num,
        )

    def get_comment(self):
        return self.__comment


class Instance:
    """
    Instance of components to connect
    """

    def __init__(
        self, name, type, namespace, kind, base_id, base_id_window, dict_short_name
    ):
        self.__name = name
        self.__type = type
        self.__namespace = namespace
        self.__kind = kind
        self.__base_id = base_id
        self.__base_id_window = base_id_window
        self.__base_max_id_window = None
        self.__xml_parsed_file = None
        self.__dict_short_name = dict_short_name
        self.__component_obj = None  # Used in TopoFactory to compute window IDS

    def set_component_object(self, comp):
        self.__component_obj = comp

    def get_component_object(self):
        return self.__component_obj

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_namespace(self):
        return self.__namespace

    def set_kind(self, k):
        self.__kind = k

    def get_kind(self):
        return self.__kind

    def get_base_id(self):
        return self.__base_id

    def get_base_id_window(self):
        return self.__base_id_window

    def get_comp_xml(self):
        return self.__xml_parsed_file

    def set_comp_xml(self, x):
        self.__xml_parsed_file = x

    def get_base_max_id_window(self):
        return self.__base_max_id_window

    def set_base_max_id_window(self, m):
        self.__base_max_id_window = m

    def get_dict_short_name(self):
        return self.__dict_short_name


if __name__ == "__main__":

    xmlfile = "../../test/app1a/DuckAppAi.xml"

    print("Topology XML parse test (%s)" % xmlfile)

    xml_topology_parser = XmlTopologyParser(xmlfile)
    instances = xml_topology_parser.get_instances()
    connections = xml_topology_parser.get_connections()

    print("Topology XML: %s" % xml_topology_parser.is_topology())
    print("Namespace: %s" % xml_topology_parser.get_namespace())
    print("Comment: %s" % xml_topology_parser.get_comment())

    print("Instances:")
    for x in instances:
        print("Name: {}, Type: {}".format(x.get_name(), x.get_type()))
    print("Connections:")
    for c in connections:
        print("Name: {}, Type: {}".format(c.get_name(), c.get_type()))
        print("Source: Component is %s, Port is %s, Port Type is %s" % (c.get_source()))
        print("Target: Component is %s, Port is %s, Port Type is %s" % (c.get_target()))
        print("Comment: %s" % c.get_comment())
