"""
Created on August 16, 2019

@author: tcanham
"""

import struct
import zlib

from fprime.common.models.serialize.type_exceptions import TypeMismatchException
from fprime.common.models.serialize.numerical_types import U8Type, U16Type, U32Type
from fprime_gds.common.utils import config_manager
from fprime_gds.common.utils.data_desc_type import DataDescType


class SeqBinaryWriter:
    """
    Write out the Binary (ASTERIA) form of sequencer file.
    """
    def __init__(self, timebase=0xffff, config=None):
        """
        Constructor
        """
        if config is None:
            config = config_manager.ConfigManager().get_instance()

        self.__fd = None
        self.__timebase = timebase
        self.desc_obj = config.get_type("msg_desc")
        self.opcode_obj = config.get_type("op_code")
        self.len_obj = config.get_type("msg_len")

    def open(self, filename):
        """
        Open the ASCII file
        """
        self.__fd = open(filename, "wb")

    def __binaryCmdRecord(self, cmd_obj):
        """
        Return the binary command record the sequencer is expecting.
        @todo: Currently the command descriptor is always zero for immediate commands and execution time tags needed in
        command objects and seq_panel.
        """

        def __time_tag(cmd_obj):
            """
            TODO: support a timebase in the cmd obj? This is mission specific, so it is tough to handle. For now
            I am hardcoding this to 2 which is TB_NONE
            """
            # return TimeType(timeBase=2, seconds=cmd_obj.getSeconds(), useconds=cmd_obj.getUseconds()).serialize()
            # TKC - new command time format
            return (
                U32Type(cmd_obj.getSeconds()).serialize()
                + U32Type(cmd_obj.getUseconds()).serialize()
            )

        def __descriptor(cmd_obj):
            # subtract 1 from the value because enum34 enums start at 1, and this can't be changed
            return U8Type(cmd_obj.getDescriptor().value - 1).serialize()

        def __command(cmd_obj):
          self.desc_obj.val = DataDescType["FW_PACKET_COMMAND"].value
          self.opcode_obj.val = cmd_obj.getOpCode()
          command = self.desc_obj.serialize()  # serialize combuffer type enum: FW_PACKET_COMMAND
          command += self.opcode_obj.serialize()  # serialize opcode
          # Command arguments
          for arg in cmd_obj.getArgs():
              command += arg[2].serialize()
          return command

        def __length(command):
          self.len_obj.val = len(command)
          return self.len_obj.serialize()

        def __print(byteBuffer):
            print("Byte buffer size: %d" % len(byteBuffer))
            for entry in range(0, len(byteBuffer)):
                print(
                    "Byte %d: 0x%02X (%c)"
                    % (
                        entry,
                        struct.unpack("B", byteBuffer[entry])[0],
                        struct.unpack("B", byteBuffer[entry])[0],
                    )
                )

        # This is no longer in the sequence file format.
        # def __checksum(data):
        #  csum = 0
        #  for entry in range(0,len(data)):
        #    byte = struct.unpack("B",data[entry])[0]
        #    csum += byte
        #  return U64Type(long(csum)).serialize()

        # Form header:
        descriptor = __descriptor(cmd_obj)
        time = __time_tag(cmd_obj)
        header = descriptor + time

        # Command opcode:
        command = __command(cmd_obj)

        # Command length:
        length = __length(command)

        # Checksum:
        # This is no longer in the sequence file format.
        # checksum = __checksum(header + length + command)

        # Debug printing (comment out when not debugging):
        # print "descriptor:"
        # __print(descriptor)
        # print "time:"
        # __print(time)
        # print "length:"
        # __print(length)
        # print "command:"
        # __print(command)
        # print "total record:"
        # __print(header + checksum + length + command)

        # Construct the record:
        return header + length + command

    def write(self, seq_cmds_list):
        """
        Write out each command record
        """
        num_records = len(seq_cmds_list)
        sequence = b""
        for cmd in seq_cmds_list:
            sequence += self.__binaryCmdRecord(cmd)
        size = len(sequence)
        if self.__timebase == 0xFFFF:
            tb_txt = b"ANY"
        else:
            tb_txt = bytes(self.__timebase)

        print("Sequence is %d bytes with timebase %s" % (size, tb_txt))

        header = b""
        header += U32Type(
            size + 4
        ).serialize()  # Write out size of the sequence file in bytes here
        header += U32Type(num_records).serialize()  # Write number of records
        header += U16Type(self.__timebase).serialize()  # Write time base
        header += U8Type(0xFF).serialize()  # write time context
        sequence = header + sequence  # Write the list of command records here
        # compute CRC. Ported from Utils/Hash/libcrc/libcrc.h (update_crc_32)
        crc = self.computeCrc(sequence)

        print("CRC: %d (0x%04X)" % (crc, crc))
        try:
            sequence += U32Type(crc).serialize()
        except TypeMismatchException as typeErr:
            print("Exception: %s" % typeErr.getMsg())
            raise

        # Write the list of command records here
        self.__fd.write(sequence)

    def close(self):
        """
        Close the Binary file
        """
        self.__fd.close()

    def computeCrc(self, buff):
        # See http://stackoverflow.com/questions/30092226/how-to-calculate-crc32-with-python-to-match-online-results
        # RE: signed to unsigned CRC
        return zlib.crc32(buff) % (1 << 32)


class SeqAsciiWriter:
    """
    Write out the ASCII record form of sequencer file.
    """

    def __init__(self):
        """
        Constructor
        """
        self.__fd = None

    def open(self, filename):
        """
        Open the ASCII file
        """
        self.__fd = open(filename, "w")

    def __getCmdString(self, cmd_obj):
        """
        For an command return it stringified.
        """
        mnemonic = cmd_obj.getMnemonic()
        opcode = cmd_obj.getOpCode()
        args = cmd_obj.getArgs()
        #
        cmd = "{} (0x{:x})".format(mnemonic, int(opcode))
        for arg in args:
            cmd += ", %s" % arg[2].val
        return cmd

    def write(self, seq_cmds_list):
        """
        Write out each record as it appears in the listbox widget.
        """
        for cmd in seq_cmds_list:
            self.__fd.write(self.__getCmdString(cmd) + "\n")

    def close(self):
        """
        Close the ASCII file
        """
        self.__fd.close()
