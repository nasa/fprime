#!/usr/bin/env python3
# ===============================================================================
# NAME: XmlComponentParser.py
#
# DESCRIPTION:  This parses the XML component description files.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Aug. 14, 2007
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import configparser
import logging
import os
import sys

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
from lxml import etree, isoschematron

# For Python determination

#
# Python extension modules and custom interfaces
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
class XmlComponentParser:
    def __init__(self, xml_file=None):
        self.__root = None
        self.__import_port_type_files = []
        self.__import_serializable_type_files = []
        self.__include_header_files = []
        self.__import_dictionary_files = []
        self.__import_enum_type_files = []
        self.__import_array_type_files = []
        #
        self.__is_component_xml = False
        #
        self.__component = None
        self.__ports = []
        self.__commands = []
        self.__internal_interfaces = []
        self.__channels = []
        self.__parameters = []
        self.__events = []
        self.__instances = (
            None  # Stores number of detected instances based on id base values
        )

        #
        if not os.path.isfile(xml_file):
            stri = "ERROR: Could not find specified XML file %s." % xml_file
            raise OSError(stri)

        fd = open(xml_file)
        xml_file = os.path.basename(xml_file)
        self.__xml_filename = xml_file
        #
        self.Config = ConfigManager.ConfigManager.getInstance()
        ## Dictionary of special_ports
        self.special_ports = self.Config._ConfigManager__prop["special_ports"]

        ## get constants file name and read it in
        constants_file = self.Config.get("constants", "constants_file")
        if not os.path.isabs(constants_file):
            constants_file = os.path.join(ROOTDIR, constants_file)
        ## make sure it is a real file
        if os.path.isfile(constants_file):
            self.__const_parser = configparser.ConfigParser()
            self.__const_parser.read(constants_file)
        else:
            self.__const_parser = None

        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd, parser=xml_parser)
        fd.close()  # Close the file, which is only used for the parsing above

        # Validate against current schema. if more are imported later in the process, they will be reevaluated
        relax_file_handler = open(ROOTDIR + self.Config.get("schema", "component"))
        relax_parsed = etree.parse(relax_file_handler)
        relax_file_handler.close()
        relax_compiled = etree.RelaxNG(relax_parsed)

        # 2/3 conversion
        if not relax_compiled.validate(element_tree):
            raise FprimeRngXmlValidationException(relax_compiled.error_log)

        # Check for async_input port
        self.validate_xml(xml_file, element_tree, "schematron", "active_comp")

        self.validate_xml(xml_file, element_tree, "schematron", "comp_unique")

        ## Add Implicit ports if needed
        #  element_tree = __check_ports(element_tree)

        ## Used to check if implicit ports need to be defined
        has_commands = False
        has_parameters = False
        has_events = False
        has_telemetry = False

        component = element_tree.getroot()
        component_name = component.attrib["name"]

        print("Parsing Component %s" % component_name)

        if "namespace" in component.attrib:
            namespace_name = component.attrib["namespace"]
        else:
            namespace_name = None

        if "modeler" in component.attrib:
            modeler = True
            if component.attrib["modeler"] != "true":
                PRINT.info(
                    '%s: Component %s: "modeler" attribute must be "true" or not present'
                    % (xml_file, component_name)
                )
                sys.exit(-1)
        else:
            modeler = False

        comp_kind = component.attrib["kind"]
        if comp_kind not in ["passive", "queued", "active"]:
            PRINT.info(
                '%s: Component %s: "kind" attribute must be "passive","queued", or "active"'
                % (xml_file, component_name)
            )
            sys.exit(-1)

        self.__component = Component(
            namespace_name, component_name, component.attrib["kind"], None, modeler
        )

        for comp_tag in component:
            if comp_tag.tag == "comment":
                self.__component.set_comment(comp_tag.text.strip())
            elif comp_tag.tag == "include_header":
                self.__include_header_files.append(comp_tag.text)
            elif comp_tag.tag == "import_port_type":
                self.__import_port_type_files.append(comp_tag.text)
            elif comp_tag.tag == "import_serializable_type":
                self.__import_serializable_type_files.append(comp_tag.text)
            elif comp_tag.tag == "import_enum_type":
                self.__import_enum_type_files.append(comp_tag.text)
            elif comp_tag.tag == "import_array_type":
                self.__import_array_type_files.append(comp_tag.text)
            elif comp_tag.tag == "import_dictionary":
                try:
                    dict_file = locate_build_root(comp_tag.text)
                except (BuildRootMissingException, BuildRootCollisionException) as bre:
                    stri = "ERROR: Could not find specified dictionary XML file. {}. Error: {}".format(
                        comp_tag.text, str(bre)
                    )
                    raise OSError(stri)
                PRINT.info("Reading external dictionary %s" % dict_file)
                dict_fd = open(dict_file)
                _ = etree.XMLParser(remove_comments=True)
                dict_element_tree = etree.parse(dict_fd, parser=xml_parser)

                component.append(dict_element_tree.getroot())

                # Validate new imports using their root tag as a key to find what schema to use
                self.validate_xml(
                    dict_file,
                    dict_element_tree,
                    "schema",
                    dict_element_tree.getroot().tag.lower(),
                )

                # Validate ID and Opcode uniqueness with Schematron
                self.validate_xml(dict_file, dict_element_tree, "schematron", "chan_id")
                self.validate_xml(dict_file, dict_element_tree, "schematron", "evr_id")
                self.validate_xml(
                    dict_file, dict_element_tree, "schematron", "param_id"
                )
                self.validate_xml(dict_file, dict_element_tree, "schematron", "cmd_op")

                # add to list of imported dictionaries for make dependencies later
                self.__import_dictionary_files.append(comp_tag.text)
            elif comp_tag.tag == "ports":  # parse ports
                for port in comp_tag:
                    if port.tag != "port":
                        PRINT.info(
                            "%s: Invalid tag %s in ports definition"
                            % (xml_file, port.tag)
                        )
                        sys.exit(-1)
                    n = port.attrib["name"]
                    ## Set role if defined
                    r = None
                    if "role" in list(port.attrib.keys()):
                        r = port.attrib["role"]
                    d_orig = port.attrib["kind"]
                    # special case for MagicDraw modeling tool. A bit of a hack...
                    if d_orig == "input":
                        d_orig = "model_input"
                    if (
                        component.attrib["kind"] == "passive"
                        and d_orig == "async_input"
                    ):
                        PRINT.info(
                            "%s: Port %s cannot be async_input with a passive component"
                            % (xml_file, n)
                        )
                        sys.exit(-1)
                    d = d_orig.split("_")
                    t = port.attrib["data_type"]
                    if d[0] == "output":
                        s = None
                        d = "output"
                    else:
                        err = '%s: Invalid attribute value "%s" for "kind" in port "%s" definition. Should be one of "sync_input", "async_input", "guarded_input", or "output"'
                        s = d[0]
                        if s not in ["sync", "async", "guarded", "model"]:
                            PRINT.info(err % (xml_file, d_orig, n))
                            sys.exit(-1)
                        d = d[1]
                        if d != "input":
                            PRINT.info(err % (xml_file, d_orig, n))
                            sys.exit(-1)
                    if "max_number" in list(port.attrib.keys()):
                        m = self.__eval_var(
                            constants_file, "Component", port.attrib["max_number"]
                        )
                    else:
                        m = 1
                    if "priority" in list(port.attrib.keys()):
                        p = port.attrib["priority"]
                    else:
                        p = 0
                    if "full" in list(port.attrib.keys()):
                        f = port.attrib["full"]
                        if f not in ["drop", "assert", "block"]:
                            err = '%s: Invalid attribute value "%s" for "full" in port "%s" definition. Should be one of "drop", "assert", or "block"'
                            PRINT.info(err % (xml_file, f, n))
                            sys.exit(-1)
                    else:
                        f = "assert"
                    port_obj = Port(n, d, t, s, p, f, None, m, role=r)
                    for comment in port:
                        # only valid tag in ports is comment
                        if comment.tag != "comment":
                            PRINT.info(
                                "%s: Invalid tag %s in port %s"
                                % (xml_file, comment.tag, port.tag)
                            )
                            sys.exit(-1)
                        port_obj.set_comment(comment.text.strip())
                    self.__ports.append(port_obj)
            elif comp_tag.tag == "commands":  # parse commands
                # see if command opcode base is specified
                if "opcode_base" in list(comp_tag.attrib.keys()):
                    opcode_base = self.__eval_var(
                        constants_file, "Component", comp_tag.attrib["opcode_base"]
                    )
                    opcode_bases = opcode_base.split(",")
                    # Check number of instances
                    if self.__instances is None:
                        self.__instances = len(opcode_bases)
                    else:
                        if len(opcode_bases) != self.__instances:
                            PRINT.info(
                                "%s: Component has mismatched command instances: %d vs. %d"
                                % (xml_file, len(opcode_bases), self.__instances)
                            )
                            sys.exit(-1)

                    # convert text bases to integers
                    opcode_base_list = []
                    for base in opcode_bases:
                        opcode_base_list.append(int(base, base=0))
                else:
                    opcode_bases = None

                for command in comp_tag:
                    if command.tag != "command":
                        PRINT.info(
                            "%s: Invalid tag %s in commands definition"
                            % (xml_file, command.tag)
                        )
                        sys.exit(-1)
                    m = command.attrib["mnemonic"]
                    o = command.attrib["opcode"]
                    # check to see if there is a base opcode specified
                    opcode_list = []
                    if opcode_bases is not None:
                        # walk through opcode base list
                        for base in opcode_base_list:
                            opcode_list.append("0x%X" % (int(o, base=0) + base))
                    else:
                        opcode_list.append(o)

                    s = command.attrib["kind"]
                    if component.attrib["kind"] == "passive" and s == "async":
                        PRINT.info(
                            "%s: Command %s cannot be async with a passive component"
                            % (xml_file, m)
                        )
                        sys.exit(-1)
                    if "priority" in list(command.attrib.keys()):
                        p = command.attrib["priority"]
                    else:
                        p = 0

                    if "full" in list(command.attrib.keys()):
                        f = command.attrib["full"]
                        if f not in ["drop", "assert", "block"]:
                            err = '%s: Invalid attribute value "%s" for "full" in command "%s" definition. Should be one of "drop", "assert", or "block"'
                            PRINT.info(err % (xml_file, f, m))
                            sys.exit(-1)
                    else:
                        f = "assert"

                    err = '%s: Invalid attribute "%s" in command "%s" definition. Should be one of "sync", "async", or "guarded"'
                    if s not in ["sync", "async", "guarded"]:
                        PRINT.info(err % (xml_file, s, m))
                        sys.exit(-1)
                    command_obj = Command(
                        m, opcode_list, s, p, None, base_opcode=o, full=f
                    )
                    for command_tag in command:
                        # parse command tags
                        if command_tag.tag == "comment":
                            command_obj.set_comment(command_tag.text.strip())
                        elif command_tag.tag == "args":
                            for arg in command_tag:
                                if arg.tag != "arg":
                                    PRINT.info(
                                        "%s: Invalid tag %s in command args definition"
                                        % (xml_file, arg.tag)
                                    )
                                    sys.exit(-1)
                                n = arg.attrib["name"]
                                t = arg.attrib["type"]
                                if t == "string":
                                    s = arg.attrib["size"]
                                else:
                                    s = None
                                command_arg_obj = CommandArg(n, t, s)
                                for arg_tag in arg:
                                    # only valid tag in command args is comment
                                    if arg_tag.tag == "comment":
                                        command_arg_obj.set_comment(arg_tag.text)
                                    elif arg_tag.tag == "enum" and t == "ENUM":
                                        en = arg_tag.attrib["name"]
                                        enum_members = []
                                        for mem in arg_tag:
                                            mn = mem.attrib["name"]
                                            if "value" in list(mem.attrib.keys()):
                                                v = mem.attrib["value"]
                                            else:
                                                v = None
                                            if "comment" in list(mem.attrib.keys()):
                                                mc = mem.attrib["comment"].strip()
                                            else:
                                                mc = None
                                            enum_members.append((mn, v, mc))
                                        command_arg_obj.set_type(
                                            ((t, en), enum_members)
                                        )
                                    else:
                                        PRINT.info(
                                            "%s: Invalid argument tag %s in command %s argument %s"
                                            % (
                                                xml_file,
                                                arg_tag.tag,
                                                command_tag.tag,
                                                n,
                                            )
                                        )
                                        sys.exit(-1)
                                command_obj.add_arg(command_arg_obj)
                        else:
                            PRINT.info(
                                "%s: Invalid tag %s in command definition"
                                % (xml_file, command_tag.tag)
                            )
                            sys.exit(-1)
                    self.__commands.append(command_obj)
                ## Check if there was at least 1 command
                if len(self.__commands) > 0:
                    has_commands = True
                else:
                    PRINT.info(
                        "Warning: No commands defined within the 'commands' tag "
                    )
            elif comp_tag.tag == "telemetry":  # parse telemetry channels
                if "telemetry_base" in list(comp_tag.attrib.keys()):
                    telemetry_base = self.__eval_var(
                        constants_file, "Component", comp_tag.attrib["telemetry_base"]
                    )
                    telemetry_bases = telemetry_base.split(",")
                    # Check number of instances
                    if self.__instances is None:
                        self.__instances = len(telemetry_bases)
                    else:
                        if len(telemetry_bases) != self.__instances:
                            PRINT.info(
                                "%s: Component has mismatched telemetry instances: %d vs. %d"
                                % (xml_file, len(telemetry_bases), self.__instances)
                            )
                            sys.exit(-1)

                    # convert text bases to integers
                    telemetry_base_list = []
                    for base in telemetry_bases:
                        telemetry_base_list.append(int(base, base=0))
                else:
                    telemetry_bases = None

                for channel in comp_tag:
                    if channel.tag != "channel":
                        PRINT.info(
                            "%s: Invalid tag %s in channel definition"
                            % (xml_file, channel.tag)
                        )
                        sys.exit(-1)
                    i = channel.attrib["id"]
                    # check to see if there is a base id specified
                    id_list = []
                    if telemetry_bases is not None:
                        # walk through opcode base list
                        for base in telemetry_base_list:
                            id_list.append("0x%X" % (int(i, base=0) + base))
                    else:
                        id_list.append(i)

                    # Initialize limits
                    lr = None
                    lo = None
                    ly = None
                    hy = None
                    ho = None
                    hr = None

                    if "low_red" in list(channel.attrib.keys()):
                        lr = channel.attrib["low_red"]
                    if "low_orange" in list(channel.attrib.keys()):
                        lo = channel.attrib["low_orange"]
                    if "low_yellow" in list(channel.attrib.keys()):
                        ly = channel.attrib["low_yellow"]
                    if "high_yellow" in list(channel.attrib.keys()):
                        hy = channel.attrib["high_yellow"]
                    if "high_orange" in list(channel.attrib.keys()):
                        ho = channel.attrib["high_orange"]
                    if "high_red" in list(channel.attrib.keys()):
                        hr = channel.attrib["high_red"]

                    n = channel.attrib["name"]

                    # type
                    if "data_type" in list(channel.attrib.keys()) and "type" in list(
                        channel.attrib.keys()
                    ):
                        PRINT.info(
                            "%s: Telemetry channel %s attributes 'data_type' and 'type' are  both specified. Only specify one."
                            % (xml_file, n)
                        )
                        sys.exit(-1)

                    if "data_type" in list(channel.attrib.keys()):
                        d = channel.attrib["data_type"]
                    else:
                        d = channel.attrib["type"]

                    if "format_string" in list(channel.attrib.keys()):
                        f = channel.attrib["format_string"]
                    else:
                        f = None
                    if "update" in list(channel.attrib.keys()):
                        u = channel.attrib["update"]
                        if u not in ("always", "on_change"):
                            PRINT.info(
                                '%s: Invalid update %s in channel %s. Should be "always" or "on_change"'
                                % (xml_file, u, n)
                            )
                            sys.exit(-1)
                    else:
                        u = None
                    if "abbrev" in list(channel.attrib.keys()):
                        a = channel.attrib["abbrev"]
                    else:
                        a = None
                    s = None
                    if d == "string":
                        if not "size" in list(channel.attrib.keys()):
                            PRINT.info(
                                "%s: Telemetry channel %s string value must specify a size"
                                % (xml_file, n)
                            )
                            sys.exit(-1)
                        s = channel.attrib["size"]
                    channel_obj = Channel(
                        ids=id_list,
                        name=n,
                        type=d,
                        size=s,
                        abbrev=a,
                        format_string=f,
                        update=u,
                        limits=(lr, lo, ly, hy, ho, hr),
                    )
                    for channel_tag in channel:
                        # check for comments or enums
                        if channel_tag.tag == "comment":
                            channel_obj.set_comment(channel_tag.text.strip())
                        elif channel_tag.tag == "enum" and d == "ENUM":
                            en = channel_tag.attrib["name"]
                            enum_members = []
                            for mem in channel_tag:
                                mn = mem.attrib["name"]
                                if "value" in list(mem.attrib.keys()):
                                    v = mem.attrib["value"]
                                else:
                                    v = None
                                if "comment" in list(mem.attrib.keys()):
                                    mc = mem.attrib["comment"].strip()
                                else:
                                    mc = None
                                enum_members.append((mn, v, mc))
                            channel_obj.set_type(((d, en), enum_members))
                        elif channel_tag.tag == "units":
                            # print "******************Found: %s" % channel_tag.tag
                            name = channel_tag.attrib["name"]
                            # print "Name: %s" % name
                            gain = channel_tag.attrib["gain"]
                            # print "Gain: %s" % gain
                            offset = channel_tag.attrib["offset"]
                            # print "Offset: %s" % offset
                            channel_obj.set_units(name, gain, offset)
                        else:
                            PRINT.info(
                                "%s: Invalid tag %s in channel %s"
                                % (xml_file, channel_tag.tag, n)
                            )
                            sys.exit(-1)
                    self.__channels.append(channel_obj)
                if len(self.__channels) > 0:
                    has_telemetry = True
                else:
                    PRINT.info("Warning: No channels defined within telemetry tag")
            elif comp_tag.tag == "events":  # parse events
                if "event_base" in list(comp_tag.attrib.keys()):
                    event_base = self.__eval_var(
                        constants_file, "Component", comp_tag.attrib["event_base"]
                    )
                    event_bases = event_base.split(",")
                    # Check number of instances
                    if self.__instances is None:
                        self.__instances = len(event_bases)
                    else:
                        if len(event_bases) != self.__instances:
                            PRINT.info(
                                "%s: Component has mismatched event instances: %d vs. %d"
                                % (xml_file, len(opcode_bases), self.__instances)
                            )
                            sys.exit(-1)

                    # convert text bases to integers
                    event_base_list = []
                    for base in event_bases:
                        event_base_list.append(int(base, base=0))

                else:
                    event_bases = None

                for event in comp_tag:
                    if event.tag != "event":
                        PRINT.info(
                            "%s: Invalid tag %s in events definition"
                            % (xml_file, event.tag)
                        )
                        sys.exit(-1)
                    i = event.attrib["id"]
                    # check to see if there is a base id specified
                    id_list = []
                    if event_bases is not None:
                        # walk through opcode base list
                        for base in event_base_list:
                            id_list.append("0x%X" % (int(i, base=0) + base))
                    else:
                        id_list.append(i)

                    n = event.attrib["name"]
                    s = event.attrib["severity"]
                    # FIXME: Move to configuration file
                    severity_list = [
                        "FATAL",
                        "WARNING_HI",
                        "WARNING_LO",
                        "COMMAND",
                        "ACTIVITY_HI",
                        "ACTIVITY_LO",
                        "DIAGNOSTIC",
                    ]
                    if s not in severity_list:
                        PRINT.info(
                            "%s: Error: Event %s severity must be one of %s."
                            % (xml_file, n, ",".join(severity_list))
                        )
                        sys.exit(-1)
                    f = event.attrib["format_string"]
                    # Finding how many arguments are within the format string
                    f_temp = f.replace("%%", "")
                    f_arg_amount = f_temp.count("%")

                    if "throttle" in list(event.attrib.keys()):
                        t = event.attrib["throttle"]
                    else:
                        t = None

                    event_obj = Event(id_list, n, s, f, t)
                    for event_tag in event:
                        # parse event tags
                        if event_tag.tag == "comment":
                            event_obj.set_comment(event_tag.text.strip())
                        elif event_tag.tag == "args":
                            # Check if amount of args matches with the amount of arguments within the format string
                            if f_arg_amount != len(event_tag):
                                argument_tuple = (
                                    n,
                                    component_name,
                                    f_arg_amount,
                                    len(event_tag),
                                )  # event, component name,  format string arg count , supplied arg count
                                PRINT.info(
                                    "ArgumentCountMismatch: For event {} in component {} there are {} specified arguments in the format string but {} supplied arguments in the 'args' tag.".format(
                                        *argument_tuple
                                    )
                                )
                                PRINT.info("File path: {}".format(xml_file))
                                sys.exit(-1)
                            for arg in event_tag:
                                if arg.tag != "arg":
                                    PRINT.info(
                                        "%s: Invalid tag %s in %s event args definition"
                                        % (xml_file, arg.tag, event.attrib["name"])
                                    )
                                    sys.exit(-1)
                                n = arg.attrib["name"]
                                t = arg.attrib["type"]
                                s = None
                                if t == "string":
                                    if not "size" in list(arg.attrib.keys()):
                                        PRINT.info(
                                            "%s: Event %s string argument %s must specify a size"
                                            % (xml_file, event.attrib["name"], n)
                                        )
                                        sys.exit(-1)
                                    s = arg.attrib["size"]
                                event_arg_obj = EventArg(n, t, s)
                                for arg_tag in arg:
                                    # only valid tag in event args is comment
                                    if arg_tag.tag == "comment":
                                        event_arg_obj.set_comment(arg_tag.text.strip())
                                    elif arg_tag.tag == "enum" and t == "ENUM":
                                        en = arg_tag.attrib["name"]
                                        enum_members = []
                                        for mem in arg_tag:
                                            mn = mem.attrib["name"]
                                            if "value" in list(mem.attrib.keys()):
                                                v = mem.attrib["value"]
                                            else:
                                                v = None
                                            if "comment" in list(mem.attrib.keys()):
                                                mc = mem.attrib["comment"].strip()
                                            else:
                                                mc = None
                                            enum_members.append((mn, v, mc))
                                        event_arg_obj.set_type(((t, en), enum_members))
                                    else:
                                        PRINT.info(
                                            "%s: Invalid argument tag %s in event %s argument %s"
                                            % (
                                                xml_file,
                                                arg_tag.tag,
                                                event.attrib["name"],
                                                n,
                                            )
                                        )
                                        sys.exit(-1)
                                event_obj.add_arg(event_arg_obj)
                        else:
                            PRINT.info(
                                "%s: Invalid tag %s in event definition"
                                % (xml_file, event_tag.tag)
                            )
                            sys.exit(-1)
                    self.__events.append(event_obj)
                if len(self.__events) > 0:
                    has_events = True
                else:
                    PRINT.info("Warning: No events defined within events tag")
            elif comp_tag.tag == "parameters":  # parse parameters
                if "parameter_base" in list(comp_tag.attrib.keys()):
                    parameter_base = self.__eval_var(
                        constants_file, "Component", comp_tag.attrib["parameter_base"]
                    )
                    parameter_bases = parameter_base.split(",")
                    # Check number of instances
                    if self.__instances is None:
                        self.__instances = len(parameter_bases)
                    else:
                        if len(parameter_bases) != self.__instances:
                            PRINT.info(
                                "%s: Component has mismatched parameter instances: %d vs. %d"
                                % (xml_file, len(opcode_bases), self.__instances)
                            )
                            sys.exit(-1)

                    # convert text bases to integers
                    parameter_base_list = []
                    for base in parameter_bases:
                        parameter_base_list.append(int(base, base=0))
                else:
                    parameter_bases = None

                # see if parameter command opcode base is specified
                opcode_base_list = []
                if "opcode_base" in list(comp_tag.attrib.keys()):
                    opcode_base = self.__eval_var(
                        constants_file, "Component", comp_tag.attrib["opcode_base"]
                    )
                    opcode_bases = opcode_base.split(",")
                    # Check number of instances
                    if self.__instances is None:
                        self.__instances = len(opcode_bases)
                    else:
                        if len(opcode_bases) != self.__instances:
                            PRINT.info(
                                "%s: Component has mismatched command instances: %d vs. %d"
                                % (xml_file, len(opcode_bases), self.__instances)
                            )
                            sys.exit(-1)

                    # convert text bases to integers
                    for base in opcode_bases:
                        opcode_base_list.append(int(base, base=0))
                else:
                    opcode_bases = None

                for parameter in comp_tag:
                    if parameter.tag != "parameter":
                        PRINT.info(
                            "%s: Invalid tag %s in parameter definition"
                            % (xml_file, parameter.tag)
                        )
                        sys.exit(-1)
                    i = parameter.attrib["id"]
                    # check to see if there is a base id specified
                    parameter_id_list = []
                    if parameter_bases is not None:
                        for base in parameter_base_list:
                            parameter_id_list.append("0x%X" % (int(i, base=0) + base))
                    else:
                        parameter_id_list.append(i)

                    n = parameter.attrib["name"]
                    d = parameter.attrib["data_type"]

                    setop = parameter.attrib["set_opcode"]
                    set_opcode_list = []
                    if opcode_bases is not None:
                        # walk through opcode base list
                        for base in opcode_base_list:
                            set_opcode_list.append("0x%X" % (int(setop, base=0) + base))
                    else:
                        set_opcode_list.append(setop)

                    saveop = parameter.attrib["save_opcode"]
                    save_opcode_list = []
                    if opcode_bases is not None:
                        # walk through opcode base list
                        for base in opcode_base_list:
                            save_opcode_list.append(
                                "0x%X" % (int(saveop, base=0) + base)
                            )
                    else:
                        save_opcode_list.append(saveop)

                    s = None
                    if d == "string":
                        if not "size" in list(parameter.attrib.keys()):
                            PRINT.info(
                                "%s: Parameter %s string value must specify a size"
                                % (xml_file, n)
                            )
                            sys.exit(-1)
                        s = parameter.attrib["size"]
                    if "default" in list(parameter.attrib.keys()):
                        f = parameter.attrib["default"]
                    else:
                        f = None
                    parameter_obj = Parameter(
                        parameter_id_list,
                        n,
                        d,
                        set_opcode_list,
                        save_opcode_list,
                        f,
                        s,
                        base_setop=setop,
                        base_saveop=saveop,
                    )
                    for parameter_tag in parameter:
                        if parameter_tag.tag == "comment":
                            parameter_obj.set_comment(parameter_tag.text.strip())
                        elif parameter_tag.tag == "enum" and d == "ENUM":
                            en = parameter_tag.attrib["name"]
                            enum_members = []
                            for mem in parameter_tag:
                                mn = mem.attrib["name"]
                                if "value" in list(mem.attrib.keys()):
                                    v = mem.attrib["value"]
                                else:
                                    v = None
                                if "comment" in list(mem.attrib.keys()):
                                    mc = mem.attrib["comment"].strip()
                                else:
                                    mc = None
                                enum_members.append((mn, v, mc))
                            parameter_obj.set_type(((d, en), enum_members))
                        else:
                            PRINT.info(
                                "%s: Invalid tag %s in parameter %s"
                                % (xml_file, comment.tag, n)
                            )
                            sys.exit(-1)
                    self.__parameters.append(parameter_obj)
                if len(self.__parameters) > 0:
                    has_parameters = True
                else:
                    PRINT.info("Warning: No parameters defined within parameters tag")
            elif (
                comp_tag.tag == "internal_interfaces"
            ):  # parse interfaces, which are internal messages
                if comp_kind == "passive":
                    PRINT.error(
                        "%s: Component %s must be active or queued to have internal interfaces"
                        % (xml_file, component_name)
                    )
                    sys.exit(-1)
                for internal_interface in comp_tag:
                    if internal_interface.tag != "internal_interface":
                        PRINT.info(
                            "%s: Invalid tag %s in internal interface definition"
                            % (xml_file, internal_interface.tag)
                        )
                        sys.exit(-1)
                    n = internal_interface.attrib["name"]
                    if "priority" in list(internal_interface.attrib.keys()):
                        p = internal_interface.attrib["priority"]
                    else:
                        p = 0

                    if "full" in list(internal_interface.attrib.keys()):
                        f = internal_interface.attrib["full"]
                        if f not in ["drop", "assert", "block"]:
                            err = '%s: Invalid attribute value "%s" for "full" in internal_interface "%s" definition. Should be one of "drop", "assert", or "block"'
                            PRINT.info(err % (xml_file, f, n))
                            sys.exit(-1)
                    else:
                        f = "assert"

                    internal_interface_obj = InternalInterface(n, p, f, None)
                    for internal_interface_tag in internal_interface:
                        # parse interface tags
                        if internal_interface_tag.tag == "comment":
                            internal_interface_obj.set_comment(
                                internal_interface_tag.text.strip()
                            )
                        elif internal_interface_tag.tag == "args":
                            for arg in internal_interface_tag:
                                if arg.tag != "arg":
                                    PRINT.info(
                                        "%s: Invalid tag %s in internal_interface args definition"
                                        % (xml_file, arg.tag)
                                    )
                                    sys.exit(-1)
                                n = arg.attrib["name"]
                                t = arg.attrib["type"]
                                if t == "string":
                                    s = arg.attrib["size"]
                                else:
                                    s = None
                                internal_interface_arg_obj = InternalInterfaceArg(
                                    n, t, s
                                )
                                for arg_tag in arg:
                                    # only valid tag in interface args is comment
                                    if arg_tag.tag == "comment":
                                        internal_interface_arg_obj.set_comment(
                                            arg_tag.text
                                        )
                                    elif arg_tag.tag == "enum" and t == "ENUM":
                                        en = arg_tag.attrib["name"]
                                        enum_members = []
                                        for mem in arg_tag:
                                            mn = mem.attrib["name"]
                                            if "value" in list(mem.attrib.keys()):
                                                v = mem.attrib["value"]
                                            else:
                                                v = None
                                            if "comment" in list(mem.attrib.keys()):
                                                mc = mem.attrib["comment"].strip()
                                            else:
                                                mc = None
                                            enum_members.append((mn, v, mc))
                                        internal_interface_arg_obj.set_type(
                                            ((t, en), enum_members)
                                        )
                                    else:
                                        PRINT.info(
                                            "%s: Invalid argument tag %s in internal_interface %s argument %s"
                                            % (
                                                xml_file,
                                                arg_tag.tag,
                                                internal_interface_tag.tag,
                                                n,
                                            )
                                        )
                                        sys.exit(-1)
                                internal_interface_obj.add_arg(
                                    internal_interface_arg_obj
                                )
                        else:
                            PRINT.info(
                                "%s: Invalid tag %s in internal_interface definition"
                                % (xml_file, internal_interface_tag.tag)
                            )
                            sys.exit(-1)
                    self.__internal_interfaces.append(internal_interface_obj)
                ## Check if there was at least 1 interface
                if len(self.__internal_interfaces) == 0:
                    PRINT.info(
                        "Warning: No interfaces defined within the 'internal_interfaces' tag "
                    )
            else:
                PRINT.info(
                    "%s: Invalid tag %s in component definition"
                    % (xml_file, comp_tag.tag)
                )
                sys.exit(-1)

        ## Add implicit ports to port list if no ports were defined
        ## Continue if all required ports are defined
        ## Abort if required ports are defined, but there are ports missing

        ## Required if commands OR parameters are defined
        cmd_or_param = {"Cmd": False, "CmdResponse": False, "CmdRegistration": False}

        ## Required if parameters are defined
        param = {"ParamGet": False, "ParamSet": False}

        ## Required if telemetry is defined
        Telemetry = False

        ## Required if telemetry OR events are defined
        Time = False

        ## Required if Events are defined
        events = {"LogEvent": False, "LogTextEvent": False}

        ## Check ports
        for port in self.__ports:
            ## Check for Cmd, CmdResponse, and CmdRegistration ports
            if has_commands or has_parameters:
                if port.get_role() == "Cmd":
                    cmd_or_param["Cmd"] = True
                if port.get_role() == "CmdResponse":
                    cmd_or_param["CmdResponse"] = True
                if port.get_role() == "CmdRegistration":
                    cmd_or_param["CmdRegistration"] = True

            ## Check for Param(Get|Set)
            if has_parameters:
                if port.get_role() == "ParamGet":
                    param["ParamGet"] = True
                if port.get_role() == "ParamSet":
                    param["ParamSet"] = True

            ## Check for Telemetry
            if has_telemetry:
                if port.get_role() == "Telemetry":
                    Telemetry = True

            ## Check for Time
            if has_telemetry or has_events:
                if port.get_role() == "TimeGet":
                    Time = True

            ## Check Log(Text)Event
            if has_events:
                if port.get_role() == "LogEvent":
                    events["LogEvent"] = True
                if port.get_role() == "LogTextEvent":
                    events["LogTextEvent"] = True

        ## Add implicit ports to port list and port type list

        ## Required command and parameter ports
        if has_commands or has_parameters:
            ## All ports exist
            if (
                cmd_or_param["Cmd"]
                and cmd_or_param["CmdResponse"]
                and cmd_or_param["CmdRegistration"]
            ):
                pass
            ## None of the ports exist: Add them to portlist
            elif not (
                cmd_or_param["Cmd"]
                or cmd_or_param["CmdResponse"]
                or cmd_or_param["CmdRegistration"]
            ):
                Cmd = self.__generate_port_from_role("Cmd")
                CmdResponse = self.__generate_port_from_role("CmdResponse")
                CmdRegistration = self.__generate_port_from_role("CmdRegistration")

                implicitPortList = [Cmd, CmdResponse, CmdRegistration]

                ## Add to port list
                self.__ports.extend(implicitPortList)
                ## Add port files to import_port_type_files
                self.__add_to_import_port_list(implicitPortList)

            ## Ports Missing: Aborting
            else:
                for port, value in cmd_or_param.items():
                    if not value:
                        PRINT.info("%s port missing" % port)
                PRINT.info("Aborting")
                sys.exit(-1)

        ## Required parameter ports
        if has_parameters:
            ## All ports exist
            if param["ParamGet"] and param["ParamSet"]:
                pass
            ## None of the ports exist: They must be added to portlist
            elif not (param["ParamGet"] or param["ParamSet"]):
                ParamGet = self.__generate_port_from_role("ParamGet")
                ParamSet = self.__generate_port_from_role("ParamSet")

                implicitPorts = [ParamGet, ParamSet]

                self.__ports.extend(implicitPorts)
                self.__add_to_import_port_list(implicitPorts)

            ## Ports Missing: Abort
            else:
                for port, value in param.items():
                    if not value:
                        PRINT.info("%s port missing" % port)
                PRINT.info("Aborting")
                sys.exit(-1)

        ## Required telemetry ports
        #
        ## Ports exists
        if has_telemetry:
            if Telemetry:
                pass
            ## Port does not exist: Add to portlist
            else:
                Telemetry = self.__generate_port_from_role("Telemetry")

                implicitPorts = [Telemetry]

                self.__ports.extend(implicitPorts)
                self.__add_to_import_port_list(implicitPorts)

        ## Required telemetry and event ports
        #
        ## Port exists
        if has_telemetry or has_events:
            if Time:
                pass
            ## Port does not exist: Add to portlist
            else:
                Time = self.__generate_port_from_role("TimeGet")

                implicitPorts = [Time]

                self.__ports.extend(implicitPorts)
                self.__add_to_import_port_list(implicitPorts)

        ## Required event ports
        if has_events:
            implicitPorts = []
            if not events["LogEvent"]:
                LogEvent = self.__generate_port_from_role("LogEvent")
                implicitPorts += [LogEvent]
            if not events["LogTextEvent"]:
                LogTextEvent = self.__generate_port_from_role("LogTextEvent")
                implicitPorts += [LogTextEvent]
            if len(implicitPorts) > 0:
                self.__ports.extend(implicitPorts)
                self.__add_to_import_port_list(implicitPorts)

        for p in self.__ports:
            t = p.get_type()
            if "::" in t:
                # PRINT.info("WARNING: Found namespace qualifier in port type definition (name=%s, type=%s) using namespace specified in XXXPortAi.xml file." % (n,t))
                p.set_type(t.split("::")[-1])

    def __generate_port_from_role(self, role):

        special_ports = self.Config._ConfigManager__prop["special_ports"]

        n = special_ports[role]["name"]
        t = special_ports[role]["type"]
        d = special_ports[role]["direction"]
        s = None
        c = special_ports[role]["comment"]
        m = 1

        p = Port(n, d, t, sync=s, role=role, comment=c, max=m)

        return p

    def __add_to_import_port_list(self, implicitPortList):
        for port in implicitPortList:
            port_file = self.special_ports[port.get_role()]["port_file"]
            self.__import_port_type_files.append(port_file)

    def __eval_var(self, file, section, var):
        # print "Check %s:%s"%(section,var)
        # check to see if variable token
        if var[0] != "$":
            return var
        # make sure file was read in if a variable is defined
        if self.__const_parser is None:
            PRINT.info(
                "%s: Variable %s defined with no constants file %s"
                % (
                    file,
                    var,
                    self.Config._ConfigManager__prop["constants"]["constants_file"],
                )
            )
            sys.exit(-1)
        # check for more than one $
        if var.count("$") > 1:
            PRINT.info("{}: Invalid variable {}".format(file, var))
            sys.exit(-1)
        # try to find variable
        if not section in self.__const_parser.sections():
            PRINT.info(
                "%s: Config file %s has no %s variables"
                % (
                    file,
                    self.Config._ConfigManager__prop["constants"]["constants_file"],
                    section,
                )
            )
            sys.exit(-1)
        # remove $
        var = var[1:]
        if not self.__const_parser.has_option(section, var):
            PRINT.info(
                "%s: Config file %s has no variable %s in section %s"
                % (
                    file,
                    self.Config._ConfigManager__prop["constants"]["constants_file"],
                    var,
                    section,
                )
            )
            sys.exit(-1)
        # Python 3's SafeConfigParser isn't stripping comments, this fixes that problem
        return self.__const_parser.get(section, var).split(";")[0]

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
                raise FprimeXmlException(msg)

    def is_component(self):
        """
        Returns true if component xml or false if other type.
        """
        return self.__is_component_xml

    def get_xml_filename(self):
        """
        Return the original XML filename parsed.
        """
        return self.__xml_filename

    def get_port_type_files(self):
        """
        Return a list of all imported Port type XML files.
        """
        return self.__import_port_type_files

    def get_serializable_type_files(self):
        """
        Return a list of all imported serializable type XML files.
        """
        return self.__import_serializable_type_files

    def get_enum_type_files(self):
        """
        Return a list of all imported enum type XML files.
        """
        return self.__import_enum_type_files

    def get_array_type_files(self):
        """
        Return a list of all imported array type XML files.
        """
        return self.__import_array_type_files

    def get_imported_dictionary_files(self):
        """
        Return a list of all imported dictionary XML files.
        """
        return self.__import_dictionary_files

    def get_header_files(self):
        """
        Return a list of custom headers to be included.
        """
        return self.__include_header_files

    def get_component(self):
        """
        Returns a component object.
        """
        return self.__component

    def get_ports(self):
        """
        Returns a list of port objects with all text and attrib needed.
        """
        return self.__ports

    def get_commands(self):
        """
        Returns a list of command objects with all text and attrib needed.
        """
        return self.__commands

    def get_internal_interfaces(self):
        """
        Returns a list of interface objects with all text and attrib needed.
        """
        return self.__internal_interfaces

    def get_channels(self):
        """
        Returns a list of channel objects with all text and attrib needed.
        """
        return self.__channels

    def get_parameters(self):
        """
        Returns a list of parameter objects with all text and attrib needed.
        """
        return self.__parameters

    def get_events(self):
        """
        Returns a list of event objects with all text and attrib needed.
        """
        return self.__events


