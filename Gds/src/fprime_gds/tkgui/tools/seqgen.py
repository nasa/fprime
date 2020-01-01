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
import os
import copy
from optparse import OptionParser

from fprime.common.models.serialize.type_exceptions import *

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
from fprime_gds.common.models.common.command import Descriptor
from fprime_gds.common.encoders.seq_writer import SeqBinaryWriter
from fprime_gds.tkgui.controllers import command_loader
from fprime_gds.tkgui.controllers import exceptions as gseExceptions
from fprime_gds.common.parsers.seq_file_parser import SeqFileParser 

# except:
#  __error("The Gse source code was not found in your $PYTHONPATH variable. Please set PYTHONPATH to something like: $BUILD_ROOT/Gse/src:$BUILD_ROOT/Gse/generated/$DEPLOYMENT_NAME")

def __errorLine(lineNumber, string):
  '''
  Print an error message relating to a line number of the file input and exit with error code 1
  @param lineNumber: the current line number being parsed
  @param string: the custom error string to print
  '''
  __error("Error on line %d: %s" % (lineNumber + 1, string))


def generateSequence(inputFile, outputFile, generated_path, timebase):
  '''
  Write a binary sequence file from a text sequence file
  @param inputFile: A text input sequence file name (usually a .seq extension)
  @param outputFile: An output binary sequence file name (usually a .bin extension)
  '''
  # Check the user environment:
  generated_command_path = generated_path + "/commands"
  cmds = command_loader.CommandLoader.getInstance()
  try:
    cmds.create(generated_command_path)
  except gseExceptions.GseControllerUndefinedDirectoryException:
    __error("Environment variable 'GSE_GENERATED_PATH' is set to '" + generated_path + "'. This is not a valid directory. Make sure this variable is set correctly, and the GSE python deployment autocode as been installed in this location.")

  # Parse the input file:
  command_list = []
  command_obj_dict = cmds.getCommandDict()
  parser = SeqFileParser()
  for i, descriptor, seconds, useconds, mnemonic, args in parser.parse(inputFile):
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
  writer = SeqBinaryWriter(timebase=timebase)
  if not outputFile:
    outputFile = os.path.splitext(inputFile)[0] + ".bin"
  try:
    writer.open(outputFile)
  except:
    __error("Encountered problem opening output file '" + outputFile + "'.")
  writer.write(command_list)
  writer.close()
  
  
help_text = "seqgen.py -d"

if __name__ == "__main__":
  '''
  The main program if run from the command line. Note that this file can also be used
  as a module by calling the generateSequence() function
  '''

  usage = "usage: %prog [options] input_file output_file"
  parser = OptionParser(usage=usage) 
  parser.add_option("-g", "--generated_path", dest="generated_path", action="store", type="string", \
                      help="Set base path to generated command/telemetry definition files")
  parser.add_option("-t", "--timebase", dest="timebase", action="store", type="string", default = None, \
                      help="Set base path to generated command/telemetry definition files [default: any]")
  
  (opts, args) = parser.parse_args()
      
  if opts.timebase == None:
     timebase = 0xffff
  else:
     try:
        timebase = int(opts.timebase,0)
     except ValueError:
        print("Could not parse time base %s"%opts.timebase)
        sys.exit(-1)
     
  if (len(args) == 1 or len(args) == 2):
    inputfile = args[0]
    if len(args) == 1:
        outputfile = None
    generateSequence(inputfile,outputfile, opts.generated_path,timebase)
  else:
    parser.print_help()
    sys.exit(1)
  sys.exit(0)
