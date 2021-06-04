# ===============================================================================
# NAME: ComponentTestCppVisitor.py
#
# DESCRIPTION: A visitor for generating component test implementation files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 24, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.visitors import TestVisitorBase

try:
    from fprime_ac.generators.templates.test import cpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class ComponentTestCppVisitor(TestVisitorBase.TestVisitorBase):
    """
    A visitor for generating component implementation files.
    """

    def __init__(self):
        super().__init__()
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
        c.param_maxHistorySize = (
            "maxHistorySize",
            "const U32",
            "The maximum size of each history",
        )
        self._writeTmpl(c, "startSourceFilesVisit")
