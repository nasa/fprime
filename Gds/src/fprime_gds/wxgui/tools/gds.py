#!/usr/bin/env python
# encoding: utf-8
'''
This is the Fprime Ground data system GUI (GDS). It displays telemetry and event
data from an Fprime deployment. Additionally, the GDS allows for commands to be
sent to the Fprime deployment.

@author Josef Biberstein
@author Joseph Paetz

@copyright:  2018 California Institute of Technology. All rights reserved.
'''
from __future__ import print_function
import sys
import os
from time import sleep
from argparse import ArgumentParser
import wx

from fprime_gds.common.client_socket import client_socket
from fprime_gds.common.utils import config_manager
from fprime_gds.wxgui.src.main_frame_factory import MainFrameFactory
from fprime_gds.wxgui.src.GDSMainFrameImpl import MainFrameImpl


__all__ = []
__version__ = 1.0
__date__ = '2018-08-016'
__updated__ = '2018-08-16'

def get_args():
    default_port = 50000

    # setup arg parser
    parser = ArgumentParser(description=
                'GDS GUI to display telem and event data from Fprime' +
                ' deployments and send commands to them')

    # setup arg parser
    parser.add_argument("-d", "--dictionary", dest="generated_path", action="store",
                        help="Path to generated dictionary files (if using python module dictionaries)")

    parser.add_argument("-x", "--xml-dict", dest="xml_dict_path", action="store",
                        help="Path to the xml dictionary file (if using xml dictionaries)")

    parser.add_argument("-s", "--pkt-spec", dest="pkt_spec_path", action="store",
                        help="Path to the packet specification file")

    parser.add_argument("-a", "--addr", dest="addr", action="store",
                        help="Set threaded tcp socket server address [default=127.0.0.1]",
                        default="127.0.0.1")

    parser.add_argument("-p", "--port", dest="port", action="store", type=int,
                        help="Set threaded tcp socket server port [default: %d]"%default_port,
                        default=default_port)

    parser.add_argument("-L", dest="log_dir_path", action="store",
                        help="Path to directory where log files will be put " +
                        "(a subdirectory named with the current date and time " +
                        "will be made with individual log files within it)")

    parser.add_argument("-C", "--config", dest="config_path", action="store",
                        help="Path to the configuration file to read")

    # process options
    args = parser.parse_args()

    # Verify Arguments
    if (args.generated_path == None and args.xml_dict_path == None):
        sys.stderr.write("Either -d or -x argument must be used to indicate a dictionary path")
        return None

    if (args.generated_path != None and not os.path.exists(args.generated_path)):
        sys.stderr.write("Error: Dictionary directory %s does not exist."%args.generated_path)
        return None
    elif(args.generated_path != None and not os.path.isdir(args.generated_path)):
        sys.stderr.write("Error: Generated Dictionary path %s is not a directory"%args.generated_path)
        return None

    if (args.xml_dict_path != None and not os.path.exists(args.xml_dict_path)):
        sys.stderr.write("Error: Dictionary directory %s does not exist."%args.xml_dict_path)
        return None
    elif(args.xml_dict_path!= None and not os.path.isfile(args.xml_dict_path)):
        sys.stderr.write("Error: Generated Dictionary path %s is not a file"%args.xml_dict_path)
        return None

    if (args.pkt_spec_path != None and not os.path.exists(args.pkt_spec_path)):
        sys.stderr.write("Error: Dictionary directory %s does not exist."%args.pkt_spec_path)
        return None
    elif(args.pkt_spec_path != None and not os.path.isfile(args.pkt_spec_path)):
        sys.stderr.write("Error: Generated Dictionary path %s is not a file"%args.pkt_spec_path)
        return None

    if (args.log_dir_path != None and not os.path.exists(args.log_dir_path)):
        sys.stderr.write("Error: Dictionary directory %s does not exist."%args.log_dir_path)
        return None
    elif(args.log_dir_path != None and not os.path.isdir(args.log_dir_path)):
        sys.stderr.write("Error: Generated Dictionary path %s is not a directory"%args.log_dir_path)
        return None

    if (args.config_path != None and not os.path.exists(args.config_path)):
        sys.stderr.write("Error: Dictionary directory %s does not exist."%args.config_path)
        return None
    elif(args.config_path != None and not os.path.isfile(args.config_path)):
        sys.stderr.write("Error: Generated Dictionary path %s is not a file"%args.config_path)
        return None

    return args


def main(argv=None):
    # Double check wx python version by checking first character is at least
    #  4 (wx version 4+ required)
    wx_version = int(wx.version()[0])

    if (wx_version < 4):
        sys.stderr.write("wxPython version is %s, version 4 or above is required"%
                         wx.version())
        return -1

    '''
    Command line options.
    '''
    args = get_args()
    if (args == None):
        return -1

    # Setup the configuration file
    config = config_manager.ConfigManager()
    if (args.config_path != None):
        config.set_configs(args.config_path)
    else:
        print("No Configuration File in options, using defaults")


    app = wx.App(False)

    # Initialize main frame factory
    factory = MainFrameFactory(args, config)

    # Setup main frame factory (This is where the back end classes are
    #  initialized and registered to each other)
    factory.setup_pipeline()

    sleep(1)

    factory.client_socket.connect(args.addr, args.port)

    sleep(1)

    factory.client_socket.register_to_server(client_socket.GUI_TAG)

    app.MainLoop()

    factory.client_socket.disconnect()

if __name__ == "__main__":
    sys.exit(main())

