#!/usr/bin/env python
# ===============================================================================
# NAME: tlm_packet_gen.py
#
# DESCRIPTION: A tool for generating telemetry packet data structures for use with Svc/TlmPacketizer.
#              Logic borrowed from gds_dictgen.py
#
# AUTHOR: Tim Canham
# EMAIL:  timothy.canham@jpl.nasa.gov
# DATE CREATED: September 8, 2019
#
# Copyright 2019, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================

import logging
import os
import sys
from optparse import OptionParser

from Cheetah.Template import Template

# Meta-model for Component only generation
from fprime_ac.models import TopoFactory

# Parsers to read the XML
from fprime_ac.parsers import (
    XmlArrayParser,
    XmlEnumParser,
    XmlSerializeParser,
    XmlTopologyParser,
)
from fprime_ac.utils.buildroot import (
    get_nearest_build_root,
    search_for_file,
    set_build_roots,
)
from lxml import etree

header_file_template = """

\#ifndef ${packet_list_name}_header_h
\#define ${packet_list_name}_header_h

\#include <Svc/TlmPacketizer/TlmPacketizerTypes.hpp>

namespace ${packet_list_namespace} {

    // set of packets to send
    extern const Svc::TlmPacketizerPacketList ${packet_list_name}Pkts;
    // set of channels to ignore
    extern const Svc::TlmPacketizerPacket ${packet_list_name}Ignore;

}

\#endif // ${packet_list_name}_header_h

"""

impl_file_template = """

\#include <Svc/TlmPacketizer/TlmPacketizerTypes.hpp>
\#include <${output_header}>

\#include <FpConfig.hpp>
\#include <FpConfig.hpp>
\#include <Fw/Time/Time.hpp>

// Verify packets not too large for ComBuffer
// if this macro gives a compile error, that means the packets are too large

void check_func(void) {
    static_assert((${max_size} <= (FW_COM_BUFFER_MAX_SIZE - Fw::Time::SERIALIZED_SIZE - sizeof(FwTlmPacketizeIdType) - sizeof(FwPacketDescriptorType))), "PacketsTooBig");
}

namespace ${packet_list_namespace} {

#for $packet,$id,$level,$channel_list in $packet_list
  static const Svc::TlmPacketizerChannelEntry ${packet}List[] = {
  #for $channel_id,$channel_size,$channel_name in $channel_list:
      {$channel_id, $channel_size}, // $channel_name
  #end for
  };

  static const Svc::TlmPacketizerPacket ${packet} = { ${packet}List, $id, $level, FW_NUM_ARRAY_ELEMENTS(${packet}List) };

#end for


  const Svc::TlmPacketizerPacketList ${packet_list_name}Pkts = {
      {
#for $packet,$id,$level,$channel_list in $packet_list
         &${packet},
#end for
      },
      $len($packet_list)
  };


  static const Svc::TlmPacketizerChannelEntry ignoreList[] = {
#for $channel_id,$channel_name in $ignore_list:
      {$channel_id, 0}, // $channel_name
#end for
  };

  const Svc::TlmPacketizerPacket ${packet_list_name}Ignore = { ignoreList, 0, 0, FW_NUM_ARRAY_ELEMENTS(ignoreList) };

} // end namespace ${packet_list_namespace}

"""


# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

PACKET_VIEW_DIR = "./Packet-Views"

STRING_BUFFER_SIZE = 2  # Size of string buffer in bytes
ENUM_BASE_SIZE = 4  # Base size of enumeration in bytes


class TlmPacketParseValueError(ValueError):
    pass


class TlmPacketParseIOError(ValueError):
    pass