class Component:
    """
    Data container for a component.
    """

    def __init__(
        self, namespace=None, name=None, kind=None, comment=None, modeler=False
    ):
        """
        Constructor
        """
        self.__namespace = namespace
        self.__name = name
        self.__kind = kind
        self.__comment = comment
        self.__modeler = modeler

    def get_namespace(self):
        return self.__namespace

    def get_name(self):
        return self.__name

    def get_kind(self):
        return self.__kind

    def get_comment(self):
        return self.__comment

    def get_modeler(self):
        return self.__modeler

    def set_comment(self, comment):
        self.__comment = comment


class Port:
    """
    Data container for all the port name, type, etc. associated with component.
    """

    def __init__(
        self,
        name,
        direction,
        type,
        sync=None,
        priority=None,
        full=None,
        comment=None,
        max=None,
        role=None,
    ):
        """
        Constructor
        @param name:  Name of port (each instance must be unique).
        @param direction: Direction of data flow (must be input or output)
        @param type:  Type of port (must have supporting include xml)
        @param sync:  Kind of port (must be one of: async, sync, or guarded)
        @param comment:  A single or multiline comment
        """
        # TODO: ADD NAMESPACE
        self.__name = name
        self.__direction = direction
        self.__type = type
        self.__sync = sync
        self.__priority = priority
        self.__full = full
        self.__comment = comment
        self.__max_number = max
        self.__role = role

    def get_role(self):
        return self.__role

    def set_role(self, role):
        self.__role = role

    def get_name(self):
        return self.__name

    def get_direction(self):
        return self.__direction

    def get_type(self):
        return self.__type

    def set_type(self, t):
        self.__type = t

    def get_sync(self):
        return self.__sync

    def get_priority(self):
        return self.__priority

    def get_full(self):
        return self.__full

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def get_max_number(self):
        return self.__max_number


