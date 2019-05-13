#!/bin/env python
#===============================================================================
# NAME: ComponentTestCppVisitor.py
#
# DESCRIPTION: A visitor for generating component test implemetation files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 24, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import TestVisitorBase
from fprime_ac.generators.templates.test import cpp

class ComponentTestCppVisitor(TestVisitorBase.TestVisitorBase):
    """
    A visitor for generating component implemetation files.
    """

    def __init__(self):
        self.initBase("ComponentTestCpp")

    def emitCppParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def emitCppPortParams(self, params):
        return self.emitPortParamsCpp(8, params)

    def initFilesVisit(self, obj):
        self.openFile("TesterBase.cpp")

    def startSourceFilesVisit(self, obj):
        c = cpp.cpp()
        self.initTest(obj, c)
        c.emit_cpp_params = self.emitCppParams
        c.emit_cpp_port_params = self.emitCppPortParams
        c.param_maxHistorySize = ("maxHistorySize", "const U32", "The maximum size of each history")
        self._writeTmpl(c, "startSourceFilesVisit")

