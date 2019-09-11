#!/bin/env python
#===============================================================================
# NAME: GTestWriter.py
#
# DESCRIPTION: A writer for generating gtest base header files
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.writers import GTestWriterBase
from fprime_ac.generators.templates.gtest import hpp

class GTestHWriter(GTestWriterBase.GTestWriterBase):
    """
    A writer for generating gtest header files.
    """
    
    FILE_NAME = "GTestBase.hpp"

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

    def _initFilesWrite(self, obj):
        self.openFile(self.FILE_NAME)

    def _startSourceFilesWrite(self, obj):
        c = hpp.hpp()
        self.initGTest(obj, c)
        c.emit_hpp_params = self.emitHppParams
        c.emit_macro_params = self.emitMacroParams
        c.file_message = "    << \"  File:     \" << __FILE__ << \"\\n\" \\\n"
        c.line_message = "    << \"  Line:     \" << __LINE__ << \"\\n\""
        c.failure_message = "<< \"\\n\" \\\n" + c.file_message + c.line_message
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
