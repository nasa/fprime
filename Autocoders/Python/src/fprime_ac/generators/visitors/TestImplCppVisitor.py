#!/bin/env python
#===============================================================================
# NAME: TestImplCppVisitor.py
#
# DESCRIPTION: A visitor class for generating test implementation cpp files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED: October 12, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.utils import ConfigManager

from fprime_ac.generators.templates.test_impl import cpp
from fprime_ac.generators.visitors import TestImplVisitorBase

class TestImplCppVisitor(TestImplVisitorBase.TestImplVisitorBase):
    """
    A visitor class for generating test implementation cpp files.
    """

    def __init__(self):
        self.initBase("TestImplCpp")

    def emitPortParams(self, params):
        return self.emitPortParamsCpp(8, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def initFilesVisit(self, obj):
        self.openFile("Tester.cpp")

    def startSourceFilesVisit(self, obj):
        c = cpp.cpp()
        self.init(obj, c)
        self.initTestImpl(obj, c)
        c.emit_port_params = self.emitPortParams
        c.emit_non_port_params = self.emitNonPortParams
        self._writeTmpl(c, "startSourceFilesVisit")
