#!/usr/bin/env python3
# ===============================================================================
# NAME: TlmLayout.py
#
# DESCRIPTION:  Takes as input a telemetry layout in tab-delimited format and
#               generates .hpp of the packet layout(s) and a table that can be
#               used to form the packet(s)
#
# USAGE:
#
# AUTHOR: Len Day
# EMAIL:  len.day@jpl.nasa.gov
# DATE CREATED  : 09/28/15
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#

import argparse
import copy
import csv

from Cheetah.Template import Template

#
# Python extension modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#

tlm_input_line_num = 0
tlm_max_packet_bytes = 0
tlm_packet_list = []

tlm_duration = None
tlm_period = None
tlm_packet_id_schedule_list = []
tlm_packet_max_ids_list = []

tlm_packet_to_chan_list = []

# ===============================================================================
#


class HeaderItem:
    def __init__(self):
        self.m_type = ""
        self.m_id = 0
        self.m_bit_start = 0
        self.m_bits = 0
        self.m_comment = ""


class Item:
    def __init__(self):
        self.m_is_reserve = False
        self.m_is_constant = False
        self.m_name = ""
        self.m_id = 0
        self.m_data_type = ""
        self.m_constant_value = 0
        self.m_bit_start = 0
        self.m_bits = 0
        self.m_comment = ""


# The information for a telemetry packet is accumulated in a Packet class
# instance, then the packet_complete method is called when it is complete


