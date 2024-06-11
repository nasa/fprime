# ===============================================================================
# NAME: TestImplCppHelpersVisitor.py
#
# DESCRIPTION: A visitor class for generating test implementation cpp files.
#
# AUTHOR: lestarch
# DATE CREATED: November 28, 2022
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.visitors import TestImplVisitorBase

try:
    from fprime_ac.generators.templates.test import test_helpers
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class TestImplCppHelpersVisitor(TestImplVisitorBase.TestImplVisitorBase):
    """
    A visitor class for generating test implementation cpp files.
    """

    def __init__(self):
        super().__init__()
        self.initBase("TestImplCpp")

    def emitPortParams(self, params):
        return self.emitPortParamsCpp(8, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def initFilesVisit(self, obj):
        self.openFile("TesterHelpers.cpp")

    def startSourceFilesVisit(self, obj):
        c = test_helpers.test_helpers()
        self.init(obj, c)
        self.initTestImpl(obj, c)
        c.emit_port_params = self.emitPortParams
        c.emit_non_port_params = self.emitNonPortParams
        self._writeTmpl(c, "startSourceFilesVisit")
