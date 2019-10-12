#!/bin/env python
#===============================================================================
# NAME: GTestWriterBase.py
#
# DESCRIPTION: A base class for GTest writers
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.writers import ComponentWriterBase

class GTestWriterBase(ComponentWriterBase.ComponentWriterBase):
    """
    A base class for GTest writers
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

    def transformEventParams(self, c, params):
        def transformEventParam(param):
            name, type, comment, size, typeinfo = param
            if typeinfo == "string":
                return (name, "const char *const", comment, typeinfo)
            elif typeinfo == "enum":
                return (name, c.component_base + "::" + type, comment, typeinfo)
            else:
                return (name, "const " + type, comment, typeinfo)
        return list(map(transformEventParam, params))

    def getEventParams(self, c):
        def f(eventName):
            params = c.event_params[eventName]
            return self.transformEventParams(c, params)
        return f

    def getParamValTlm(self, c):
        def f(type, typeinfo):
            if type == "string":
                type = "const char *const"
            else:
                type = self.transformEnumType(c, type, typeinfo)
                type = "const " + type + "&"
            return ("val", type, "The channel value")
        return f

    def initGTest(self, obj, c):
        self.init(obj, c)
        c.gtest_base = c.name() + "GTestBase"
        c.tester_base = c.name() + "TesterBase"
        c.get_event_params = self.getEventParams(c)
        c.get_param_val_Tlm = self.getParamValTlm(c)
        c.param_maxHistorySize = (
            "maxHistorySize",
            "const U32",
            "The maximum size of each history"
        )
        c.param_fileName = (
             "__callSiteFileName",
             "const char *const",
             "The name of the file containing the call site",
             ""
        )
        c.param_lineNumber = (
            "__callSiteLineNumber",
            "const U32",
            "The line number of the call site", ""
        )
        c.param_size = ("size", "const U32", "The asserted size", "")
        c.param_index = ("__index", "const U32", "The index", "")
        c.params_assert_size = [ c.param_fileName, c.param_lineNumber, c.param_size ]
        c.params_assert_cmd_response = [
            c.param_fileName,
            c.param_lineNumber,
            c.param_index,
            c.param_opCode,
            c.param_cmdSeq,
            c.param_response
        ]
        c.params_assert_event = [ c.param_fileName, c.param_lineNumber, c.param_index ]
        c.params_assert_from_port = c.params_assert_event