class Packet:
    def init(self):
        self.m_name = ""
        self.m_id = 0
        self.m_id_comment = ""
        self.m_bit_index = 0
        self.m_bytes = 0
        self.m_num_fixed_fields = 0
        self.m_num_variable_fields = 0
        self.m_max_field_bits = 0
        self.num_header_general_fields = 0
        self.m_header_list = []
        self.m_item_list = []
        # Additional packet information needed
        # for scheduling and multiplexing to an address
        #
        self.m_chan = None
        #
        self.m_freq = None
        self.m_offset = None

    def __init__(self):
        self.init()

    # errors encountered parsing the file use err_msg to output the
    # error and exit.

    def err_msg(self, msg):
        global tlm_input_line_num
        print(msg, "at input line ", tlm_input_line_num)
        exit(1)

    def duration(self, line):
        global tlm_duration
        tlm_duration = float(line[1])

    def run_period(self, line):
        global tlm_period
        tlm_period = float(line[1])

    def frequency(self, line):
        """
        Frequency of packet is always in Hz. so no units given.
        """
        self.m_freq = float(line[1])

    def offset(self, line):
        """
        Offset of packet is essentially it's phase within duration.
        If no units detected just count cycle slots.
        If "s" for seconds is detected then compute cycle slots here.
        NOTE: One and only one offset per ID can currently be specified.
        """
        if "s" in line[1].lower():
            self.m_offset = int(
                (float(line[1].split()[0].strip()) / (1.0 / tlm_period)) + 0.5
            )
        else:
            self.m_offset = int(line[1])

    def channel(self, line):
        """
        Channel number.  This is a channel number assigned to
        the packet of type ID.  It is typically used for mapping
        of packet ID's types to 1553 sub-addresses.
        """
        self.m_chan = int(line[1])

    # Comment records are ignored

    def comment(self, line):
        pass  # Ignored

    # Process and save a packet record. This completed processing of the
    # previous packet (if any)

    def packet(self, line):

        self.packet_complete()
        self.init()

    # Process and save a header record

    def header(self, line):
        if len(line) < 2:
            self.err_msg("Missing parameter for header")
        he = HeaderItem()

        he.m_bit_start = self.m_bit_index

        kw = line[1].lower()
        if kw == "id":
            if len(line) < 5:
                self.err_msg("Missing parameter for header ID")
            he.m_type = "HEADER_FIELD_ID"
            s = line[4].strip()
            if not s.isdigit():
                self.err_msg("Illegal value for number of bits: '" + s + "'")
            he.m_bits = int(s)
            self.m_bit_index += he.m_bits
        elif kw == "time":
            he.m_type = "HEADER_FIELD_TIME"
            he.m_bits = 48
            self.m_bit_index += he.m_bits
        elif kw == "sequence":
            if len(line) < 6:
                self.err_msg("Missing parameter for header sequence")
            he.m_type = "HEADER_FIELD_SEQUENCE"
            s = line[4].strip()
            if not s.isdigit():
                self.err_msg("Illegal value for number of bits: '" + s + "'")
            he.m_bits = int(s)
            self.m_bit_index += he.m_bits
        elif kw == "field":
            if len(line) < 4:
                self.err_msg("Missing parameter for header field")
            he.m_type = "HEADER_FIELD_FIELD"

            s = line[2].strip()
            if not s.isdigit():
                self.err_msg("Illegal value for ID: '" + s + "'")
            he.m_id = int(s)

            s = line[4].strip()
            if not s.isdigit():
                self.err_msg("Illegal value for number of bits: '" + s + "'")
            he.m_bits = int(s)
            self.m_bit_index += he.m_bits
            self.num_header_general_fields += 1

        else:
            self.err_msg("Illegal keyword for header: '" + line[1] + "'")

        if len(line) > 5:
            he.m_comment = line[5]

        self.m_header_list.append(he)

    # Process and save the packet identifier record

    def identifier(self, line):
        if len(line) < 3:
            self.err_msg("Missing parameter(s) for identifier")

        self.m_name = line[1].strip()
        if not self.m_name:
            self.err_msg("Name cannot be blank")

        if len(self.m_name.split()) > 1:
            self.err_msg("Name must not contain spaces")

        if self.m_name.find("/") >= 0:
            self.err_msg("Name must not contain '/'")

        if self.m_name.find("\\") >= 0:
            self.err_msg("Name must not contain '\\'")

        s = line[2].strip()
        if not s.isdigit():
            self.err_msg("Illegal value for identifier: '" + s + "'")
        self.m_id = int(s)
        if self.m_id < 0:
            self.err_msg("Negative value for identifier: '" + line[2] + "'")
        if len(line) > 5:
            self.m_id_comment = line[5]

    # Process and save an item record

    def item(self, line):

        self.m_num_variable_fields += 1
        it = Item()

        it.m_bit_start = self.m_bit_index

        if len(line) < 5:
            self.err_msg("Missing parameter(s) for item")

        it.m_name = line[1].strip()
        if not it.m_name:
            self.err_msg("Name cannot be blank")

        if len(it.m_name.split()) > 1:
            self.err_msg("Name must not contain spaces")

        s = line[2].strip()
        if not s.isdigit():
            self.err_msg("Illegal value for identifier: '" + s + "'")
        it.m_id = int(s)

        it.m_data_type = line[3].strip()

        s = line[4].strip()
        if not s.isdigit():
            self.err_msg("Illegal value for number of bits: '" + s + "'")
        it.m_bits = int(s)

        self.m_bit_index += it.m_bits

        if len(line) > 5:
            it.m_comment = line[5]

        self.m_item_list.append(it)

    # Process and save a reserve record

    def reserve(self, line):
        it = Item()

        it.m_bit_start = self.m_bit_index

        if len(line) < 5:
            self.err_msg("Missing parameter for reserve")

        it.m_is_reserve = True

        s = line[4].strip()
        if not s.isdigit():
            self.err_msg("Illegal value for number of bits: '" + s + "'")
        it.m_bits = int(s)
        if it.m_bits < 1:
            self.err_msg("Illegal value for number of bits: '" + s + "'")

        self.m_bit_index += it.m_bits

        if len(line) > 5:
            it.m_comment = line[5]

        self.m_item_list.append(it)

    # Process and save an align record

    def align(self, line):

        it = Item()

        it.m_bit_start = self.m_bit_index

        if len(line) < 2:
            self.err_msg("Missing parameter for align")

        it.m_is_reserve = True

        s = line[1].strip()
        if not s.isdigit():
            self.err_msg("Illegal value for number of bits: '" + s + "'")
        bits = int(s)
        if bits not in (8, 16, 32, 64):
            self.err_msg("Illegal value for number of bits: '" + s + "'")

        if self.m_bit_index % bits:
            bits = bits - (self.m_bit_index % bits)
        else:
            return  # Already aligned
        it.m_bits = bits

        self.m_bit_index += it.m_bits

        if len(line) > 5:
            it.m_comment = line[5]

        self.m_item_list.append(it)

    # Process and save a constant record

    def constant(self, line):

        self.m_num_fixed_fields += 1
        it = Item()

        it.m_bit_start = self.m_bit_index

        if len(line) < 5:
            self.err_msg("Missing parameter for constant")

        it.m_is_constant = True

        it.m_name = line[1].strip()
        if not it.m_name:
            self.err_msg("Name cannot be blank")

        if len(it.m_name.split()) > 1:
            self.err_msg("Name must not contain spaces")

        it.m_data_type = line[2].strip()
        it.m_data_type = it.m_data_type.lower()
        if it.m_data_type not in ("integer", "float", "text"):
            self.err_msg("Invalid date type: '" + it.m_data_type + "'")

        it.m_constant_value = line[3]
        if it.m_data_type == "integer":
            it.m_constant_value = it.m_constant_value.strip()
            if not it.m_constant_value.isdigit():
                self.err_msg("Invalid numeric value: '" + it.m_constant_value + "'")
            it.m_constant_value = int(it.m_constant_value)
        elif it.m_data_type == "float":
            it.m_constant_value = it.m_constant_value.strip()
            try:
                f = float(it.m_constant_value)
            except ValueError:
                self.err_msg(
                    "Invalid floating point value: '" + it.m_constant_value + "'"
                )
            it.m_constant_value = f

        s = line[4].strip()
        if not s.isdigit():
            self.err_msg("Illegal value for number of bits: '" + s + "'")
        bits = int(s)
        if bits < 1:
            self.err_msg("Illegal value for number of bits: '" + s + "'")

        it.m_bits = bits

        self.m_bit_index += it.m_bits

        if len(line) > 5:
            it.m_comment = line[5]

        self.m_item_list.append(it)

    # packet_complete() is called when a packet definition is complete to save
    # the packet

    def packet_complete(self):

        global tlm_input_line_num
        global tlm_packet_list
        global tlm_max_packet_bytes
        global tlm_duration
        global tlm_period
        global verbose

        if not self.m_header_list and not self.m_item_list:
            return

        self.m_bytes = (self.m_bit_index + 7) / 8
        if self.m_bytes > tlm_max_packet_bytes:
            tlm_max_packet_bytes = self.m_bytes

        for field in self.m_header_list:
            if field.m_bits > self.m_max_field_bits:
                self.m_max_field_bits = field.m_bits

        for item in self.m_item_list:
            if not item.m_is_reserve and not item.m_is_constant:
                if item.m_bits > self.m_max_field_bits:
                    self.m_max_field_bits = item.m_bits

        if verbose:
            if tlm_duration is not None:
                print("Duration (in seconds only): %f" % tlm_duration)
            if tlm_period is not None:
                print("Run or Sample Period (in hz. only): %f" % tlm_period)

            if self.m_freq is not None:
                print("Packet frequency (Hz.): " + str(self.m_freq))

            if self.m_offset is not None:
                print("Packet offset: " + str(self.m_offset))

            print(
                "packet size in bits: "
                + str(self.m_bit_index)
                + " ("
                + str(self.m_bit_index / 8)
                + " bytes)"
            )
            print(
                "Number of fixed-value fields:"
                + str(self.m_num_fixed_fields)
                + ", variable fields: "
                + str(self.m_num_variable_fields)
            )
            print("name: ", self.m_name)
            print("packet ID: ", self.m_id, ", comment: ", self.m_id_comment)

            print(
                "Number of items in packet header list:         ",
                len(self.m_header_list),
            )
            print(
                "Number of general items in packet header list: ",
                self.num_header_general_fields,
            )
            print(
                "Maximum field size in bits:                    ", self.m_max_field_bits
            )
            for field in self.m_header_list:
                print("\tType:         ", field.m_type)
                print("\tID:           ", field.m_id)
                print("\tStart bit:    ", field.m_bit_start)
                print("\tSize in bits: ", field.m_bits)
                print("\tComment:      ", field.m_comment)
            print("")

            print("Number of items in packet item list: ", len(self.m_item_list))
            for i, item in enumerate(self.m_item_list, start=1):
                print("Item # ", i)
                print("\tis_reserve:     ", item.m_is_reserve)
                print("\tis_constant:    ", item.m_is_constant)
                print("\tname:           ", item.m_name)
                print("\tid:             ", item.m_id)
                print("\tdata type:      ", item.m_data_type)
                print("\tconstant value: ", item.m_constant_value)
                print("\tstart bit:      ", item.m_bit_start)
                print("\tbits:           ", item.m_bits)
                print("\tcomment:        ", item.m_comment)
            print()

        if self.m_name == "":
            self.err_msg("Preceding packet has no name")

        p = copy.deepcopy(self)
        tlm_packet_list.append(p)


