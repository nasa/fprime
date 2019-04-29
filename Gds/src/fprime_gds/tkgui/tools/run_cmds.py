#!/bin/env python

import os
import sys
import time
from fprime_gds.tkgui.utils.gse_api import GseApi
from optparse import OptionParser

__version__ = 0.1
__date__ = '2015-08-01'
__updated__ = '2015-08-01'

def main(argv=None):
    
    if argv is None:
        argv =sys.argv[1:]
    try:
        program_name = os.path.basename(sys.argv[0])
        program_version = "v0.1"
        program_build_date = "%s" % __updated__
        program_longdesc = '''''' # optional - give further explanation about what the program does
        program_license = "Copyright 2015 user_name (California Institute of Technology)                                            \
                ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged."

        program_version_string = '%%prog %s (%s)' % (program_version, program_build_date)
        if argv is None:
            argv = sys.argv[1:]

        parser = OptionParser(version=program_version_string, epilog=program_longdesc, description=program_license)
        parser.add_option("-d", "--dictionary", dest="generated_path", action="store", type="string", \
                help="Set base path to generated command/telemetry definition files")
        parser.add_option("-a", "--addr", dest="addr", action="store", type="string", help="set threaded tcp socket server address [default: %default]", \
                default="127.0.0.1")
        parser.add_option("-c", "--connect", action="store_true", dest="connect", help="Launches the Threaded TCP Socket Server on startup and connect to it [default: %default]", \
                default=False)
        parser.add_option("-p", "--port", dest="port", action="store", type="int", help="Set threaded tcp socket server port [default: %default]", \
                default=50000) 
        parser.add_option("-v", "--verbose", action="store_true",  help="Verbose output (commands and arguments) [default: %default]", \
                default=True) 
        parser.add_option("-w", "--wait", dest="wait", type="float",  help="Wait <arg> seconds between commands [default: %default]", \
                default=1.0) 
        (opts, args) = parser.parse_args(argv) 
    
    except Exception, e:
        indent = len(program_name) * " "
        sys.stderr.write(program_name + ": " + repr(e) + "\n")
        sys.stderr.write(indent + "  for help use --help\n")
        return 2

    api = GseApi(generated_path=opts.generated_path, port=opts.port, server_addr=opts.addr, verbose=True)
    
    # process each of the files
    for script in args:
        print "Parsing command file %s"%script
        command_list = process_command_file(script)
        for (command,cmd_args) in command_list:
            # Check for delay request in script
            if command == "WAIT":
                wait_time = int(cmd_args[0])
                while wait_time > 0:
                    print ("Wait %d"%wait_time)
                    time.sleep(1)
                    wait_time = wait_time - 1
            else:
                if api.send(command, args=cmd_args) != -1:
                    time.sleep(opts.wait)
                else:
                    sys.stderr.write("Script error. Exiting...\n")
                    sys.exit(1)
            
# returns list of command tuples; first element is command mnemonic, second is list of arguments

def process_command_file(file):
    lines = open(file,'rU').readlines()
    command_list = list()
    for line in lines:
        # first, strip whitespace
        line = line.strip()
        # first, get rid of comments. Only want text to left of #
        if line.count("#"):
            line = line.split("#",1)[0]
        # check for empty line
        if not len(line):
            continue
        # next, split mnemonic from arguments. Comma separated
        command = line.split(",")
        mnemonic = command[0].strip()
        argList = list()
        for arg in command[1:]: # Take off white space and string quotes
            argList.append(arg.strip().replace("\"",""))
        command_list.append((mnemonic,argList))
        
    return command_list    
    
if __name__ == "__main__":
    main()