class CommandArg:
    """
    Data container for a command argument
    """

    def __init__(self, name, type, size=None, comment=None):
        self.__name = name
        self.__type = type
        self.__size = size
        self.__comment = comment

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_size(self):
        return self.__size

    def set_type(self, type):
        self.__type = type

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment


class Command:
    """
    Data container for all the mnemonic, opcode, etc. associated with a command.
    """

    def __init__(
        self,
        mnemonic,
        opcodes,
        sync=None,
        priority=None,
        comment=None,
        base_opcode=None,
        full=None,
    ):
        """
        Constructor
        @param mnemonic:  command mnemonic (each instance must be unique).
        @param opcodes: opcode of command (must be unique)
        @param sync:  Kind of port (must be one of: async, sync, or guarded)
        @param priority: Priority of command message if async
        @param comment:  A single or multiline comment
        @param base_opcode: An int which represents the command
        """

        self.__mnemonic = mnemonic
        self.__opcodes = opcodes
        self.__sync = sync
        self.__priority = priority
        self.__full = full
        self.__args = []
        self.__comment = comment
        self.__base_opcode = base_opcode

    def get_mnemonic(self):
        return self.__mnemonic

    def get_opcodes(self):
        return self.__opcodes

    def get_sync(self):
        return self.__sync

    def get_priority(self):
        return self.__priority

    def get_full(self):
        return self.__full

    def get_args(self):
        return self.__args

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def add_arg(self, arg):
        self.__args += (arg,)

    def get_base_opcode(self):
        return self.__base_opcode