# The CsvLine class parses a single line and saves the info in the
# packet member.  When a packet record is found the previously defined
# packet is saved.


class CsvLine:
    def __init__(self):
        global tlm_duration
        self.packet = Packet()
        self.keywords = {
            "duration": self.packet.duration,
            "runperiod": self.packet.run_period,
            "frequency": self.packet.frequency,
            "offset": self.packet.offset,
            "channel": self.packet.channel,
            "comment": self.packet.comment,
            "packet": self.packet.packet,
            "header": self.packet.header,
            "identifier": self.packet.identifier,
            "item": self.packet.item,
            "reserve": self.packet.reserve,
            "align": self.packet.align,
            "constant": self.packet.constant,
        }

    # process() is called with a single line already tokenized as a list

    def process(self, line):

        global tlm_input_line_num

        if not line:
            return

        nonblank = False
        for i in range(len(line)):
            s = line[i].strip()
            if len(s):
                nonblank = True
                break
        if not nonblank:
            return

        kw = line[0].lower()
        if kw in self.keywords:
            self.keywords[kw](line)
        else:
            print("Invalid keyword '" + line[0] + "' at line ", tlm_input_line_num)
            exit(1)

    def finish(self):

        self.packet.packet_complete()


# CsvFile reads the input file and processes each line


