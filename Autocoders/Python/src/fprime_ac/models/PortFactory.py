#!/usr/bin/env python3
# ===============================================================================
# NAME: PortFactory.py
#
# DESCRIPTION: This is a factory class for instancing the Port (interface type)
#              and building up the Port and Arg configuration required.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb. 11. 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#

import logging

from fprime_ac.models import Arg, Port
from fprime_ac.parsers import XmlPortsParser
from fprime_ac.utils import Logger

# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")


class PortFactory:
    """
    This is a concrete factory method object used to create the interface
    to the code generation.  A single call to create is made that returns
    the port object containing all the includes, args, etc.
    """

    __parsed = None
    __instance = None
    __configured_visitors = None

    def __init__(self):
        """
        Private Constructor (singleton pattern)
        """
        self.__parsed = None
        self.__instance = None
        self.__configured_visitors = {}

    def getInstance():
        """
        Return instance of singleton.
        """
        if PortFactory.__instance is None:
            PortFactory.__instance = PortFactory()

        return PortFactory.__instance

    # define static method
    getInstance = staticmethod(getInstance)

    def create(self, the_parsed_port_xml):
        """
        Create a port (interface) type model here.
        """
        x = the_parsed_port_xml
        port_obj = x.get_interface()
        include_header_files_list = x.get_include_header_files()
        include_header_serial_files_list = (
            x.get_includes_serial_files() + x.get_include_array_files()
        )
        include_header_enum_files_list = x.get_include_enum_files()
        args_obj_list = x.get_args()
        #
        port_namespace = port_obj.get_namespace()
        port_name = port_obj.get_name()
        port_comment = port_obj.get_comment()
        port_return_type = port_obj.get_return_type()
        port_return_modifier = port_obj.get_return_modifier()
        #
        port_xml_filename = x.get_xml_filename()
        #
        # Add port args here...
        #
        args_list = []
        for arg in args_obj_list:
            n = arg.get_name()
            t = arg.get_type()
            m = arg.get_modifier()
            s = arg.get_size()
            c = arg.get_comment()
            args_list.append(Arg.Arg(n, t, m, s, c))
        #
        # Instance the port here...
        #
        the_port = Port.Port(
            None, port_name, None, None, None, None, None, port_xml_filename
        )
        the_port.set(
            port_namespace,
            args_list,
            include_header_files_list,
            include_header_serial_files_list,
            include_header_enum_files_list,
            port_comment,
        )
        the_port.set_return(port_return_type, port_return_modifier)
        return the_port


def main():

    # Configures output only to stdout.
    Logger.connectOutputLogger(None)

    xmlfile = "../../test/Msg1InterfaceAi.xml"

    print("Port XML parse test (%s)" % xmlfile)
    #
    # Basic usage of this factory to create the component meta-model
    #
    the_parsed_port_xml = XmlPortsParser.XmlPortsParser(xmlfile)
    print(the_parsed_port_xml.get_args())
    args = the_parsed_port_xml.get_args()
    for a in args:
        print("\t", a)
        print("\t", a.get_name())
        print("\t", a.get_type())
        print("\t", a.get_comment())
    print(the_parsed_port_xml.get_include_header_files())
    print(the_parsed_port_xml.get_interface())
    print(the_parsed_port_xml.get_interface().get_comment())
    print(the_parsed_port_xml.get_interface().get_name())
    print(the_parsed_port_xml.get_interface().get_namespace())

    port = PortFactory().create(the_parsed_port_xml)
    #
    # End of usage and port is the instance of model to be used.
    #
    print("Port: %s" % port)
    print("Namespace: %s" % port.get_namespace())
    print("Type: %s" % port.get_type())
    print("Comment: %s" % port.get_ifcomment())
    print("Includes: %s" % port.get_includes())
    print()
    args = port.get_args()
    print("Args: %s" % args)
    for a in args:
        print(
            "Arg Name: %s Type: %s Comment: %s"
            % (a.get_name(), a.get_type(), a.get_comment())
        )


if __name__ == "__main__":
    main()