class InternalInterfaceArg:
    """
    Data container for an interface argument
    """

    def __init__(self, name, type, size=None, comment=None):
        self.__name = name
        self.__type = type
        self.__size = size
        self.__comment = comment

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_size(self):
        return self.__size

    def set_type(self, type):
        self.__type = type

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment


class InternalInterface:
    """
    Data container for all the arguments, etc associated with an internal interface.
    """

    def __init__(self, name, priority=None, full=None, comment=None):
        """
        Constructor
        @param name:  interface name (each instance must be unique).
        @param priority:  Priority of interface message
        @param comment:  A single or multiline comment
        """

        self.__name = name
        self.__priority = priority
        self.__full = full
        self.__args = []
        self.__comment = comment

    def get_name(self):
        return self.__name

    def get_priority(self):
        return self.__priority

    def get_full(self):
        return self.__full

    def get_args(self):
        return self.__args

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def add_arg(self, arg):
        self.__args += (arg,)


class Channel:
    """
    Data container for a telemetry channel definition.
    """

    def __init__(
        self,
        ids,
        name,
        type,
        size=None,
        abbrev=None,
        format_string=None,
        update="always",
        limits=(None, None, None, None, None, None),
        comment=None,
    ):
        """
        Constructor
        @param id:  numeric channel id
        @param name: name of command
        @param type:  Type of command (must have supporting include xml)
        @param abbrev: Channel abbreviation (to support AMPCS)
        @param size: size, if type is string
        @param format_string: represents the format string to display the value
        @param update: "always" or "on_change" to signify if written always or only when a value changes
        @param limits: a tuple of (low_red, low_yellow, high_yellow, high_red) which specifies warning levels for telemetry. 'None' when not specified
        @param comment:  A single or multiline comment
        """

        self.__ids = ids
        self.__name = name
        self.__type = type
        self.__abbrev = abbrev
        self.__size = size
        self.__format_string = format_string
        self.__update = update
        self.__limits = limits
        self.__comment = comment
        # A list of unit conversions for each channel
        self.__units = []

    def get_ids(self):
        return self.__ids

    def get_name(self):
        return self.__name

    def get_abbrev(self):
        return self.__abbrev

    def set_abbrev(self, new_abbrev):
        self.__abbrev = new_abbrev

    def get_type(self):
        return self.__type

    def set_type(self, type):
        self.__type = type

    def get_size(self):
        return self.__size

    def get_comment(self):
        return self.__comment

    def get_format_string(self):
        return self.__format_string

    def get_update(self):
        return self.__update

    def get_limits(self):
        return self.__limits

    def set_comment(self, comment):
        self.__comment = comment

    def get_units(self):
        return self.__units

    def set_units(self, name, gain, offset):
        self.__units.append((name, gain, offset))


