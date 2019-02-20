#!/usr/bin/env python
#===============================================================================
# NAME: tinyseqgen
#
# DESCRIPTION: A tiny sequence generator for F Prime. This sequence compiler takes a
# .seq file as input and produces a binary sequence file compatible with the
# F Prime sequence file loader and sequence file runner.
# AUTHOR: Kevin Dinkel
# EMAIL:  dinkel@jpl.nasa.gov
# DATE CREATED: December 15, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import sys
import re
import os
import copy
from datetime import datetime, timedelta

from enum import Enum

from fprime.gds.models.serialize.time_type import *

from fprime.gds.models.serialize.bool_type import *
from fprime.gds.models.serialize.enum_type import *
from fprime.gds.models.serialize.f32_type import *
from fprime.gds.models.serialize.f64_type import *

from fprime.gds.models.serialize.u8_type import *
from fprime.gds.models.serialize.u16_type import *
from fprime.gds.models.serialize.u32_type import *
from fprime.gds.models.serialize.u64_type import *

from fprime.gds.models.serialize.i8_type import *
from fprime.gds.models.serialize.i16_type import *
from fprime.gds.models.serialize.i32_type import *
from fprime.gds.models.serialize.i64_type import *

from fprime.gds.models.serialize.string_type import *
from fprime.gds.models.serialize.serializable_type import *

import zlib

Descriptor = Enum(value='Descriptor', names='ABSOLUTE RELATIVE')

__author__ = "Kevin Dinkel"
__copyright__ = "Copyright 2015, California Institute of Technology."
__version__ = "1.0"
__email__ = "kevin.dinkel@jpl.nasa.gov"

class SeqBinaryWriter(object):
    """
    Write out the Binary (ASTERIA) form of sequencer file.
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
        self.__fd = open(filename, "wb")

    def __binaryCmdRecord(self, cmd_obj):
        """
        Return the binary command record the sequencer is expecting.
        @todo: Currently the command descriptor is always zero for immediate commands.
        @todo: Execution time tags needed in command objects and seq_panel.
        """

        def __time_tag(cmd_obj):
          '''
          TODO: support a timebase in the cmd obj? This is mission specific, so it is tough to handle. For now
          I am hardcoding this to 2 which is TB_NONE
          '''
          #return TimeType(timeBase=2, seconds=cmd_obj.getSeconds(), useconds=cmd_obj.getUseconds()).serialize()
          # TKC - new command time format
          return U32Type( cmd_obj[1].time.seconds ).serialize() + U32Type( cmd_obj[1].time.useconds ).serialize()

        def __descriptor(cmd_obj):
          # subtract 1 from the value because enum34 enums start at 1, and this can't be changed
          return U8Type(cmd_obj[0].value-1).serialize()

        def __command(cmd_obj):
          command = U32Type( 0 ).serialize() # serialize combuffer type enum: FW_PACKET_COMMAND
          command += U32Type( cmd_obj[1].template.get_op_code() ).serialize() # serialize opcode
          # Command arguments
          for arg in cmd_obj[1].get_args():
            print(arg)
            command += arg.serialize()
          return command

        def __length(command):
          return U32Type( len(command) ).serialize()

        def __print(byteBuffer):
            print "Byte buffer size: %d" % len(byteBuffer)
            for entry in range(0,len(byteBuffer)):
                print "Byte %d: 0x%02X (%c)"%(entry,struct.unpack("B",byteBuffer[entry])[0],struct.unpack("B",byteBuffer[entry])[0])

        # This is no longer in the sequence file format.
        #def __checksum(data):
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
        #checksum = __checksum(header + length + command)

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
        Write out each record as it appears in the listbox widget.
        """
        num_records = len(seq_cmds_list)
        sequence = ""
        for cmd in seq_cmds_list:
            sequence += self.__binaryCmdRecord(cmd)
        size = len(sequence)
        print("Sequence is %d bytes"%size)

        header = ""
        header += U32Type( size + 4 ).serialize() # Write out size of the sequence file in bytes here
        header += U32Type( num_records ).serialize() # Write number of records
        header += U16Type( 0xFFFF ).serialize() # Write time base FIXME: Set to don't care = 0xFFFF for now
        header += U8Type( 0xFF ).serialize() # write time context
        sequence = header + sequence # Write the list of command records here
        # compute CRC. Ported from Utils/Hassh/libcrc/libcrc.h (update_crc_32)
        crc = self.computeCrc(sequence)

        print "CRC: %d (0x%04X)"%(crc,crc)
        try:
            sequence += U32Type( crc ).serialize()
        except TypeMismatchException as typeErr:
            print "Exception: %s" % typeErr.getMsg()
            raise

        # Write the list of command records here
        self.__fd.write( sequence )

    def close(self):
        """
        Close the Binary file
        """
        self.__fd.close()

    def computeCrc(self, buff):
        # See http://stackoverflow.com/questions/30092226/how-to-calculate-crc32-with-python-to-match-online-results
        # RE: signed to unsigned CRC
        return zlib.crc32(buff)% (1<<32)

