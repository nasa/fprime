#!/bin/env python
#===============================================================================
# NAME: GTestVisitor.py
#
# DESCRIPTION: A visitor for generating gtest base header files
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 26, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import GTestVisitorBase
from fprime_ac.generators.templates.gtest import hpp

class GTestHVisitor(GTestVisitorBase.GTestVisitorBase):
    """
    A visitor for generating gtest header files.
    """

    def __init__(self):
        self.initBase("GTestH")

    def emitHppParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def emitMacroParams(self, params):
        length = len(params)
        str = ""
        for param in params:
            name = param[0]
            str += (", _" + name)
        return str

    def initFilesVisit(self, obj):
        self.openFile("GTestBase.hpp")

    def startSourceFilesVisit(self, obj):
        c = hpp.hpp()
        self.initGTest(obj, c)
        c.emit_hpp_params = self.emitHppParams
        c.emit_macro_params = self.emitMacroParams
        c.file_message = "    << \"  File:     \" << __FILE__ << \"\\n\" \\\n"
        c.line_message = "    << \"  Line:     \" << __LINE__ << \"\\n\""
        c.failure_message = "<< \"\\n\" \\\n" + c.file_message + c.line_message
        c.LTLT = "<<"
        self._writeTmpl(c, "startSourceFilesVisit")