class TlmPacketParser(object):
    def __init__(self, verbose=False, dependency=None):
        self.verbose = verbose
        self.dependency = dependency
        self.size_dict = {}

    def add_type_size(self, type, size):
        PRINT.debug("Type: %s size: %d" % (type, size))
        self.size_dict[type] = size

    def get_type_size(self, type_name, size):
        # switch based on type
        if type_name in ("I16", "U16"):
            return 2  # 2 bytes for 16 bit types
        elif type_name in ("I32", "U32", "F32"):
            return 4  # 4 bytes for 32 bit types
        elif type_name in ("I64", "U64", "F64"):
            return 8  # 8 bytes for 64 bit types
        elif type_name in ("I8", "U8", "bool"):
            return 1  # 1 byte for 8 bit types
        elif type_name == "string":
            return int(size) + STRING_BUFFER_SIZE  # String size + buffer size in bytes
        else:
            return None

    def generate_channel_size_dict(self, the_parsed_topology_xml, xml_filename):
        """
        Generates GDS XML dictionary from parsed topology XML
        """
        if self.verbose:
            print(f"Topology xml type description file: {xml_filename}")
        model = TopoFactory.TopoFactory.getInstance()
        topology_model = model.create(the_parsed_topology_xml, generate_list_file=False)

        # uses the topology model to process the items
        # create list of used parsed component xmls
        parsed_xml_dict = {}
        for comp in the_parsed_topology_xml.get_instances():
            if comp.get_type() in topology_model.get_base_id_dict():
                parsed_xml_dict[comp.get_type()] = comp.get_comp_xml()
            else:
                PRINT.info(
                    f"Components with type {comp.get_type()} aren't in the topology model."
                )

        xml_list = []
        for parsed_xml_type, value in parsed_xml_dict.items():
            if value is None:
                print(
                    f"ERROR: XML of type {parsed_xml_type} is being used, but has not been parsed correctly. Check if file exists or add xml file with the 'import_component_type' tag to the Topology file."
                )
                raise Exception()
            xml_list.append(parsed_xml_dict[parsed_xml_type])

        topology_model.set_instance_xml_list(xml_list)

        ch_size_dict = {}

        for comp in the_parsed_topology_xml.get_instances():
            comp_name = comp.get_name()
            comp_id = int(comp.get_base_id(), 0)
            comp_type = comp.get_type()
            if self.verbose:
                PRINT.debug(f"Processing {comp_name}")

            # check for included XML types
            self.process_enum_files(parsed_xml_dict[comp_type].get_enum_type_files())
            self.process_array_files(parsed_xml_dict[comp_type].get_array_type_files())
            self.process_serializable_files(
                parsed_xml_dict[comp_type].get_serializable_type_files()
            )

            # check for channels
            if parsed_xml_dict[comp_type].get_channels() is not None:
                for chan in parsed_xml_dict[comp_type].get_channels():
                    channel_name = f"{comp_name}.{chan.get_name()}"
                    if self.verbose:
                        print(f"Processing Channel {channel_name}")
                    chan_type = chan.get_type()
                    # if channel is enum
                    if isinstance(chan_type, tuple):
                        chan_size = ENUM_BASE_SIZE

                    # if channel type is string
                    #                    elif chan_type == "string":
                    #                        chan_size = int(chan.get_size()) + STRING_BUFFER_SIZE # String size + buffer size in bytes
                    # if channel is serializable
                    elif chan_type in self.size_dict:
                        chan_size = self.size_dict[chan_type]
                    else:
                        chan_size = self.get_type_size(chan_type, chan.get_size())
                    if chan_size is None:
                        print(
                            f'Component {comp_name} channel {channel_name} type "{chan_type}" not found!'
                        )
                        sys.exit(-1)
                    chan_id = int(chan.get_ids()[0], 0) + comp_id
                    if self.verbose:
                        print("Channel %s size %d" % (channel_name, chan_size))
                    ch_size_dict[channel_name] = (chan_id, chan_size)

        return ch_size_dict

    def gen_packet_file(self, xml_filename):
        view_path = PACKET_VIEW_DIR

        if not os.path.exists(view_path):
            os.mkdir(view_path)

        # Make sure files
        if not os.path.isfile(xml_filename):
            raise TlmPacketParseIOError(f"File {xml_filename} does not exist!")

        fd = open(xml_filename, "r")
        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd, parser=xml_parser)
        channel_size_dict = None

        ht = Template(header_file_template)
        it = Template(impl_file_template)

        if element_tree.getroot().tag != "packets":
            raise TlmPacketParseValueError(
                f"Invalid xml type {element_tree.getroot().tag}"
            )

        list_name = element_tree.getroot().attrib["name"]
        list_namespace = element_tree.getroot().attrib["namespace"]
        max_size = int(element_tree.getroot().attrib["size"])

        # fill in template fields for header
        ht.packet_list_name = list_name
        ht.packet_list_namespace = list_namespace

        # fill in template fields for implementation file
        it.packet_list_name = list_name
        it.packet_list_namespace = list_namespace
        it.max_size = max_size

        packet_list_container = []

        packetized_channel_list = []
        it.ignore_list = []
        id_list = []  # check for duplicates
        ignore_name_list = []

        size_dict = {}

        ht.num_packets = 0
        total_packet_size = 0
        levels = []
        view_path = PACKET_VIEW_DIR
        # find the topology import
        for entry in element_tree.getroot():
            # read in topology file
            if entry.tag == "import_topology":
                top_file = search_for_file("Topology", entry.text)
                if top_file is None:
                    raise TlmPacketParseIOError(f"import file {entry.text} not found")
                the_parsed_topology_xml = XmlTopologyParser.XmlTopologyParser(top_file)
                deployment = the_parsed_topology_xml.get_deployment()
                if self.verbose:
                    print("Found assembly or deployment named: %s\n" % deployment)
                channel_size_dict = self.generate_channel_size_dict(
                    the_parsed_topology_xml, xml_filename
                )
            elif entry.tag == "packet":
                if channel_size_dict is None:
                    raise TlmPacketParseValueError(
                        f"{xml_filename}: Topology import must be before packet definitions"
                    )
                packet_size = 0
                packet_name = entry.attrib["name"]
                with open(f"{view_path}/{packet_name}.txt", "w") as vfd:
                    packet_id = entry.attrib["id"]
                    packet_level = entry.attrib["level"]
                    print(f"Packetizing {packet_name} ({packet_id})")
                    if packet_id in id_list:
                        raise TlmPacketParseValueError(
                            f"Duplicate packet id {packet_id}"
                        )
                    else:
                        id_list.append(packet_id)

                    channel_list = []
                    for channel in entry:
                        channel_name = channel.attrib["name"]
                        if channel_name not in channel_size_dict:
                            raise TlmPacketParseValueError(
                                f"Channel {channel_name} does not exist"
                            )
                        (channel_id, channel_size) = channel_size_dict[channel_name]
                        packet_size += channel_size
                        if self.verbose:
                            print(
                                f" -Channel {channel_name} ID {channel_id} size {channel_size}"
                            )
                        channel_list.append((channel_id, channel_size, channel_name))
                        packetized_channel_list.append(channel_name)
                        vfd.write("%s\n" % channel_name)
                    packet_list_container.append(
                        (packet_name, packet_id, packet_level, channel_list)
                    )
                    ht.num_packets += 1
                    packet_size += (
                        11 + 2 + 4
                    )  # raw packet size + time tag + packet id + packet descriptor
                    if packet_size > max_size:
                        raise TlmPacketParseValueError(
                            "Packet %s is too large. Size: %d max: %d"
                            % (packet_name, packet_size, max_size)
                        )
                    print("Packet %s size %d/%d" % (packet_name, packet_size, max_size))
                    total_packet_size += packet_size

                    size_dict[packet_level] = (
                        size_dict[packet_level] + packet_size
                        if packet_level in size_dict
                        else packet_size
                    )
                    if packet_level not in levels:
                        levels.append(packet_level)
            elif entry.tag == "ignore":
                if channel_size_dict is None:
                    raise TlmPacketParseValueError(
                        f"{xml_filename}: Topology import must be before packet definitions"
                    )
                for channel in entry:
                    channel_name = channel.attrib["name"]
                    if channel_name not in channel_size_dict:
                        raise TlmPacketParseValueError(
                            f"Channel {channel_name} does not exist"
                        )
                    (channel_id, channel_size) = channel_size_dict[channel_name]
                    it.ignore_list.append((channel_id, channel_name))
                    if self.verbose:
                        print("Channel %s (%d) ignored" % (channel_name, channel_id))
                    ignore_name_list.append(channel_name)
            else:
                raise TlmPacketParseValueError(f"Invalid packet tag {entry.tag}")

            if self.verbose:
                print(f"Entry: {entry.tag}")
        output_file_base = os.path.splitext(os.path.basename(xml_filename))[0]
        nearest_build_root = get_nearest_build_root(xml_filename)
        file_dir = os.path.relpath(os.path.dirname(xml_filename), nearest_build_root)

        missing_channels = False

        for channel in channel_size_dict:
            if (
                channel not in packetized_channel_list
                and channel not in ignore_name_list
            ):
                (channel_id, channel_size) = channel_size_dict[channel]
                print(f"Channel {channel} ({channel_id}) not packetized or ignored.")
                missing_channels = True

        if missing_channels:
            raise TlmPacketParseValueError("Channels missing from packets")

        header = f"{output_file_base}Ac.hpp"
        source = f"{output_file_base}Ac.cpp"
        print(f"Generating {header} and {source}")
        levels.sort()
        for level in levels:
            print(
                f"Level: {level} Bytes: {size_dict[level]} bits: {size_dict[level] * 8}"
            )
        print(
            f"Number of packets: {ht.num_packets}\nTotal packet bytes: {total_packet_size} bits: {total_packet_size * 8}"
        )

        it.packet_list = packet_list_container
        it.output_header = f"{file_dir}/{output_file_base}Ac.hpp"

        open(header, "w").write(str(ht))
        open(source, "w").write(str(it))

        target_directory = os.getcwd().replace("\\", os.sep)
        header_target = target_directory + os.sep + header
        source_target = target_directory + os.sep + source

        # write dependency file
        if self.dependency is not None:
            dependency_file_txt = f"\n{source_target} {header_target}: {top_file}\n"
            open(self.dependency, "w").write(dependency_file_txt)

    def process_serializable_files(self, serializable_file_list):
        for serializable_file in serializable_file_list:
            serializable_file = search_for_file("Serializable", serializable_file)
            serializable_model = XmlSerializeParser.XmlSerializeParser(
                serializable_file
            )
            # process XML includes
            self.process_enum_files(serializable_model.get_include_enums())
            self.process_array_files(serializable_model.get_include_arrays())
            self.process_serializable_files(serializable_model.get_includes())
            serializable_type = (
                serializable_model.get_namespace()
                + "::"
                + serializable_model.get_name()
            )
            serializable_size = 0
            for (
                member_name,
                member_type,
                member_array_size,
                member_size,
                member_format_specifier,
                member_comment,
                _,
            ) in serializable_model.get_members():
                # if enumeration
                if isinstance(member_type, tuple):
                    type_size = ENUM_BASE_SIZE

                elif (
                    member_type in self.size_dict.keys()
                ):  # See if it is a registered type
                    type_size = self.size_dict[member_type]
                else:
                    type_size = self.get_type_size(member_type, member_size)
                if type_size is None:
                    print(
                        f"Illegal type {member_type} in serializable {serializable_type}"
                    )
                    sys.exit(-1)
                serializable_size += type_size
                if member_array_size != None:
                    serializable_size *= member_array_size
            self.add_type_size(serializable_type, serializable_size)
            if self.verbose:
                print(f"Serializable {serializable_type} size {serializable_size}")

    def process_enum_files(self, enum_file_list):
        for enum_file in enum_file_list:
            enum_file = search_for_file("Enumeration", enum_file)
            enum_model = XmlEnumParser.XmlEnumParser(enum_file)
            enum_type = f"{enum_model.get_namespace()}::{enum_model.get_name()}"
            self.add_type_size(enum_type, ENUM_BASE_SIZE)

    def process_array_files(self, array_file_list):
        for array_file in array_file_list:
            array_file = search_for_file("Array", array_file)
            array_model = XmlArrayParser.XmlArrayParser(array_file)
            # process any XML includes
            self.process_enum_files(array_model.get_include_enum_files())
            self.process_array_files(array_model.get_include_array_files())
            self.process_serializable_files(array_model.get_includes())
            array_type = f"{array_model.get_namespace()}::{array_model.get_name()}"
            array_size = int(array_model.get_size())
            elem_type = array_model.get_type()
            elem_type_size = None
            if isinstance(elem_type, tuple):
                elem_type_size = ENUM_BASE_SIZE

            elif elem_type in self.size_dict.keys():  # See if it is a registered type
                elem_type_size = self.size_dict[elem_type]
            else:
                elem_type_size = self.get_type_size(elem_type, 1)  # FIXME: strings?
            if elem_type_size is None:
                print(f"Illegal type {elem_type} in array {array_type}")
                sys.exit(-1)
            self.add_type_size(array_type, elem_type_size * array_size)


