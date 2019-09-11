#!/bin/env python
#===============================================================================
# NAME: GTestCppVisitor.py
#
# DESCRIPTION: A visitor for generating component gtest base implemetation files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 24, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import GTestVisitorBase
from fprime_ac.generators.templates.gtest import cpp

class GTestCppVisitor(GTestVisitorBase.GTestVisitorBase):
    """
    A visitor for generating component gtest base implemetation files.
    """

    def __init__(self):
        self.initBase("GTestCpp")

    def emitCppParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def initFilesVisit(self, obj):
        self.openFile("GTestBase.cpp")

    def startSourceFilesVisit(self, obj):
        c = cpp.cpp()
        self.initGTest(obj, c)
        c.emit_cpp_params = self.emitCppParams
        c.file_message = "      << \"  File:     \" << __callSiteFileName << \"\\n\"\n"
        c.line_message = "      << \"  Line:     \" << __callSiteLineNumber << \"\\n\""
        c.failure_message = "<< \"\\n\"\n" + c.file_message + c.line_message
        c.LTLT = "<<"
        self._writeTmpl(c, "startSourceFilesVisit")

