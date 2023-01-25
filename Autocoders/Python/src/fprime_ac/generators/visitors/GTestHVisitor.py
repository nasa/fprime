# ===============================================================================
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
# ===============================================================================
import sys

from fprime_ac.generators.visitors import GTestVisitorBase

try:
    from fprime_ac.generators.templates.gtest import hpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class GTestHVisitor(GTestVisitorBase.GTestVisitorBase):
    """
    A visitor for generating gtest header files.
    """

    def __init__(self):
        super().__init__()
        self.initBase("GTestH")

    def emitHppParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def emitMacroParams(self, params):
        str = ""
        for param in params:
            name = param[0]
            str += ", _" + name
        return str

    def initFilesVisit(self, obj):
        self.openFile("GTestBase.hpp")

    def startSourceFilesVisit(self, obj):
        c = hpp.hpp()
        self.initGTest(obj, c)
        c.emit_hpp_params = self.emitHppParams
        c.emit_macro_params = self.emitMacroParams
        c.file_and_line_message = '    << __FILE__ << ":" << __LINE__ << "\\n"'
        c.failure_message = '<< "\\n" \\\n' + c.file_and_line_message
        c.LTLT = "<<"
        self._writeTmpl(c, "startSourceFilesVisit")