class SeqAsciiWriter(object):
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
        cmd = "%s (0x%x)" % (mnemonic,int(opcode))
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



def __error(string):
  '''
  Print an error message and exit with error code 1
  @param string: the custom error string to print
  '''
  print string
  sys.exit(1)

from fprime.gds.loaders import cmd_py_loader
from fprime.gds.data_types import cmd_data

# try:
# from fprime.gds.models.common.command import Descriptor
# TODO might want to reimplement but probably doesn't matter
from fprime.gds.controllers import exceptions as gseExceptions

# except:
#  __error("The Gse source code was not found in your $PYTHONPATH variable. Please set PYTHONPATH to something like: $BUILD_ROOT/Gse/src:$BUILD_ROOT/Gse/generated/$DEPLOYMENT_NAME")

def __errorLine(lineNumber, string):
  '''
  Print an error message relating to a line number of the file input and exit with error code 1
  @param lineNumber: the current line number being parsed
  @param string: the custom error string to print
  '''
  __error("Error on line %d: %s" % (lineNumber + 1, string))

def __parse(seqfile):
  '''
  Generator that parses an input sequence file and returns a tuple
  for each valid line of the sequence file.
  @param seqfile: A sequence file name (usually a .seq extension)
  @return A list of tuples:
    (lineNumber, descriptor, seconds, useconds, mnemonic, arguments)
  '''

  def subQuoted(f, string):
    '''
    Run a substitution function on only substrings within a string that are surrounded
    by single or double quotes
    @param f: a string substitution acting on matchobjs
    @param string: the string to perform the substitution on
    @return the substituted string
    '''
    s = re.sub(r'"[^"]*"', f, string)
    return re.sub(r"'[^']*'", f, s)

  def removeTrailingComments(string):
    '''
    Remove any trailing comments (proceded by ';') in a string
    @param string: the string to perform comment removal on
    @return the string without trailing comments
    '''
    def replaceSemis(matchobj):
      return matchobj.group(0).replace(';', ' ')
    # ignore all semicolons in quotes:
    s = subQuoted(replaceSemis, string)
    s = subQuoted(replaceSemis, s)
    # get index of first semicolon, and return everything before it:
    if ';' in s:
      index = s.index(';')
      return string[:index]
    # return original string if no semicolon found:
    return string

  def splitString(string):
    '''
    Split a string with ' ' or ',' as a delimiter. Ignore any delimiters
    found within quoted substrings.
    @param string: the string to perform the split on
    @return a list representing the split string
    '''
    def replaceSpacesAndCommas(matchobj):
      s = re.sub('\s', '_', matchobj.group(0))
      s = re.sub('\,', '_', s)
      return s
    # ignore all spaces in quotes:
    s = subQuoted(replaceSpacesAndCommas, string)
    # replace all commas with spaces, since either can be a delimiter:
    s = s.replace(',', ' ')
    # get the split indices of the modified string:
    indices = [(m.start(), m.end()) for m in re.finditer(r'\S+', s)]
    toReturn = []
    for start, end in indices:
      toReturn.append(string[start:end])
    return toReturn

  def parseArgs(args):
    '''
    Turn .seq command argument list into their appropriate python types
    @param args: a list of parsed arguments
    @return a list of arguments as native python types
    '''
    def parseArg(arg):
      # See if argument is a string, if so remove the quotes and return it:
      if (arg[0] == '"' and arg[-1] == '"') or (arg[0] == "'" and arg[-1] == "'"):
        return arg[1:-1]
      # If the string contains a "." assume that it is a float:
      elif "." in arg:
        return float(arg)
      else:
        try:
          # See if it translates to an integer:
          return int(arg)
        except ValueError:
          try:
            # See if it translates to a float:
            return float(arg)
          except ValueError:
            # Otherwise it is an enum type:
            return str(arg)
    return map(parseArg, args)

  def parseTime(lineNumber, time):
    '''
    Parse a time string and return the command descriptor, seconds, and useconds of the time string
    @param lineNumber: the current line number where the time string was parsed
    @param time: the time string to parse
    @return a tuple (descriptor, seconds, useconds)
    '''
    
    def parseTimeStringOption(timeStr, timeFmts):
      '''
      Parse a time string by trying to use different time formats, until one succeeds
      @param timeStr: the time string
      @param timeFmts: the time format used to parse the string
      @return the datetime object containing the parsed string
      '''
      def parseTimeString(timeFmt):
        try:
          return datetime.strptime(timeStr, timeFmt)
        except:
          return None

      for fmt in timeFmts:
        dt = parseTimeString(fmt)
        if dt:
          return dt
      raise BaseException

    def parseRelative(timeStr):
      '''
      Parse a relative time string
      @param timeStr: the time string
      @return the datetime object containing the parsed string
      '''
      options = ["%H:%M:%S.%f", "%H:%M:%S"]
      return parseTimeStringOption(timeStr, options)

    def parseAbsolute(timeStr):
      '''
      Parse an absolute time string
      @param timeStr: the time string
      @return the datetime object containing the parsed string
      '''
      options = ["%Y-%jT%H:%M:%S.%f","%Y-%jT%H:%M:%S"]
      return parseTimeStringOption(timeStr, options)

    descriptor = None
    d = time[0]
    t = time[1:]
    if d == 'R':
      descriptor = Descriptor.RELATIVE
      dt = parseRelative(t)
      delta = timedelta(hours=dt.hour, minutes=dt.minute, seconds=dt.second, microseconds=dt.microsecond).total_seconds()
    elif d == 'A':
      descriptor = Descriptor.ABSOLUTE
      dt = parseAbsolute(t)
      # See if timezone was specified. If not, use UTC
      if dt.tzinfo != None:
          print "Using timezone %s" % dt.tzinfo.tzname()
          epoch = datetime.fromtimestamp(0, dt.tzinfo)
      else:
          print "Using UTC timezone"
          epoch = datetime.utcfromtimestamp(0)
      delta = (dt - epoch).total_seconds()
    else:
      __error(lineNumber, "Invalid time descriptor '" + d + "' found. Descriptor should either be 'A' for absolute times or 'R' for relative times")
    seconds = int(delta)
    useconds = int((delta - seconds) * 1000000)
    return descriptor, seconds, useconds

  # Open the sequence file and parse each line:
  with open(seqfile, "r") as inputFile:
    for i, line in enumerate(inputFile):
      line = line.strip()
      # ignore blank lines and comments
      if line and line[0] != ';':
        line = removeTrailingComments(line)
        line = splitString(line)
        length = len(line)
        if length < 2:
          __errorLine(i, "Each line must contain a minimum of two fields, time and command mnemonic\n")
        else:
          try:
            descriptor, seconds, useconds = parseTime(i, line[0])
          except:
            __errorLine(i, "Encountered syntax error parsing timestamp")
          mnemonic = line[1]
          args = []
          if length > 2:
            args = line[2:]
            try:
              args = parseArgs(args)
            except:
              __errorLine(i, "Encountered sytax error parsing arguments")
        yield i, descriptor, seconds, useconds, mnemonic, args