class CsvFile:
    def __init__(self):
        self.line = CsvLine()

    def process(self, name):
        global tlm_input_line_num
        try:
            m_fp = open(name)
        except OSError:
            print(f"Error opening {name}")
            exit()
        m_reader = csv.reader(m_fp, dialect="excel")

        for line in m_reader:
            tlm_input_line_num += 1
            self.line.process(line)

        self.line.finish()
        m_fp.close()


def sched_cycle_ids_max(max_cycle):
    """
    Return a list of the maximum times an ID is assigned to a cycle slot.
    """
    cycle_max_list = [0 for _ in range(max_cycle)]
    cycle_offset = None
    i = 0

    if tlm_period is None:
        return []

    for p in tlm_packet_list:
        if (p.m_freq is not None) and (p.m_offset is not None):
            cycle_offset = int(((1.0 / p.m_freq) / (1.0 / tlm_period)) + 0.5)
            offset = p.m_offset
        else:
            cycle_offset = None

        if cycle_offset is not None:
            for i in range(max_cycle):
                if i % cycle_offset == 0:
                    if (i + offset) < max_cycle:
                        cycle_max_list[i + offset] += 1
        # print p.m_id, p.m_freq, p.m_offset
        # print cycle_offset
    # print cycle_max_list
    return cycle_max_list