class Parameter:
    """
    Data container for a parameter type definition.
    """

    def __init__(
        self,
        ids,
        name,
        type,
        set_opcodes,
        save_opcodes,
        default,
        size=None,
        comment=None,
        base_setop=None,
        base_saveop=None,
    ):
        """
        Constructor
        @param id:  numeric channel id
        @param name: name of command
        @param type:  Type of command (must have supporting include xml)
        @param comment:  A single or multiline comment
        """

        self.__ids = ids
        self.__name = name
        self.__type = type
        self.__set_opcodes = set_opcodes
        self.__save_opcodes = save_opcodes
        self.__default = default
        self.__size = size
        self.__comment = comment
        self.__base_setop = base_setop
        self.__base_saveop = base_saveop

    def get_ids(self):
        return self.__ids

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def get_set_opcodes(self):
        return self.__set_opcodes

    def get_save_opcodes(self):
        return self.__save_opcodes

    def set_type(self, type):
        self.__type = type

    def get_size(self):
        return self.__size

    def get_default(self):
        return self.__default

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def get_base_setop(self):
        return self.__base_setop

    def get_base_saveop(self):
        return self.__base_saveop


class EventArg:
    """
    Data container for an event argument
    """

    def __init__(self, name, type, size=None, comment=None):
        self.__name = name
        self.__type = type
        self.__comment = comment
        self.__size = size

    def get_name(self):
        return self.__name

    def get_type(self):
        return self.__type

    def set_type(self, type):
        self.__type = type

    def get_size(self):
        return self.__size

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment


