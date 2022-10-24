# ===============================================================================
# NAME: ComponentVisitorBase.py
#
# DESCRIPTION: A base class for test code generation.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 24, 2015
#
# Based on code by Leonard Reder
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import datetime
import logging
import sys
from getpass import getuser

from fprime_ac.generators import formatters
from fprime_ac.generators.visitors import AbstractVisitor
from fprime_ac.models import ModelParser

#
# Python extension modules and custom interfaces
#
from fprime_ac.utils import ConfigManager

#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
LAST = 0
NOT_LAST = 1


class ComponentVisitorBase(AbstractVisitor.AbstractVisitor):
    """
    An abstract base class for unit test code generation.
    """

    __instance = None
    __config = None
    __fp = None
    __form = None
    __form_comment = None
    __model_parser = None
    __visitor = None

    def __init__(self):
        super().__init__()

    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug(f"ComponentVisitorBase:{visit_str}")
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        self.__fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def argsString(self, args):
        """
        Make a list of args into a string
        """
        return ", ".join(args)

    def buildFileName(self, obj):
        """
        Build the file name
        """
        if self.config("component", "XMLDefaultFileName") == "True":
            filename = (
                obj.get_namespace()
                + obj.get_name()
                + self.config("component", self.__visitor)
            )
            DEBUG.info(
                f"Generating code filename: {filename}, using XML namespace and name attributes..."
            )
        else:
            xml_file = obj.get_xml_filename()
            x = xml_file.split(".")
            s = self.config("component", "ComponentXML").split(".")
            l = len(s[0])
            if (x[0][-l:] == s[0]) & (x[1] == s[1]):
                filename = x[0].split(s[0])[0] + self.config(
                    "component", self.__visitor
                )
                DEBUG.info(f"Generating code filename: {filename}...")
            else:
                msg = f"XML file naming format not allowed (must be XXXComponentAi.xml), Filename: {xml_file}"
                PRINT.info(msg)
                raise ValueError(msg)
        return filename

    def commandArgsStr(self):
        """
        Make a list of command args into a string
        """

        def f(lst):
            def g(xxx_todo_changeme):
                (name, a, b, c) = xxx_todo_changeme
                return name

            return self.argsString(list(map(g, lst)))

        return f

    def config(self, a, b):
        return self.__config.get(a, b)

    def doxygenPostComment(self, comment):
        """
        Emit a doxygen post comment
        """
        if comment is None or comment == "":
            return ""
        else:
            return "/*!< " + comment + "*/"

    def doxygenPreComment(self, comment):
        """
        Emit a doxygen pre comment
        """
        if comment is None or comment == "":
            return ""
        else:
            return "/*! " + comment + "*/"

    def emitComment(self, comment):
        """
        Emit a comment
        """
        if comment is None or comment == "":
            return ""
        else:
            return "/* " + comment + "*/"

    def emitIndent(self, indent):
        str = ""
        for i in range(0, indent):
            str += " "
        return str

    def emitNonPortParamsCpp(self, indent, params):
        """
        Emit a list of non-port function parameters in a .cpp file
        """
        return self.emitParams(self.paramStrsCpp, indent, params)

    def emitNonPortParamsHpp(self, indent, params):
        """
        Emit a list of non-port function parameters in a .hpp file
        """
        return self.emitParams(self.paramStrsHpp, indent, params)

    def emitParam(self, kind, indent, paramStrs, param):
        """
        Emit a parameter
        """
        paramStr, commentStr = paramStrs(param)
        indentStr = self.emitIndent(indent)
        if kind == LAST:
            return indentStr + paramStr + commentStr
        else:
            return indentStr + paramStr + "," + commentStr + "\n"

    def emitParams(self, paramStrs, indent, params):
        """
        Emit a list of parameters (port or non-port)
        """
        length = len(params)
        if length == 0:
            return ""
        else:
            str = ""
            for i in range(0, length - 1):
                str += self.emitParam(NOT_LAST, indent, paramStrs, params[i])
            str += self.emitParam(LAST, indent, paramStrs, params[length - 1])
            return str

    def emitPortParamsCpp(self, indent, params):
        """
        Emit a list of port function parameters in a .cpp file
        """
        return self.emitParams(self.portParamStrsCpp, indent, params)

    def emitPortParamsHpp(self, indent, params):
        """
        Emit a list of port function parameters in a .hpp file
        """
        return self.emitParams(self.portParamStrsHpp, indent, params)

    def eventArgsStr(self):
        """
        Make a list of event args into a string
        """

        def f(args):
            def g(lst):
                name = lst[0]
                return name

            return self.argsString(list(map(g, args)))

        return f

    def finishSourceFilesVisit(self, obj):
        self.__fp.close()

    def fp(self):
        return self.__fp

    def includes1Visit(self, obj):
        pass

    def includes2Visit(self, obj):
        pass

    def init(self, obj, c):
        self.initPreamble(obj, c)
        self.initCommands(obj, c)
        self.initEvents(obj, c)
        self.initInternalInterfaces(obj, c)
        self.initIncludes(obj, c)
        # self.initParameters(obj, c)
        self.initPorts(obj, c)
        self.initTelemetry(obj, c)
        self.initParameters(obj, c)
        if c.kind == "passive":
            c.params_init_hpp = [c.param_instance_default_zero]
            c.params_init_cpp = [c.param_instance]
        elif c.needs_msg_size:
            c.params_init_hpp = [
                c.param_queueDepth,
                c.param_msgSize,
                c.param_instance_default_zero,
            ]
            c.params_init_cpp = [c.param_queueDepth, c.param_msgSize, c.param_instance]
        else:
            c.params_init_hpp = [c.param_queueDepth, c.param_instance_default_zero]
            c.params_init_cpp = [c.param_queueDepth, c.param_instance]

    def initBase(self, visitor):
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.__form = formatters.Formatters.getInstance()
        self.__form_comment = formatters.CommentFormatters()
        self.__model_parser = ModelParser.ModelParser.getInstance()
        self.__visitor = visitor
        DEBUG.info("ComponentVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def initCommandParams(self, obj, c):
        """
        Command function parameters for code generation
        """
        c.param_cmdSeq = ("cmdSeq", "const U32", "The command sequence number")
        c.param_opCode = ("opCode", "const FwOpcodeType", "The opcode")
        c.param_response = (
            "response",
            "const Fw::CmdResponse",
            "The command response",
        )

    def initCommands(self, obj, c):
        c.has_commands = len(obj.get_commands()) > 0
        c.commands = self.__model_parser.getCommandsList(obj)
        c.commands_sync = self.__model_parser.getCommandsListSync(obj)
        c.command_enums = self.__model_parser.getEnumList(obj)
        c.command_param_strs = self.__model_parser.getCommandArgsPrototypeStringDict(
            obj
        )
        c.command_args_str_dict = c.command_param_strs
        c.command_args = self.__model_parser.getCommandArgsDict(obj)
        c.command_params = self.__model_parser.getCommandArgsDict(obj, True)
        c.command_args_str = self.commandArgsStr()
        self.initCommandParams(obj, c)

    def initCompIncludePath(self, obj, c):
        """
        Configurable override for includes
        """
        relative_path = self.relativePath()
        if self.__config.get("includes", "comp_include_path") == "None":
            if relative_path is not None:
                c.comp_include_path = relative_path
            else:
                c.comp_include_path = obj.get_namespace()
        else:
            c.comp_include_path = self.__config.get("includes", "comp_include_path")
        c.include_path = c.comp_include_path

    def initEventParams(self, obj, c):
        """
        Event function parameters for code generation
        Some of these are also used for telemetry
        """
        c.param_event_id = ("id", "const FwEventIdType", "The event ID")
        c.param_timeTag = ("timeTag", "Fw::Time&", "The time")
        c.param_const_timeTag = ("timeTag", "const Fw::Time&", "The time")
        c.param_log_severity = ("severity", "const Fw::LogSeverity", "The severity")
        c.param_text_log_severity = (
            "severity",
            "const Fw::TextLogSeverity&",
            "The severity",
        )
        c.param_args = ("args", "Fw::LogBuffer&", "The serialized arguments")
        c.param_text = ("text", "const Fw::TextLogString&", "The event string")

    def initEvents(self, obj, c):
        c.has_events = len(obj.get_events()) > 0
        c.events = self.__model_parser.getEventsList(obj)
        c.event_enums = self.__model_parser.getEventEnumList(obj)
        c.event_args = self.__model_parser.getEventArgsDict(obj)
        c.event_params = c.event_args
        c.event_args_str = self.eventArgsStr()
        c.event_param_strs = self.__model_parser.getEventArgsPrototypeStringDict(obj)
        self.initEventParams(obj, c)

    def initIncludeName(self, obj, c):
        """
        Generate the header file name
        """
        if self.__config.get("component", "XMLDefaultFileName") == "False":
            c.include_name = obj.get_xml_filename().split("ComponentAi.xml")[0]
        else:
            c.include_name = c.name

    def initIncludes(self, obj, c):
        self.initTypeIncludes(obj, c)
        self.initPortIncludes(obj, c)
        self.initSerialIncludes(obj, c)
        self.initIncludeName(obj, c)
        self.initCompIncludePath(obj, c)

    def initInternalInterfaces(self, obj, c):
        c.has_internal_interfaces = len(obj.get_internal_interfaces()) > 0
        c.internal_interface_enums = self.__model_parser.getInternalInterfaceEnumList(
            obj
        )
        c.internal_interfaces = self.__model_parser.getInternalInterfacesList(obj)
        c.internal_interface_param_strs = (
            self.__model_parser.getInternalInterfaceArgsPrototypeStringDict(obj)
        )
        c.internal_interface_args_str_dict = c.internal_interface_param_strs
        c.internal_interface_args = self.__model_parser.getInternalInterfaceArgsDict(
            obj
        )
        c.internal_interface_args_str = self.internalInterfaceArgsStr()
        c.internal_interface_params = self.__model_parser.getInternalInterfaceArgsDict(
            obj, True
        )

    def initMsgTypes(self, obj, c):
        """
        Construct msg types list
        """

        def f(xxx_todo_changeme2):
            (instance, type, direction, sync, priority, role) = xxx_todo_changeme2
            if self.isInput(direction) and self.isAsync(sync):
                return instance.upper() + "_" + type.upper()
            else:
                return None

        port_types = self.mapPartial(f, c.ports_sync)

        def g(xxx_todo_changeme3):
            (mnemonic, opcodes, sync, priority, full, comment) = xxx_todo_changeme3
            if self.isAsync(sync):
                if len(opcodes) == 1:
                    return f"CMD_{mnemonic.upper()}"
                else:
                    return [
                        f"CMD_{mnemonic.upper()}_{inst}"
                        for inst, opcode in enumerate(opcodes)
                    ]
            else:
                return None

        cmd_types = self.mapPartial(g, c.commands)

        def h(xxx_todo_changeme4):
            (name, priority, full) = xxx_todo_changeme4
            return f"INT_IF_{name.upper()}"

        self.__model_parser.getInternalInterfacesList(obj)
        interface_types = self.mapPartial(h, c.internal_interfaces)

        c.msg_types = port_types + cmd_types + interface_types

    def initParameterParams(self, obj, c):
        """
        Parameter function parameters for code generation
        """
        c.param_valid = ("valid", "Fw::ParamValid", "The parameter valid flag")

    def initParameters(self, obj, c):
        c.has_parameters = len(obj.get_parameters()) > 0
        c.parameter_enums = self.__model_parser.getParamEnumList(obj)
        c.parameters = self.__model_parser.getParametersList(obj)
        self.initParameterParams(obj, c)

    def initPortFlags(self, obj, c):
        """
        Init port flags
        """
        # Do we have serializable ports?
        c.has_serializable_ports = self.__model_parser.hasSerializablePort(obj)
        # Do we have guarded ports?
        c.has_guarded_ports = obj.get_has_guarded_ports()
        # Do we need a message size?
        c.needs_msg_size = False
        for (name, type, direction, sync, priority, role) in c.ports_sync:
            if self.isSerial(type) and self.isAsync(sync):
                c.needs_msg_size = True
        # Flags for different port categories
        c.has_ports = len(c.ports) > 0
        c.has_input_ports = len(c.input_ports) > 0
        c.has_typed_input_ports = len(c.typed_input_ports) > 0
        c.has_serial_input_ports = len(c.serial_input_ports) > 0
        c.has_output_ports = len(c.output_ports) > 0
        c.has_typed_output_ports = len(c.typed_output_ports) > 0
        c.has_serial_output_ports = len(c.serial_output_ports) > 0
        roles = [
            role for name, ptype, sync, priority, role, max_number in c.output_ports
        ]
        c.has_time_get = "TimeGet" in roles

    def initPortIncludes(self, obj, c):
        c.port_includes = []
        for include in self.__model_parser.uniqueList(obj.get_xml_port_files()):
            c.port_includes.append(include.replace("PortAi.xml", "PortAc.hpp"))

    def initPortInputTypes(self, obj, c):
        """
        Construct port input type list
        """

        def f(xxx_todo_changeme5):
            (instance, type, direction, sync, priority, role) = xxx_todo_changeme5
            if self.isSerial(type):
                # No serial ports
                return None
            elif not self.isAsync(sync):
                # Only async input ports
                return None
            else:
                return type

        l = self.mapPartial(f, c.ports_sync)
        c.port_input_types = self.__model_parser.uniqueList(l)

    def initPortLists(self, obj, c):
        """
        Construct port lists
        """
        # Construct (instance, type, direction, role) list
        c.ports = self.__model_parser.getPortsList(obj)
        # Construct (instance, type, direction, sync, priority, role, max num) list
        c.ports_all = self.__model_parser.getPortsListAll(obj)
        # Construct (instance, type, direction, sync, priority, role) list
        c.ports_sync = self.__model_parser.getPortsListSync(obj)
        # Construct (instance, type, direction, max num, role) list
        c.ports_max_num = self.__model_parser.getPortsListMaxNum(obj)
        # Construct port input type list
        self.initPortInputTypes(obj, c)
        # Construct port type list
        l = [p.get_type() for p in obj.get_ports()]
        c.port_types = self.__model_parser.uniqueList(l)
        # Construct msg type list
        self.initMsgTypes(obj, c)
        # Init input ports
        self.initInputPorts(c)
        # Init output ports
        self.initOutputPorts(c)
        # Init handlers
        self.initHandlers(c)
        # Init pre-message hooks
        self.initPreMessageHooks(c)
        # Init invocation functions
        self.initInvocationFunctions(c)
        # Init message calls
        self.initMessageCalls(c)

    def initInputPorts(self, c):
        """
        Init input ports
        """
        # All input ports
        c.input_ports = [
            (instance, type, sync, priority, full, role, max_num)
            for (
                instance,
                type,
                direction,
                sync,
                priority,
                full,
                role,
                max_num,
            ) in c.ports_all
            if direction == "Input"
        ]
        # User input ports
        c.user_input_ports = [
            (instance, type, sync, priority, full, role, max_num)
            for (instance, type, sync, priority, full, role, max_num) in c.input_ports
            if role is None
        ]
        # Serial input ports
        c.serial_input_ports = [
            (instance, sync, priority, full, max_num)
            for (instance, type, sync, priority, full, role, max_num) in c.input_ports
            if type == "Serial"
        ]
        # Typed input ports
        c.typed_input_ports = [
            (instance, type, sync, priority, full, role, max_num)
            for (instance, type, sync, priority, full, role, max_num) in c.input_ports
            if type != "Serial"
        ]
        # Typed user input ports
        c.typed_user_input_ports = [
            (instance, type, sync, priority, full, role, max_num)
            for (
                instance,
                type,
                sync,
                priority,
                full,
                role,
                max_num,
            ) in c.typed_input_ports
            if role is None
        ]

    def initOutputPorts(self, c):
        """
        Init output ports
        """
        # All output ports
        c.output_ports = [
            (instance, type, sync, priority, role, max_num)
            for (
                instance,
                type,
                direction,
                sync,
                priority,
                full,
                role,
                max_num,
            ) in c.ports_all
            if direction == "Output"
        ]
        # User output ports
        c.user_output_ports = [
            (instance, type, sync, priority, role, max_num)
            for (instance, type, sync, priority, role, max_num) in c.output_ports
            if role is None
        ]
        # Typed output ports
        c.typed_output_ports = [
            (instance, type, sync, priority, role, max_num)
            for (instance, type, sync, priority, role, max_num) in c.output_ports
            if type != "Serial"
        ]
        # Typed user output ports
        c.typed_user_output_ports = [
            (instance, type, sync, priority, role, max_num)
            for (instance, type, sync, priority, role, max_num) in c.typed_output_ports
            if role is None
        ]
        # Serial output ports
        c.serial_output_ports = [
            (instance, sync, priority, max_num)
            for (instance, type, sync, priority, role, max_num) in c.output_ports
            if type == "Serial"
        ]

    def initHandlers(self, c):
        """
        Handlers to implement for input ports
        """
        # Typed handlers
        c.handlers_typed = [
            (instance, type, sync, priority, full, role, max_num)
            for (
                instance,
                type,
                sync,
                priority,
                full,
                role,
                max_num,
            ) in c.typed_input_ports
            if role != "Cmd" and sync != "model"
        ]
        # Serial handlers
        c.handlers_serial = c.serial_input_ports

    def initPreMessageHooks(self, c):
        """
        Init Pre-message hooks
        """
        # Pre-message hooks for async input ports
        c.pre_message_hooks = [
            (instance, type, sync, priority, role, max_num)
            for (instance, type, sync, priority, full, role, max_num) in c.input_ports
            if role != "Cmd" and sync == "async"
        ]
        # Pre-message hooks for typed async input ports
        c.pre_message_hooks_typed = [
            (instance, type, sync, priority, full, role, max_num)
            for (
                instance,
                type,
                sync,
                priority,
                full,
                role,
                max_num,
            ) in c.typed_input_ports
            if role != "Cmd" and sync == "async"
        ]
        # Pre-message hooks for serial async input ports
        c.pre_message_hooks_serial = c.serial_input_ports

    def initInvocationFunctions(self, c):
        # Invocation functions for output ports
        c.invocation_functions = [
            (instance, type, sync, priority, role, max_num)
            for (instance, type, sync, priority, role, max_num) in c.output_ports
            if role is None
        ]
        # Invocation functions for typed output ports
        c.typed_invocation_functions = [
            (instance, type, sync, priority, role, max_num)
            for (instance, type, sync, priority, role, max_num) in c.typed_output_ports
            if role is None
        ]
        # Invocation functions for serial output ports
        c.serial_invocation_functions = c.serial_output_ports

    def initMessageCalls(self, c):
        """
        Calls for messages
        """
        # Message calls on typed input ports
        c.message_calls_typed = [
            (instance, type, sync, priority, full, role, max_num)
            for (
                instance,
                type,
                sync,
                priority,
                full,
                role,
                max_num,
            ) in c.typed_input_ports
            if role != "Cmd"
        ]

    def initPortMaps(self, obj, c):
        """
        Construct port maps
        """
        # Construct map: port type -> namespace
        c.port_namespaces = self.__model_parser.getPortNamespaceTypeDict(obj)
        # Construct map: port name -> arg list
        c.port_args = self.__model_parser.getPortArgsDict(obj)
        c.port_params = c.port_args
        # Construct map: port name -> args string
        c.port_arg_strs = self.__model_parser.getPortArgsCallStringDict(obj)
        # Construct map: port name -> params string
        c.port_param_strs = self.__model_parser.getPortArgsPrototypeStringDict(obj)
        # Construct map: port name -> return type string
        c.port_return_type_strs = self.__model_parser.getPortReturnDict(obj)

    def initPortNames(self, obj, c):
        """
        Set special port names
        """
        for name, type, direction, role in c.ports:
            if role == "Cmd":
                c.Cmd_Name = name
            if role == "CmdRegistration":
                c.CmdReg_Name = name
            if role == "CmdResponse":
                c.CmdStatus_Name = name
            if role == "LogEvent":
                c.LogEvent_Name = name
            if role == "LogTextEvent":
                c.LogTextEvent_Name = name
            if role == "ParamGet":
                c.ParamGet_Name = name
            if role == "ParamSet":
                c.ParamSet_Name = name
            if role == "Telemetry":
                c.Tlm_Name = name
            if role == "TimeGet":
                c.Time_Name = name

    def initPortParams(self, obj, c):
        """
        Port function parameters for code generation
        """
        c.param_portNum = ("portNum", "const NATIVE_INT_TYPE", "The port number", "")
        c.param_Buffer = (
            "Buffer",
            "Fw::SerializeBufferBase",
            "The serialization buffer",
            "&",
        )
        c.param_callComp = (
            "callComp",
            "Fw::PassiveComponentBase",
            "The component instance",
            "*const ",
        )
        c.emit_port_params_hpp = self.emitPortParamsHpp
        c.emit_port_params = c.emit_port_params_hpp
        c.emit_port_params_cpp = self.emitPortParamsCpp

    def initPorts(self, obj, c):
        """
        Initialize port info
        """
        self.initPortLists(obj, c)
        self.initPortMaps(obj, c)
        self.initPortFlags(obj, c)
        self.initPortNames(obj, c)
        self.initPortParams(obj, c)

    def initPreamble(self, obj, c):
        d = datetime.datetime.now()
        c.date = d.strftime("%A, %d %B %Y")
        c.kind = obj.get_kind()
        c.modeler = obj.get_modeler()
        c.name = obj.get_name
        c.component_base = c.name() + "ComponentBase"
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        c.user = getuser()
        c.args_string = self.argsString
        c.doxygen_pre_comment = self.doxygenPreComment
        c.doxygen_post_comment = self.doxygenPostComment
        c.is_primitive_type = self.isPrimitiveType
        c.emit_non_port_params_hpp = self.emitNonPortParamsHpp
        c.emit_non_port_params = c.emit_non_port_params_hpp
        c.emit_non_port_params_cpp = self.emitNonPortParamsCpp
        c.param_compName = ("compName", "const char *const", "The component name")
        c.param_instance = ("instance", "const NATIVE_INT_TYPE", "The instance number")
        c.param_instance_default_zero = (
            "instance = 0",
            "const NATIVE_INT_TYPE",
            "The instance number",
        )
        c.param_msgSize = ("msgSize", "const NATIVE_INT_TYPE", "The message size")
        c.param_queueDepth = ("queueDepth", "const NATIVE_INT_TYPE", "The queue depth")

    def initSerialIncludes(self, obj, c):
        """
        Include any headers for channel/parameter serializable includes
        """
        ser_includes = self.__model_parser.uniqueList(obj.get_xml_serializable_files())
        s_includes = [sinc.replace("Ai.xml", "Ac.hpp") for sinc in ser_includes]
        c.ser_includes = s_includes

    def initTelemetry(self, obj, c):
        """
        Init telemetry info
        """
        c.has_telemetry = len(obj.get_channels()) > 0
        c.has_channels = c.has_telemetry
        c.channel_enums = self.__model_parser.getTelemEnumList(obj)
        c.channels = self.__model_parser.getChannelsList(obj)
        self.initTelemetryParams(obj, c)

    def initTelemetryParams(self, obj, c):
        """
        Telemetry function parameters for code generation
        """
        c.param_tlm_id = ("id", "const FwChanIdType", "The channel ID")
        c.param_val = ("val", "Fw::TlmBuffer&", "The channel value")

    def initTypeIncludes(self, obj, c):
        type_includes = [
            e.replace("Ai.xml", "Ac.hpp") for e in obj.get_included_enums()
        ]

        for p in obj.get_ports():
            type_includes = type_includes + p.get_includes()
        c.types_includes = self.__model_parser.uniqueList(type_includes)
        c.c_includes = obj.get_c_header_files()
        if False in [x[-3:] == "hpp" or x[-1:] == "h" for x in c.c_includes]:
            PRINT.info(
                "ERROR: Only .hpp or .h files can be given within <include_header> tag!!!"
            )
            sys.exit(-1)

    def internalInterfaceArgsStr(self):
        """
        Make a list of command args into a string
        """

        def f(lst):
            def g(xxx_todo_changeme1):
                (name, a, b, c) = xxx_todo_changeme1
                return name

            return self.argsString(list(map(g, lst)))

        return f

    def isPrimitiveType(self, type):
        return type in [
            "I8",
            "U8",
            "I16",
            "U16",
            "I32",
            "U32",
            "I64",
            "U64",
            "F32",
            "F64",
            "bool",
        ]

    def mapPartial(self, f, l):
        """
        Map partial function f over list l
        """
        result = []
        for x in l:
            y = f(x)
            if y is not None:
                # check if list
                if isinstance(y, list):
                    result += y
                else:
                    result += [y]
        return result

    def namespaceVisit(self, obj):
        pass

    def openFile(self, filename):
        """
        Open the file for writing
        """
        DEBUG.info("Open file: %s" % filename)
        self.__fp = open(filename, "w")
        DEBUG.info("Completed")

    def initFilesVisit(self, obj):
        filename = self.buildFileName(obj)
        self.openFile(filename)

    def paramStrsCpp(self, param):
        """
        Get the strings for a function parameter in a .cpp file
        """
        name, type = param[:2]
        param_str = "{} {}".format(type, name)
        return param_str, ""

    def paramStrsHpp(self, param):
        """
        Get the strings for a function parameter in a .hpp file
        """
        name, type, comment = param[:3]
        param_str = "{} {}".format(type, name)
        comment_str = " " + self.doxygenPostComment(comment)
        return param_str, comment_str

    def portParamStrsCpp(self, param):
        """
        Get the strings for a port function parameter in a .cpp file
        """
        name, type, comment, modifier = param[:4]
        param_str = "{} {}{}".format(type, modifier, name)
        return param_str, ""

    def portParamStrsHpp(self, param):
        """
        Get the strings for a port function parameter in a .hpp file
        """
        name, type, comment, modifier = param[:4]
        param_str = "{} {}{}".format(type, modifier, name)
        comment_str = " " + self.doxygenPostComment(comment)
        return param_str, comment_str

    def privateVisit(self, obj):
        pass

    def protectedVisit(self, obj):
        pass

    def publicVisit(self, obj):
        pass
