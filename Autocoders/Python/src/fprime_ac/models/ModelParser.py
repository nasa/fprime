#
# ===============================================================================
# NAME: ModelParser.py
#
# DESCRIPTION: This is a simple meta-model parsing class.  It contains
#              various routines for parsing the meta-model object and
#              returning useful lists, dicts, etc. for visitors to
#              utilize in mapping to code.
#
# AUTHOR: Leonard J. Reder
#
# EMAIL:  reder@jpl.nasa.gov
#
# DATE CREATED  : 24 May 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.utils import TypesList


class ModelParser:
    """
    This class provides a single entry point returning items from
    the meta-model configuration used for component code generation.
    """

    __instance = None

    def __init__(self):
        """
        Constructor.
        """

    def getInstance():
        """
        Return instance of singleton.
        """
        if ModelParser.__instance is None:
            ModelParser.__instance = ModelParser()
        return ModelParser.__instance

    # define static method
    getInstance = staticmethod(getInstance)

    def uniqueList(self, seq):
        """
        Make a list of duplicates all unique.
        """
        seen = set()
        seen_add = seen.add
        return [x for x in seq if x not in seen and not seen_add(x)]

    def getPortsList(self, obj):
        """
        Return list of ports as (instance_name, type, direction, role) tuples.
        """
        port_instance_name_list = []
        for port in obj.get_ports():
            i = port.get_name()
            t = port.get_type()
            d = port.get_direction()
            d = d[0].upper() + d[1:].lower()
            r = port.get_role()
            port_instance_name_list.append((i, t, d, r))
        return port_instance_name_list

    def getPortsListMaxNum(self, obj):
        """
        Return list of ports as (instance_name, type, direction, max_number, role) tuples.
        """
        port_instance_name_list = []
        for port in obj.get_ports():
            i = port.get_name()
            t = port.get_type()
            d = port.get_direction()
            d = d[0].upper() + d[1:].lower()
            m = port.get_max_number()
            r = port.get_role()
            port_instance_name_list.append((i, t, d, m, r))
        return port_instance_name_list

    def getPortsListSync(self, obj):
        """
        Return list of ports as (instance_name, type, direction, sync, priority, role) tuples.
        """
        port_instance_name_list = []
        for port in obj.get_ports():
            i = port.get_name()
            t = port.get_type()
            d = port.get_direction()
            d = d[0].upper() + d[1:].lower()
            s = port.get_sync()
            p = port.get_priority()
            r = port.get_role()
            port_instance_name_list.append((i, t, d, s, p, r))
        return port_instance_name_list

    def getPortsListAll(self, obj):
        """
        Return list of ports as (instance_name, type, direction, sync, priority, full, role, max_number) tuples.
        """
        result = []
        for port in obj.get_ports():
            i = port.get_name()
            t = port.get_type()
            d = port.get_direction()
            d = d[0].upper() + d[1:].lower()
            s = port.get_sync()
            p = port.get_priority()
            f = port.get_full()
            r = port.get_role()
            m = port.get_max_number()
            result.append((i, t, d, s, p, f, r, m))
        return result

    def getCommandsListSync(self, obj):
        """
        Return list of commands with mnemonic and sync
        """
        command_instance_name_list = []
        for command in obj.get_commands():
            m = command.get_mnemonic()
            s = command.get_sync()
            p = command.get_priority()
            f = command.get_full()
            command_instance_name_list.append((m, s, p, f))
        return command_instance_name_list

    def _getPortArgsNamespacePrefix(self, comp_namespace, port_namespace, arg_type):
        """
        Return the arg_type prefixed by <namespace>::arg_type using namespace rules.
        """
        if comp_namespace == port_namespace:
            return arg_type
        # U8 * or char * pointer types
        #        elif arg_type.split(" ")[0] in ['U8','char']:
        #            if arg_type.split(" ")[1] =='*':
        #                return arg_type
        # Basic types
        elif arg_type in (TypesList.types_list + TypesList.port_types_list):
            return arg_type
        elif arg_type == "Serial":
            return arg_type
        # Type already has specific namespace preffixed
        elif "::" in arg_type:
            return arg_type
        else:
            return port_namespace + "::" + arg_type

    def getPortReturnDict(self, obj):
        """
        Return a dict of return type strings, keyed by port name.
        If no return type it is 'void '.   Append modifiers to return types.
        """
        ret_dict = dict()
        for port in obj.get_ports():
            name = port.get_name()
            ret_tuple = port.get_return()
            ret_dict[name] = "void "
            # print "Port: %s" % name
            if ret_tuple is not None:
                t = ret_tuple[0]
                # check for enumeration
                if isinstance(t, tuple):
                    t = t[0][1]
                    # append namespace
                    if port.get_namespace() is not None:
                        t = port.get_namespace() + "::" + t
                m = ret_tuple[1]
                if m == "pointer":
                    m = "*"
                elif m == "reference":
                    m = "&"
                else:
                    m = ""
                ret_dict[name] = t + " " + m
        return ret_dict

    def getPortArgsDict(self, obj):
        """
        Return a dict of list of args, keyed by port name
        (e.g. arg_dict['Port name'] => [(name,type,comment), (name,type,comment), ...]
        """
        args_dict = dict()
        comp_namespace = obj.get_namespace()
        for port in obj.get_ports():
            name = port.get_name()
            port_namespace = port.get_namespace()
            args_dict[name] = list()
            # print "Port: %s" % name
            args = port.get_args()
            for a in args:
                n = a.get_name()
                t = a.get_type()
                m = a.get_modifier()
                e = None
                #
                # Store modifier as language symblol
                if m == "pointer":
                    m = "*"
                elif m == "reference":
                    m = "&"
                else:
                    m = ""

                if t == "string":
                    t = n + "String"

                if t == "buffer":
                    t = n + "Buffer"
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        t = t[0][1]
                        e = "ENUM"
                    else:
                        print("ERROR: Expected ENUM type in component args list...")
                        sys.exit(-1)
                c = a.get_comment()
                # Add namespace to type here...
                t = self._getPortArgsNamespacePrefix(comp_namespace, port_namespace, t)
                # If serialize type set it here...
                # if t == "Serial":
                #    n = "Buffer"
                #    t = "Fw::SerializeBufferBase"
                #    c = ""
                #    m = "&"
                # print "Name %s : Type %s" % (n,t)
                args_dict[name].append((n, t, c, m, e))
        return args_dict

    def getPortArgsPrototypeStringDict(self, obj):
        """
        Return a dict of prototype string args signature, keyed by port name
        (.e.g. arg_dict['Port name'] => "type1 name1, type2, name2 ..."
        """
        d = self.getPortArgsDict(obj)
        d2 = dict()
        for l in d:
            if len(d[l]) > 0:
                d2[l] = ", ".join(["{} {}{}".format(x[1], x[3], x[0]) for x in d[l]])
            else:
                d2[l] = "void"
        return d2

    def getPortArgsCallStringDict(self, obj):
        """
        Return a dict of call string args signature, keyed by port name
        (.e.g. arg_dict['Port name'] => "name1, name2 ..."
        """
        d = self.getPortArgsDict(obj)
        d2 = dict()
        for l in d:
            if len(d[l]) > 0:
                d2[l] = ", ".join(["%s" % x[0] for x in d[l]])
            else:
                d2[l] = ""
        return d2

    def getMsgTypeArgsDict(self, obj, msg_types_list):
        """
        Return a dict of list of args, keyed by port type name (in all CAPS)
        (e.g. msg_type_arg_dict['Msg type name'] => [(name,type,comment),(name,type,comment),...]
        """
        msg_type_arg_dict = dict()
        port_instance_name_list = self.getPortsList(obj)
        args_dict = self.getPortArgsDict(obj)
        #
        for msg_type in msg_types_list:
            for port in port_instance_name_list:
                if msg_type == port[1].upper():
                    msg_type_arg_dict[msg_type] = args_dict[port[0]]
        return msg_type_arg_dict

    def getPortNamespaceTypeDict(self, obj):
        """
        Return a dict of port namespace, keyed on port type.
        """
        # Build list of unique port types here...
        port_types = []
        for p in obj.get_ports():
            port_types.append(p.get_type())
        port_types = self.uniqueList(port_types)
        # Build a dictionary of namespaces keyed on port type here...
        port_namespace_dict = dict()
        for p in port_types:
            for p2 in obj.get_ports():
                if p == p2.get_type():
                    port_namespace_dict[p] = p2.get_namespace()
        # Now for any Serial port types make sure namespace is 'Fw'...
        if "Serial" in list(port_namespace_dict.keys()):
            port_namespace_dict["Serial"] = "Fw"
        #
        return port_namespace_dict

    def hasSerializablePort(self, obj):
        """
        Tests the list of ports in the model and if any one is a Serializable returns true
        else returns false.
        """
        return "Serial" in [p.get_type() for p in obj.get_ports()]

    def hasSyncPort(self, obj):
        """
        Tests the list of ports in the model and if any one is a sync returns true
        else returns false.
        """
        return "sync" in [p.get_sync() for p in obj.get_ports()]

    def getCommandsList(self, obj):
        """
        Return list of command names based on mnemonic, opcode, and sync
        """
        command_instance_name_list = []
        for command in obj.get_commands():
            m = command.get_mnemonic()
            o = command.get_opcodes()
            s = command.get_sync()
            p = command.get_priority()
            f = command.get_full()
            c = command.get_comment()

            command_instance_name_list.append((m, o, s, p, f, c))
        return command_instance_name_list

    def getEventsList(self, obj):
        """
        Return list of event names based on id, name, severity, format_string, and comments
        """
        event_instance_name_list = []
        for event in obj.get_events():
            i = event.get_ids()
            n = event.get_name()
            s = event.get_severity()
            f = event.get_format_string()
            t = event.get_throttle()
            c = event.get_comment()

            event_instance_name_list.append((i, n, s, f, t, c))
        return event_instance_name_list

    def getTelemEnumList(self, obj):
        enum_list = []
        for channel in obj.get_channels():
            t = channel.get_type()
            #
            # Check and get enum type here...
            if isinstance(t, tuple):
                if t[0][0].upper() == "ENUM":
                    enum_type = t[0][1]
                    mem_list = t[1]
                    member_lines = []
                    for member in mem_list:
                        if member[1] is not None:
                            mem_init = " = %s" % member[1]
                        else:
                            mem_init = ""
                        if member[2] is not None:
                            mem_comment = " //<! %s" % member[2]
                        else:
                            mem_comment = ""
                        member_lines.append((member[0], mem_init, mem_comment))
                    enum_list.append((enum_type, member_lines))
                else:
                    print("ERROR: Expected ENUM type in telemetry args list...")
                    sys.exit(-1)
        return enum_list

    def getChannelsList(self, obj):
        """
        Return list of telemetry channels
        """
        channel_instance_name_list = []
        for channel in obj.get_channels():
            i = channel.get_ids()
            n = channel.get_name()
            t = channel.get_type()
            ti = None
            if isinstance(t, tuple):
                # rename type to enum type
                t = t[0][1]
                ti = "enum"
            elif t not in TypesList.types_list:
                ti = "user"
            s = channel.get_size()
            u = channel.get_update()
            c = channel.get_comment()

            channel_instance_name_list.append((i, n, t, s, u, c, ti))
        return channel_instance_name_list

    def getParamEnumList(self, obj):
        enum_list = []
        for parameter in obj.get_parameters():
            t = parameter.get_type()
            #
            # Check and get enum type here...
            if isinstance(t, tuple):
                if t[0][0].upper() == "ENUM":
                    enum_type = t[0][1]
                    mem_list = t[1]
                    member_lines = []
                    for member in mem_list:
                        if member[1] is not None:
                            mem_init = " = %s" % member[1]
                        else:
                            mem_init = ""
                        if member[2] is not None:
                            mem_comment = " //<! %s" % member[2]
                        else:
                            mem_comment = ""
                        member_lines.append((member[0], mem_init, mem_comment))
                    enum_list.append((enum_type, member_lines))
                else:
                    print("ERROR: Expected ENUM type in telemetry args list...")
                    sys.exit(-1)
        return enum_list

    def getParametersList(self, obj):
        """
        Return list of parameters
        """
        parameter_instance_name_list = []
        for parameter in obj.get_parameters():
            i = parameter.get_ids()
            n = parameter.get_name()
            t = parameter.get_type()
            set_ops = parameter.get_set_opcodes()
            save_ops = parameter.get_save_opcodes()
            ti = None
            if isinstance(t, tuple):
                # rename type to enum type
                t = t[0][1]
                ti = "enum"
            s = parameter.get_size()
            d = parameter.get_default()
            c = parameter.get_comment()

            parameter_instance_name_list.append(
                (i, n, t, set_ops, save_ops, s, d, c, ti)
            )
        return parameter_instance_name_list

    def getEnumList(self, obj):
        enum_list = []
        for command in obj.get_commands():
            args = command.get_args()
            for a in args:
                t = a.get_type()
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        enum_type = t[0][1]
                        mem_list = t[1]
                        member_lines = []
                        for member in mem_list:
                            if member[1] is not None:
                                mem_init = " = %s" % member[1]
                            else:
                                mem_init = ""
                            if member[2] is not None:
                                mem_comment = " //<! %s" % member[2]
                            else:
                                mem_comment = ""
                            member_lines.append((member[0], mem_init, mem_comment))
                        enum_list.append((enum_type, member_lines))
                    else:
                        print("ERROR: Expected ENUM type in component args list...")
                        sys.exit(-1)
        return enum_list

    def getCommandArgsDict(self, obj, from_proto=False):
        """
        Return a dict of list of args, keyed by command mnemonic
        (e.g. arg_dict['mnemonic'] => [(name,type,comment), (name,type,comment), ...]
        """
        args_dict = dict()
        for command in obj.get_commands():
            mnemonic = command.get_mnemonic()
            args_dict[mnemonic] = list()
            # print "Command: %s" % name
            args = command.get_args()

            for a in args:
                n = a.get_name()
                t = a.get_type()
                typeinfo = None
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        t = t[0][1]
                        typeinfo = "enum"
                    else:
                        print("ERROR: Expected ENUM type in command args list...")
                        sys.exit(-1)
                elif t == "string":
                    if from_proto:
                        t = "const Fw::CmdStringArg&"
                    else:
                        t = "Fw::CmdStringArg"
                    typeinfo = "string"
                c = a.get_comment()
                # Add namespace to type here...
                # If serialize type set it here...
                # if t == "Serial":
                #    n = "Buffer"
                #    t = "Fw::SerializeBufferBase"
                #    c = ""
                #    m = "&"
                # print "Name %s : Type %s" % (n,t)
                args_dict[mnemonic].append((n, t, c, typeinfo))
        return args_dict

    def getCommandArgsPrototypeStringDict(self, obj):
        """
        Return a dict of prototype string args signature, keyed by command mnemonic
        (.e.g. arg_dict['mnemonic'] => "type1 name1, type2, name2 ..."
        """
        d = self.getCommandArgsDict(obj, True)
        d2 = dict()
        for l in d:
            if len(d[l]) > 0:
                d2[l] = ", ".join(["{} {}".format(x[1], x[0]) for x in d[l]])
            else:
                d2[l] = "void"
        return d2

    def getEventEnumList(self, obj):
        enum_list = []
        for event in obj.get_events():
            args = event.get_args()
            for a in args:
                t = a.get_type()
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        enum_type = t[0][1]
                        mem_list = t[1]
                        member_lines = []
                        for member in mem_list:
                            if member[1] is not None:
                                mem_init = " = %s" % member[1]
                            else:
                                mem_init = ""
                            if member[2] is not None:
                                mem_comment = " //<! %s" % member[2]
                            else:
                                mem_comment = ""
                            member_lines.append((member[0], mem_init, mem_comment))
                        enum_list.append((enum_type, member_lines))
                    else:
                        print("ERROR: Expected ENUM type in component args list...")
                        sys.exit(-1)
        return enum_list

    def getEventArgsDict(self, obj):
        """
        Return a dict of list of args, keyed by name mnemonic
        (e.g. arg_dict['name'] => [(name,type,comment), (name,type,comment), ...]
        """
        args_dict = dict()
        for event in obj.get_events():
            name = event.get_name()
            args_dict[name] = list()
            # print "Command: %s" % name
            args = event.get_args()

            for a in args:
                n = a.get_name()
                t = a.get_type()
                typeinfo = None
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        t = t[0][1]
                    else:
                        print("ERROR: Expected ENUM type in event args list...")
                        sys.exit(-1)
                    typeinfo = "enum"
                elif t == "string":
                    t = "Fw::LogStringArg&"
                    typeinfo = "string"
                elif t not in TypesList.types_list:
                    typeinfo = "user"
                c = a.get_comment()
                s = a.get_size()
                # Add namespace to type here...
                # If serialize type set it here...
                # if t == "Serial":
                #    n = "Buffer"
                #    t = "Fw::SerializeBufferBase"
                #    c = ""
                #    m = "&"
                # print "Name %s : Type %s" % (n,t)
                args_dict[name].append((n, t, c, s, typeinfo))
        return args_dict

    def getEventArgsPrototypeStringDict(self, obj):
        """
        Return a dict of prototype string args signature, keyed by port name
        (.e.g. arg_dict['mnemonic'] => "type1 name1, type2, name2 ..."
        """
        d = self.getEventArgsDict(obj)
        d2 = dict()
        for l in d:
            if len(d[l]) > 0:
                d2[l] = ", ".join(["{} {}".format(x[1], x[0]) for x in d[l]])
            else:
                d2[l] = "void"
        return d2

    def getInternalInterfacesList(self, obj):
        """
        Return list of interface names
        """
        internal_interface_instance_name_list = []
        for internal_interface in obj.get_internal_interfaces():
            n = internal_interface.get_name()
            p = internal_interface.get_priority()
            f = internal_interface.get_full()
            internal_interface_instance_name_list.append((n, p, f))
        return internal_interface_instance_name_list

    def getInternalInterfaceArgsPrototypeStringDict(self, obj):
        """
        Return a dict of prototype string args signature, keyed by interface name
        (.e.g. arg_dict['name'] => "type1 name1, type2, name2 ..."
        """
        d = self.getInternalInterfaceArgsDict(obj, True)
        d2 = dict()
        for l in d:
            if len(d[l]) > 0:
                d2[l] = ", ".join(["{} {}".format(x[1], x[0]) for x in d[l]])
            else:
                d2[l] = "void"
        return d2

    def getInternalInterfaceArgsDict(self, obj, from_proto=False):
        """
        Return a dict of list of args, keyed by internal interface name
        (e.g. arg_dict['name'] => [(name,type,comment), (name,type,comment), ...]
        """
        args_dict = dict()
        for internal_interface in obj.get_internal_interfaces():
            name = internal_interface.get_name()
            args_dict[name] = list()
            # print "Interface: %s" % name
            args = internal_interface.get_args()

            for a in args:
                n = a.get_name()
                t = a.get_type()
                typeinfo = None
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        t = t[0][1]
                        typeinfo = "enum"
                    else:
                        print(
                            "ERROR: Expected ENUM type in internal_interface args list..."
                        )
                        sys.exit(-1)
                elif t == "string":
                    if from_proto:
                        t = "const Fw::InternalInterfaceString&"
                    else:
                        t = "Fw::InternalInterfaceString"
                    typeinfo = "string"
                elif t in TypesList.types_list + TypesList.port_types_list:
                    pass
                else:
                    if from_proto:
                        t = "%s&" % t
                    typeinfo = "user"
                c = a.get_comment()
                args_dict[name].append((n, t, c, typeinfo))
        return args_dict

    def getInternalInterfaceEnumList(self, obj):
        enum_list = []
        for internal_interface in obj.get_internal_interfaces():
            args = internal_interface.get_args()
            for a in args:
                t = a.get_type()
                #
                # Check and get enum type here...
                if isinstance(t, tuple):
                    if t[0][0].upper() == "ENUM":
                        enum_type = t[0][1]
                        mem_list = t[1]
                        member_lines = []
                        for member in mem_list:
                            if member[1] is not None:
                                mem_init = " = %s" % member[1]
                            else:
                                mem_init = ""
                            if member[2] is not None:
                                mem_comment = " //<! %s" % member[2]
                            else:
                                mem_comment = ""
                            member_lines.append((member[0], mem_init, mem_comment))
                        enum_list.append((enum_type, member_lines))
                    else:
                        print("ERROR: Expected ENUM type in interface args list...")
                        sys.exit(-1)
        return enum_list


if __name__ == "__main__":
    #
    # Quick tests.
    #
    pass