def generateSequence(inputFile, outputFile=None):
  '''
  Write a binary sequence file from a text sequence file
  @param inputFile: A text input sequence file name (usually a .seq extension)
  @param outputFile: An output binary sequence file name (usually a .bin extension)
  '''
  # Check the user environment:
  try:
    generated_path = os.environ['GSE_GENERATED_PATH']
    generated_command_path = generated_path + "/commands"
  except:
    __error("Environment variable 'GSE_GENERATED_PATH' not set. It should be set to something like '$BUILD_ROOT/Gse/generated/$DEPLOYMENT' and also added to $PYTHONPATH.")
  command_loader = cmd_py_loader.CmdPyLoader()
  try:
    _, command_obj_dict = command_loader.construct_dicts(generated_command_path)
  except Exception:
    __error("Environment variable 'GSE_GENERATED_PATH' is set to '" + generated_path + "'. This is not a valid directory. Make sure this variable is set correctly, and the GSE python deployment autocode as been installed in this location.")
  
  # Parse the input file:
  command_list = []
  for i, descriptor, seconds, useconds, mnemonic, args in __parse(inputFile):
    # Make sure that command is in the command dictionary:
    if mnemonic in command_obj_dict:
      command_obj = cmd_data.CmdData(tuple([str(a) for a in args]), command_obj_dict[mnemonic], TimeType(seconds=seconds, useconds=useconds))

      # Append this command to the command list:
      command_list.append((descriptor, command_obj))
    else:
      __errorLine(i, "'" + mnemonic + "' does not match any command in the command dictionary.")

  # Write to the output file:
  writer = SeqBinaryWriter()
  if not outputFile:
    outputFile = os.path.splitext(inputFile)[0] + ".bin"
  try:
    writer.open(outputFile)
  except:
    __error("Encountered problem opening output file '" + outputFile + "'.")
  writer.write(command_list)
  writer.close()