def sched_cycle_ids(max_cycle):
    """
    Return a list of cycle slots where each slot is a list of IDs assigned in that slot.
    Note the list returned by sched_cycle_ids_max is the number of IDs per cycle slot.
    """
    cycle_id_list = [list() for _ in range(max_cycle)]
    cycle_offset = None

    if tlm_period is None:
        return []

    for p in tlm_packet_list:
        if (p.m_freq is not None) and (p.m_offset is not None):
            cycle_offset = int(((1.0 / p.m_freq) / (1.0 / tlm_period)) + 0.5)
            offset = p.m_offset
        else:
            cycle_offset = None

        if cycle_offset is not None:
            for i in range(max_cycle):
                if i % cycle_offset == 0:
                    if (i + offset) < max_cycle:
                        cycle_id_list[i + offset].append(p.m_id)

        # print p.m_id, p.m_freq, p.m_offset
    for id in cycle_id_list:
        if len(id) == 0:
            id.append(-1)

    # j = 0
    # for i in cycle_id_list:
    #    print "%d: %s" % (j, i)
    #    j += 1

    return cycle_id_list


def sched_id_arr_size(cycle_id_list):
    """
    Return total size for the scheduled ID array.
    """
    id_list_size = 0
    for l in cycle_id_list:
        s = len(l)
        if s == 0:
            id_list_size += 1
        else:
            id_list_size += s
    return id_list_size


def id_to_channel_map():
    """
    Return an association list of ID to channel number assignments.
    NOTE: the channel number is intended to be 1553 sub-address.
    """


def output_cpp(output_file, template_file):
    global tlm_duration
    global tlm_period
    global tlm_packet_list
    global tlm_max_packet_bytes

    t = Template(file=template_file)
    t.tlm_packet_list = tlm_packet_list

    # Generate schedule code if both duration and period defined
    if (tlm_duration is not None) and (tlm_period is not None):
        t.tlm_max_cycles = int(tlm_duration * tlm_period)
    else:
        t.tlm_max_cycles = None

    t.tlm_cycle_max_list = sched_cycle_ids_max(t.tlm_max_cycles)
    t.tlm_cycle_id_list = sched_cycle_ids(t.tlm_max_cycles)
    # Create ID to channel mapping
    t.tlm_cycle_id_arr_size = sched_id_arr_size(t.tlm_cycle_id_list)
    t.tlm_max_packet_bytes = tlm_max_packet_bytes
    t.tlm_max_num_chan = sum([pkt.m_chan is not None for pkt in tlm_packet_list])
    # Create duration and period values
    t.tlm_max_num_freq = sum([pkt.m_freq is not None for pkt in tlm_packet_list])
    if tlm_duration is not None:
        t.tlm_duration = tlm_duration
    else:
        t.tlm_duration = -1.0
    #
    if tlm_period is not None:
        t.tlm_period = tlm_period
    else:
        t.tlm_period = -1

    f = open(output_file, "w")
    print(t, file=f)


def output_html():
    global tlm_packet_list


def output_dict():
    global tlm_packet_list


def output(cpp_output_file, template_file):
    output_cpp(cpp_output_file, template_file)
    output_html()
    output_dict()


def main():

    global verbose

    parser = argparse.ArgumentParser(description="Process a telemetry definition file")
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose mode"
    )
    parser.add_argument(
        "--cpp",
        action="store",
        required=True,
        help="Required. Fully-qualified output cpp file name",
    )
    parser.add_argument(
        "--template",
        action="store",
        required=True,
        help="Required. Fully-qualified file name of the cheetah template used by this script (normally in Autocoders/Python/src/fprime_ac/utils/TlmPacketDefBase.tmpl)",
    )
    parser.add_argument(
        "input_file", nargs="+", help="One or more input tab-delimited csv file names"
    )
    args = parser.parse_args()

    verbose = args.verbose
    cpp_output_file = args.cpp
    template_file = args.template
    f = CsvFile()
    for file in args.input_file:
        f.process(file)

    output(cpp_output_file, template_file)


if __name__ == "__main__":
    main()
