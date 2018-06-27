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

__author__ = "Kevin Dinkel"
__copyright__ = "Copyright 2015, California Institute of Technology."
__version__ = "1.0"
__email__ = "kevin.dinkel@jpl.nasa.gov"

def __error(string):
  '''
  Print an error message and exit with error code 1
  @param string: the custom error string to print
  '''
  print string
  sys.exit(1)

# try:
from models.common.command import Descriptor
from views.seq_panel import SeqBinaryWriter
from controllers import command_loader
from controllers import exceptions as gseExceptions

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
  command_loader = command_loader.CommandLoader.getInstance()
  try:
    command_loader.create(generated_command_path)
  except gseExceptions.GseControllerUndefinedDirectoryException:
    __error("Environment variable 'GSE_GENERATED_PATH' is set to '" + generated_path + "'. This is not a valid directory. Make sure this variable is set correctly, and the GSE python deployment autocode as been installed in this location.")

  # Parse the input file:
  command_list = []
  command_obj_dict = command_loader.getCommandDict()
  for i, descriptor, seconds, useconds, mnemonic, args in __parse(inputFile):
    # Make sure that command is in the command dictionary:
    if mnemonic in command_obj_dict:
      command_obj = copy.deepcopy(command_obj_dict[mnemonic])
      # Set the command arguments:
      try:
        command_obj.setArgs(args)
      except ArgLengthMismatchException as e:
        __errorLine(i, "'" + mnemonic + "' argument length mismatch. " + e.getMsg())
      except TypeException as e:
        __errorLine(i, "'" + mnemonic + "' argument type mismatch. " + e.getMsg())
      # Set the command time and descriptor:
      command_obj.setDescriptor(descriptor)
      command_obj.setSeconds(seconds)
      command_obj.setUseconds(useconds)
      # Append this command to the command list:
      command_list.append(command_obj)
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
