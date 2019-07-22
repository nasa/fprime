#!/bin/env python
#===============================================================================
# NAME: ComponentTestCppWriter.py
#
# DESCRIPTION: A writer for generating component test implemetation files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.writers import TestWriterBase
from fprime_ac.generators.templates.test import cpp

class ComponentTestCppWriter(TestWriterBase.TestWriterBase):
    """
    A writer for generating component implemetation files.
    """
    
    FILE_NAME = "TesterBase.cpp"

    def __init__(self):
        self.initBase("ComponentTestCpp")

    def emitCppParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def emitCppPortParams(self, params):
        return self.emitPortParamsCpp(8, params)

    def _initFilesWrite(self, obj):
        self.openFile(self.FILE_NAME)

    def _startSourceFilesWrite(self, obj):
        c = cpp.cpp()
        self.initTest(obj, c)
        c.emit_cpp_params = self.emitCppParams
        c.emit_cpp_port_params = self.emitCppPortParams
        c.param_maxHistorySize = ("maxHistorySize", "const U32", "The maximum size of each history")
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
