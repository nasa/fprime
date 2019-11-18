#!/bin/env python
#===============================================================================
# NAME: cosmosgen.py
#
# DESCRIPTION: A tool for autocoding topology XML files into cosmos targets in 
# COSMOS directory.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import os
import sys

from optparse import OptionParser
from fprime_ac.models import ModelParser
from fprime_ac.utils import ConfigManager

from fprime_ac.utils.cosmos import CosmosGenerator
from fprime_ac.utils.cosmos import CosmosTopParser
from fprime_ac.utils.cosmos.util import CosmosUtil

# Needs to be initialized to create the other parsers
CONFIG = ConfigManager.ConfigManager.getInstance()

# Flag to indicate verbose mode.
VERBOSE = False

# Build Root environmental variable if one exists.
BUILD_ROOT = None

# Directory name for COSMOS files at base of fprime.
COSMOS_PATH = None

# Deployment name from topology XML only
DEPLOYMENT = None

# Version label for now
class Version:
    id      = "0.1"
    comment = "Initial prototype"
VERSION = Version()

def pinit():
    """
    Initialize the option parser and return it.
    """
    
    # Format build root for program description
    if ('BUILD_ROOT' in os.environ.keys()) == True:
        root_dir = os.environ["BUILD_ROOT"]
    else:
        print("ERROR: Build root not set to root build path...")
        sys.exit(-1)
    
    usage = "usage: %prog [options] [xml_topology_filename]"
    vers = "%prog " + VERSION.id + " " + VERSION.comment
    program_longdesc = '''
This script reads F' topology XML and produces .txt and ruby files that configure a COSMOS target.
These can be used to send commands and receive telemetry within the COSMOS system. Full documentation for this tool can be found at %sAutocoders/Python/src/fprime_ac/utils/cosmos''' % root_dir
    program_license = "Copyright 2018 user_name (California Institute of Technology)                                            \
                ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged."

    parser = OptionParser(usage, version=vers, epilog=program_longdesc,description=program_license)
    
    parser.add_option("-p", "--path", dest="path", default = None,
        help="Set the location of the COSMOS directory(def: 'BUILD_ROOT/COSMOS').")
    
    parser.add_option("-r", "--rm_target", dest="target_rm",
                      help="Target to be removed", action="store", default=None)
    
    parser.add_option("-v", "--verbose", dest="verbose_flag",
                      help="Enable verbose mode showing more runtime detail (def: False)",
                      action="store_true", default=False)
    
    return parser

        
def change_to_lowest_dir():
    """
    Go down directories until current working directory is "/"
    """
    cur_dir = os.getcwd()
    while not cur_dir == "/":
        os.chdir("./../")
        cur_dir = os.getcwd()
    
def main():
    """
    Main program.
    """

    Parser = pinit()
    (opt, args) = Parser.parse_args()
    CosmosUtil.VERBOSE = opt.verbose_flag
    CONFIG = ConfigManager.ConfigManager.getInstance()
        
    #
    # Handle command line arguments
    #
    
    # Get the current working directory so that we can return to it at end
    CosmosUtil.STARTING_DIRECTORY = os.getcwd()
    
    # Check for BUILD_ROOT env. variable
    if ('BUILD_ROOT' in os.environ.keys()) == False:
        print("ERROR: Build root not set to root build path...")
        sys.exit(-1)
    else:
        # Handle BUILD_ROOT
        BUILD_ROOT = os.environ['BUILD_ROOT']
        ModelParser.BUILD_ROOT = BUILD_ROOT
        if CosmosUtil.VERBOSE:
            print("BUILD_ROOT set to %s in environment" % BUILD_ROOT)
        
        # Create Cosmos Generator and Parser
        cosmos_gen = CosmosGenerator.CosmosGenerator()
        cosmos_parser = CosmosTopParser.CosmosTopParser(BUILD_ROOT)
        
        # Handle custom COSMOS directory location from command line
        path = "COSMOS"
        if opt.path:
            change_to_lowest_dir()
            path = opt.path
            # Fix string if too many /'s
            if path[0] == "/":
                path = path[1:]
            if path[len(path) - 1] == "/":
                path = path[:len(path) - 1]
            if not os.path.exists(path):
                print("ERROR: CUSTOM COSMOS PATH " + path + " DOES NOT EXIST")
                sys.exit(-1)
            COSMOS_PATH = path
        else:
            COSMOS_PATH = BUILD_ROOT + "/" + path
        print("COSMOS_PATH: " + COSMOS_PATH)
          
    # Remove a target from filesystem
    if opt.target_rm:
        target = opt.target_rm.upper()
        cosmos_gen.remove_target(COSMOS_PATH, target)
        sys.exit(-1)

    #
    #  Parse the input Topology XML filename
    #
    if len(args) == 0:
        print("ERROR: Usage: %s [options] xml_filename" % sys.argv[0])
        return
    else:
        xml_filenames = args[0:]
    
    #
    # Create XML Parser and write COSMOS files for Topology
    #
    for xml_filename in xml_filenames:
            #
            # Create COSMOS application file system here by passing XML parser data to cosmos config file generator
            #

            # Parse the topology file into the models
            # DO NOT ADD MODELS TO CHANNELS, EVENTS, OR COMMANDS OUTSIDE OF THIS METHOD
            cosmos_parser.parse_topology(xml_filename)
            
            DEPLOYMENT = cosmos_parser.get_deployment()
            
            # Pass parsed data from parser to generator
            cosmos_gen.load_channels(cosmos_parser.get_channels())
            cosmos_gen.load_events(cosmos_parser.get_events())
            cosmos_gen.load_commands(cosmos_parser.get_commands())
            
            # Create the config/targets directories for the Deployment and clears out old files
            cosmos_gen.create_filesystem(DEPLOYMENT, COSMOS_PATH)
             
            # Generate all files here
            cosmos_gen.generate_cosmos_files(DEPLOYMENT, COSMOS_PATH)

            
    # Always return to directory where we started.
    os.chdir(CosmosUtil.STARTING_DIRECTORY)

if __name__ == '__main__':
    main()