class Event:
    """
    Data Container for an event
    """

    def __init__(self, ids, name, severity, format_string, throttle=None, comment=None):
        """
        Constructor
        @param id:  numeric event id
        @param name: name of event
        @param severity:  Severity of event, one of DEBUG, INFO, WARNING, FATAL
        @param format_string: format string for displaying event arguments
        """
        self.__ids = ids
        self.__name = name
        self.__severity = severity
        self.__format_string = format_string
        self.__throttle = throttle
        self.__args = []
        self.__comment = comment

    def get_ids(self):
        return self.__ids

    def get_name(self):
        return self.__name

    def get_severity(self):
        return self.__severity

    def get_format_string(self):
        return self.__format_string

    def get_throttle(self):
        return self.__throttle

    def get_args(self):
        return self.__args

    def get_comment(self):
        return self.__comment

    def set_comment(self, comment):
        self.__comment = comment

    def add_arg(self, arg):
        self.__args += (arg,)


if __name__ == "__main__":
    xmlfile = sys.argv[1]

    print("Component XML parse test (%s)" % xmlfile)

    xml_component_parser = XmlComponentParser(xmlfile)

    comp = xml_component_parser.get_component()
    port_import_list = xml_component_parser.get_port_type_files()
    port_list = xml_component_parser.get_ports()

    print(
        "Namespace: %s Component name: %s Kind: %s"
        % (comp.get_namespace(), comp.get_name(), comp.get_kind())
    )
    print("Component comment:")
    print(comp.get_comment())
    print()
    print("Ports:")
    for port in port_list:
        print(
            "Name: %s, Direction: %s, Type: %s, Sync: %s"
            % (port.get_name(), port.get_direction(), port.get_type(), port.get_sync())
        )
        print("Port comment:")
        print(port.get_comment())
    print()
    print("Imports:")
    for i in port_import_list:
        print("\t%s" % i)
    print()
