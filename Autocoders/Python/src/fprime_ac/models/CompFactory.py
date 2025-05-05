#!/usr/bin/env python3
# ===============================================================================
# NAME: CompFactory.py
#
# DESCRIPTION: This is a factory class for instancing the Component
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
import sys

from fprime_ac.models import (
    Arg,
    Channel,
    Command,
    Component,
    Event,
    InternalInterface,
    Parameter,
    Port,
    Serialize,
)

# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")


class CompFactory:
    """
    This is a concrete factory method object used to create the interface
    to the code generation.  A single call to create is made that returns
    the component object containing all the ports, includes, args, etc.
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
        if CompFactory.__instance is None:
            CompFactory.__instance = CompFactory()

        return CompFactory.__instance

    # define static method
    getInstance = staticmethod(getInstance)

    def create(
        self, the_parsed_component_xml, parsed_port_xml_list, parsed_serializable_list
    ):
        """
        Create a component model here.
        """
        x = the_parsed_component_xml
        comp_obj = x.get_component()
        comp_port_obj_list = x.get_ports()
        comp_command_obj_list = x.get_commands()
        comp_channel_obj_list = x.get_channels()
        comp_parameter_obj_list = x.get_parameters()
        comp_event_obj_list = x.get_events()
        comp_internal_interface_obj_list = x.get_internal_interfaces()
        comp_included_enums_list = x.get_enum_type_files()

        #
        comp_namespace = comp_obj.get_namespace()
        comp_name = comp_obj.get_name()
        comp_kind = comp_obj.get_kind()
        comp_comment = comp_obj.get_comment()
        comp_modeler = comp_obj.get_modeler()
        if comp_namespace is None:
            comp_full_name = comp_name
        else:
            comp_full_name = comp_namespace + "::" + comp_name
        # get original filename here...
        comp_xml_filename = x.get_xml_filename()
        #
        comp_xml_port_files = x.get_port_type_files()
        comp_c_header_files = x.get_header_files()
        has_guarded_ports = False

        num_async_ports = 0
        num_sync_ports = 0  # includes guarded ports

        #
        # print ("Component: %s"%comp_name)
        incl_list = []
        #
        # Create list of ports with all ports of the component.
        #
        port_obj_list = []
        for port_obj in comp_port_obj_list:
            n = port_obj.get_name()
            t = port_obj.get_type()
            d = port_obj.get_direction()
            s = port_obj.get_sync()
            r = port_obj.get_role()
            if s in ("sync", "guarded"):
                num_sync_ports += 1
            if s == "async":
                num_async_ports += 1
            p = port_obj.get_priority()
            if s == "guarded":
                has_guarded_ports = True
            c = port_obj.get_comment()
            m = port_obj.get_max_number()
            f = port_obj.get_full()
            port_obj_list.append(Port.Port(n, t, d, s, p, f, c, max_number=m, role=r))
        command_obj_list = []
        for command_obj in comp_command_obj_list:
            m = command_obj.get_mnemonic()
            o = command_obj.get_opcodes()
            s = command_obj.get_sync()
            p = command_obj.get_priority()
            f = command_obj.get_full()
            if s == "guarded":
                has_guarded_ports = True
            if s in ("sync", "guarded"):
                num_sync_ports += 1
            if s == "async":
                num_async_ports += 1
            c = command_obj.get_comment()
            arg_obj_list = []
            for a in command_obj.get_args():
                name = a.get_name()
                atype = a.get_type()
                comment = a.get_comment()
                size = a.get_size()
                arg_obj_list.append(Arg.Arg(name, atype, None, size, comment))
            command_obj_list.append(
                Command.Command(
                    m,
                    o,
                    arg_obj_list,
                    s,
                    p,
                    c,
                    comp_xml_filename,
                    comp_full_name,
                    component_base_name=comp_name,
                    base_opcode=command_obj.get_base_opcode(),
                    full=f,
                )
            )

        channel_obj_list = []
        for channel_obj in comp_channel_obj_list:
            i = channel_obj.get_ids()
            n = channel_obj.get_name()
            t = channel_obj.get_type()
            s = channel_obj.get_size()
            c = channel_obj.get_comment()
            a = channel_obj.get_abbrev()
            f = channel_obj.get_format_string()
            u = channel_obj.get_update()
            l = channel_obj.get_limits()
            channel_obj_list.append(
                Channel.Channel(
                    ids=i,
                    name=n,
                    ctype=t,
                    size=s,
                    abbrev=a,
                    format_string=f,
                    update=u,
                    limits=l,
                    comment=c,
                    xml_filename=comp_xml_filename,
                    component_name=comp_full_name,
                    component_base_name=comp_name,
                )
            )

        event_obj_list = []
        for event_obj in comp_event_obj_list:
            i = event_obj.get_ids()
            n = event_obj.get_name()
            s = event_obj.get_severity()
            f = event_obj.get_format_string()
            t = event_obj.get_throttle()
            c = event_obj.get_comment()
            arg_obj_list = []
            for a in event_obj.get_args():
                name = a.get_name()
                atype = a.get_type()
                size = a.get_size()
                comment = a.get_comment()
                arg_obj_list.append(Arg.Arg(name, atype, None, size, comment))
            event_obj_list.append(
                Event.Event(
                    i,
                    n,
                    s,
                    f,
                    t,
                    arg_obj_list,
                    c,
                    comp_xml_filename,
                    comp_full_name,
                    component_base_name=comp_name,
                )
            )

        internal_interface_obj_list = []
        for internal_interface_obj in comp_internal_interface_obj_list:
            # borrow this for check
            num_async_ports += 1
            n = internal_interface_obj.get_name()
            p = internal_interface_obj.get_priority()
            f = internal_interface_obj.get_full()
            c = internal_interface_obj.get_comment()
            arg_obj_list = []
            for a in internal_interface_obj.get_args():
                name = a.get_name()
                atype = a.get_type()
                size = a.get_size()
                comment = a.get_comment()
                arg_obj_list.append(Arg.Arg(name, atype, None, size, comment))
            internal_interface_obj_list.append(
                InternalInterface.InternalInterface(
                    n, p, f, arg_obj_list, c, comp_xml_filename, comp_full_name
                )
            )

        parameter_obj_list = []
        for parameter_obj in comp_parameter_obj_list:
            i = parameter_obj.get_ids()
            n = parameter_obj.get_name()
            t = parameter_obj.get_type()
            set_ops = parameter_obj.get_set_opcodes()
            save_ops = parameter_obj.get_save_opcodes()
            d = parameter_obj.get_default()
            s = parameter_obj.get_size()
            c = parameter_obj.get_comment()
            parameter_obj_list.append(
                Parameter.Parameter(
                    i,
                    n,
                    t,
                    set_ops,
                    save_ops,
                    d,
                    s,
                    c,
                    comp_xml_filename,
                    comp_full_name,
                    base_setop=parameter_obj.get_base_setop(),
                    base_saveop=parameter_obj.get_base_saveop(),
                )
            )

        serializable_obj_list = []
        for serializable_obj in parsed_serializable_list:
            f = serializable_obj.get_xml_filename()
            n = serializable_obj.get_name()
            ns = serializable_obj.get_namespace()
            c = serializable_obj.get_comment()
            x = serializable_obj.get_includes()
            # shouldn't be c includes
            m = serializable_obj.get_members()
            t = serializable_obj.get_typeid()
            serializable_obj_list.append(
                Serialize.Serialize(f, n, ns, c, x, None, m, t)
            )

        #
        # Check here to make sure all the port types in the component XML
        # exist in the port XMLs
        #
        interface_xml_list = [
            parsed_port_obj.get_interface().get_name()
            for parsed_port_obj in parsed_port_xml_list
        ]
        for port_obj in port_obj_list:
            t = port_obj.get_type()
            ## Skip if special port. (If there role)
            ## Namespaces for special ports are set above
            if (
                (t not in interface_xml_list)
                and (t.lower() != "serial")
                and (port_obj.get_role() is None)
            ):
                PRINT.info(
                    "ERROR: Missing port type definition in component XML (name: %s, type: %s)"
                    % (port_obj.get_name(), t)
                )
                sys.exit(-1)
        #
        # Check here to make sure all the port types in the component XML
        # exist in the port XMLs
        #

        #        interface_xml_list = [parsed_command_obj.get_interface().get_name() for parsed_command_obj in parsed_command_xml_list]
        #        print interface_xml_list
        #        for command_obj in command_obj_list:
        #            t = command_obj.get_type()
        #            if (t not in interface_xml_list):
        #                PRINT.info("ERROR: Missing command type definition in component XML (name: %s, type: %s)" % (command_obj.get_type(),t))
        #                sys.exit(-1)
        #       #
        # Add port type specifics to port object.
        # Specifics are things like: args, includes, etc.
        for port_obj in port_obj_list:

            for parsed_port_obj in parsed_port_xml_list:
                # print "Meta: Name: %s, Type: %s" % (port_obj.get_name(), port_obj.get_type())
                # print "Meta: Port Type: %s, Port Interface: %s" % (port_obj.get_type(),parsed_port_obj.get_interface().get_name())

                if port_obj.get_type() == parsed_port_obj.get_interface().get_name():
                    arg_obj_list = []
                    incl_list = parsed_port_obj.get_include_header_files()
                    namespace = parsed_port_obj.get_interface().get_namespace()
                    if_comment = parsed_port_obj.get_interface().get_comment()
                    return_type = parsed_port_obj.get_interface().get_return_type()
                    return_modifier = (
                        parsed_port_obj.get_interface().get_return_modifier()
                    )

                    for a in parsed_port_obj.get_args():
                        name = a.get_name()
                        atype = a.get_type()
                        comment = a.get_comment()
                        modifier = a.get_modifier()
                        size = a.get_size()
                        arg_obj_list.append(
                            Arg.Arg(name, atype, modifier, size, comment)
                        )
                    port_obj.set(
                        namespace, arg_obj_list, incl_list, None, None, if_comment
                    )
                    port_obj.set_return(return_type, return_modifier)
                    # check some rules
                    # 1) No return values for async ports
                    if (port_obj.get_sync() == "async") and (return_type is not None):
                        PRINT.info(
                            'ERROR: %s: Port "%s" cannot be asynchronous and have a return value'
                            % (
                                the_parsed_component_xml.get_xml_filename(),
                                port_obj.get_name(),
                            )
                        )
                        sys.exit(-1)
                    # 2) Serial ports can't have roles
                    if (port_obj.get_type() == "Serial") and (
                        port_obj.get_role() is not None
                    ):
                        PRINT.info(
                            'ERROR: %s: Port "%s" cannot have a role and be a serialized port'
                            % (
                                the_parsed_component_xml.get_xml_filename(),
                                port_obj.get_name(),
                            )
                        )
                        sys.exit(-1)

        # check some component/port rules
        # 1) Active or queued need at least one async port/command
        if comp_kind in ("active", "queued"):
            if num_async_ports == 0 and len(parameter_obj_list) == 0:
                PRINT.info(
                    'ERROR: %s: Active/Queued component "%s" needs at least one async port, command, or interface'
                    % (the_parsed_component_xml.get_xml_filename(), comp_name)
                )
                sys.exit(-1)
        # 2) Queued component needs at least one sync port/command
        if comp_kind == "queued":
            if num_sync_ports == 0:
                PRINT.info(
                    'ERROR: %s: Queued component "%s" needs at least one sync/guarded port or command'
                    % (the_parsed_component_xml.get_xml_filename(), comp_name)
                )
                sys.exit(-1)

        parsed_array_list = []
        for array_file in the_parsed_component_xml.get_array_type_files():
            parsed_array_list.append(array_file.replace("Ai.xml", "Ac.hpp"))

        #
        # Instance the component here...
        #
        the_component = Component.Component(
            comp_namespace,
            comp_name,
            comp_kind,
            comp_comment,
            comp_modeler,
            port_obj_list,
            command_obj_list,
            channel_obj_list,
            parameter_obj_list,
            event_obj_list,
            internal_interface_obj_list,
            serializable_obj_list,
            comp_xml_filename,
            comp_included_enums_list,
        )
        the_component.set_xml_port_files(comp_xml_port_files + parsed_array_list)
        the_component.set_xml_serializable_files(
            the_parsed_component_xml.get_serializable_type_files()
        )
        the_component.set_c_header_files(comp_c_header_files)
        if has_guarded_ports:
            the_component.set_has_guarded_ports()

        # for p in the_component.get_ports():
        #    print p.get_name(), p.get_namespace()
        #    for a in p.get_args():
        #        print a.get_name(), a.get_type(), a.get_modifier()

        return the_component
