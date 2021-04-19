# ===============================================================================
# NAME: GTestCppWriter.py
#
# DESCRIPTION: A writer for generating component gtest base implementation files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.writers import GTestWriterBase

try:
    from fprime_ac.generators.templates.gtest import cpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class GTestCppWriter(GTestWriterBase.GTestWriterBase):
    """
    A writer for generating component gtest base implementation files.
    """

    FILE_NAME = "GTestBase.cpp"

    def __init__(self):
        super().__init__()
        self.initBase("GTestCpp")

    def emitCppParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def _initFilesWrite(self, obj):
        self.openFile(self.FILE_NAME)

    def _startSourceFilesWrite(self, obj):
        c = cpp.cpp()
        self.initGTest(obj, c)
        c.emit_cpp_params = self.emitCppParams
        c.file_message = '      << "  File:     " << __callSiteFileName << "\\n"\n'
        c.line_message = '      << "  Line:     " << __callSiteLineNumber << "\\n"'
        c.failure_message = '<< "\\n"\n' + c.file_message + c.line_message
        c.LTLT = "<<"
        self._writeTmpl(c, "startSourceFilesWrite")

    def write(self, obj):
        """
        Calls all of the write methods so that full file is made
        """
        self._initFilesWrite(obj)
        self._startSourceFilesWrite(obj)
        self.includes1Write(obj)
        self.includes2Write(obj)
        self.namespaceWrite(obj)
        self.publicWrite(obj)
        self.protectedWrite(obj)
        self.privateWrite(obj)
        self.finishSourceFilesWrite(obj)

    def toString(self):
        return self.FILE_NAME