if __name__ == "__main__":
  '''
  The main program if run from the commandline. Note that this file can also be used
  as a module by calling the generateSequence() function
  '''
  if len(sys.argv) == 2 or len(sys.argv) == 3:
    generateSequence(*sys.argv[1:])
  else:
    print "Usage: tinyseqgen sequence_file_in.seq [binary_sequence_file_out.bin]"
    print
    print "Welcome to tinyseqgen - the Tiny F Prime Command Sequence Generator."
    print
    print "Description: tinyseqgen takes a simple input sequence format (.seq),"
    print "and outputs a binary command sequence loadable by the Svc/SequenceFileLoader."
    print
    print "Here, try out this example input sequence file (simple_sequence.seq):"
    print
    print ";--------------------------------------------------------------------"
    print "; Simple sequence file "
    print "; Note: that anything after a ';' is a comment"
    print ";--------------------------------------------------------------------"
    print
    print "; Commands in a sequence can either be timed absolutely or relative "
    print "; to the execution of the previous command. Here is an absolute NOOP"
    print "; command. Make sure to use UTC times, since tinyseqgen will calculate"
    print "; the seconds since the unix epoch from the given absolute time in UTC."
    print "A2015-075T22:32:40.123 CMD_NO_OP"
    print
    print "; Here is a relative NOOP command, which will be run 1 second after"
    print "; the execution of the previous command"
    print "R00:00:01 CMD_NO_OP; Send a no op command"
    print
    print "; This command will run immediately after the previously executed command"
    print "; has completed"
    print "R00:00:00 CMD_NO_OP"
    print
    print "; Let's try out some commands with arguments"
    print "R01:00:01.050 CMD_NO_OP_STRING \"Awesome string!\"; <- cool argument right?"
    print "R03:51:01.000 CMD_TEST_CMD_1 17, 3.2, 2; <- this command has 3 arguments"
    print "R00:05:00 ALOG_SET_EVENT_REPORT_FILTER INPUT_COMMAND, INPUT_DISABLED; <- this command uses enum arguments"
    print
    sys.exit(1)
  sys.exit(0)
