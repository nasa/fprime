#!/bin/env python
#===============================================================================
# NAME: ComponentTestHVisitor.py
#
# DESCRIPTION: A visitor for generating component test header files
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 26, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import TestVisitorBase
from fprime_ac.generators.templates.test import hpp

class ComponentTestHVisitor(TestVisitorBase.TestVisitorBase):
    """
    A visitor for generating component test header files.
    """

    def __init__(self):
        self.initBase("ComponentTestH")

    def emitHppParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def emitHppPortParams(self, params):
        return self.emitPortParamsHpp(10, params)

    def initFilesVisit(self, obj):
        self.openFile("TesterBase.hpp")

    def startSourceFilesVisit(self, obj):
        c = hpp.hpp()
        self.initTest(obj, c)
        c.emit_hpp_params = self.emitHppParams
        c.emit_hpp_port_params = self.emitHppPortParams
        c.param_maxHistorySize = (
            "maxHistorySize",
            "const U32",
            "The maximum size of each history"
        )
        self._writeTmpl(c, "startSourceFilesVisit")