def pinit():
    """
    Initialize the option parser and return it.
    """

    usage = "usage: %prog [options] [xml_topology_filename]"

    parser = OptionParser(usage)

    parser.add_option(
        "-b",
        "--build_root",
        dest="build_root_overwrite",
        type="string",
        help="Overwrite environment variable BUILD_ROOT",
        default=None,
    )

    parser.add_option(
        "-v",
        "--verbose",
        dest="verbose_flag",
        help="Enable verbose mode showing more runtime detail (def: False)",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-d",
        "--dependency-file",
        dest="dependency_file",
        type="string",
        help="Generate dependency file for make",
        default=None,
    )

    return parser


def main():
    """
    Main program.
    """
    global VERBOSE  # prevent local creation of variable
    global BUILD_ROOT  # environmental variable if set

    Parser = pinit()
    (opt, args) = Parser.parse_args()
    # opt.verbose_flag = True

    #
    #  Parse the input Topology XML filename
    #
    if len(args) == 0:
        print(f"Usage: {sys.argv[0]} [options] xml_filename")
        return
    elif len(args) == 1:
        xml_filename = os.path.abspath(args[0])
    else:
        print("ERROR: Too many filenames, should only have one")
        return

    print(f"Processing packet file {xml_filename}")
    set_build_roots(os.environ.get("BUILD_ROOT"))

    packet_parser = TlmPacketParser(opt.verbose_flag, opt.dependency_file)
    try:
        packet_parser.gen_packet_file(xml_filename)
    except TlmPacketParseValueError as e:
        print(f"Packet XML parsing error: {e}")
        sys.exit(-1)
    except TlmPacketParseIOError as e:
        print(f"Packet XML file error: {e}")
        sys.exit(-1)

    sys.exit(0)


if __name__ == "__main__":
    main()
