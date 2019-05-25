#!/bin/env python
#===============================================================================
# NAME: TestVisitorBase.py
#
# DESCRIPTION: A base class for component test visitors
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : September 26, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import ComponentVisitorBase

class TestVisitorBase(ComponentVisitorBase.ComponentVisitorBase):
    """
    A base class for component test visitors
    """

    def transformEnumType(self, c, type, typeinfo):
        return c.component_base + "::" + type \
        if typeinfo == "enum" \
        else type

    def getTlmType(self, c):
        def f(type, typeinfo):
            if type == "string":
                type = "Fw::TlmString"
            return self.transformEnumType(c, type, typeinfo)
        return f

    def getParamType(self, c):
        def f(type, typeinfo):
            if type == "string":
                type = "Fw::ParamString"
            return self.transformEnumType(c, type, typeinfo)
        return f

    def getParamValTlm(self, c):
        def f(type, typeinfo):
            type = self.getTlmType(c)(type, typeinfo)
            type = "const " + type + "&"
            return ("val", type, "The channel value")
        return f

    def getParamValParam(self, c):
        def f(type, typeinfo):
            type = self.getParamType(c)(type, typeinfo)
            type = "const " + type + "&"
            return ("val", type, "The parameter value")
        return f

    def transformEventParams(self, c, params):
        def transformEventParam(param):
            name, type, comment, size, typeinfo = param
            type = self.transformEnumType(c, type, typeinfo)
            return (name, type, comment, typeinfo)
        return list(map(transformEventParam, params))

    def getEventParams(self, c):
        def f(eventName):
            params = c.event_params[eventName]
            return self.transformEventParams(c, params)
        return f

    def transformCommandParams(self, c, params):
        def transformCommandParam(param):
            name, type, comment, typeinfo = param
            type = self.transformEnumType(c, type, typeinfo)
            return (name, type, comment, typeinfo)
        return list(map(transformCommandParam, params))

    def getCommandParams(self, c):
        def f(mnemonic):
            params = c.command_params[mnemonic]
            return self.transformCommandParams(c, params)
        return f

    def getParamPort(self, c):
        def f(instance, type):
            namespace = c.port_namespaces[type]
            if namespace == None:
                namespace = ""
            if type == "Serial":
                type = namespace + "::InputSerializePort"
            else:
                type = namespace + "::Input" + type + "Port"
            return ("*const " + instance, type, "The port")
        return f

    def initTest(self, obj, c):
        self.init(obj, c)
        c.tester_base = c.name() + "TesterBase"
        c.get_command_params = self.getCommandParams(c)
        c.get_event_params = self.getEventParams(c)
        c.get_param_port = self.getParamPort(c)
        c.get_param_val_Param = self.getParamValParam(c)
        c.get_param_val_Tlm = self.getParamValTlm(c)
        c.get_tlm_type = self.getTlmType(c)
        c.get_param_type = self.getParamType(c)
        c.has_from_ports = len(c.typed_user_output_ports) > 0 \
          or c.has_commands or c.has_parameters \
          or c.has_telemetry or c.has_events

