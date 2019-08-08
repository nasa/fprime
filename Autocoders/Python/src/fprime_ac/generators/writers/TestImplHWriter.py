#!/bin/env python
#===============================================================================
# NAME: TestImplHWriter.py
#
# DESCRIPTION: A writer class for generating test implementation
# header files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.utils import ConfigManager

from fprime_ac.generators.templates.test_impl import hpp
from fprime_ac.generators.writers import TestImplWriterBase

class TestImplHWriter(TestImplWriterBase.TestImplWriterBase):
    """
    A writer class for generating test implementation header files.
    """
    __config   = None
    FILE_NAME = "Tester.hpp"

    def __init__(self):
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.initBase("TestImplH")

    def emitPortParams(self, params):
        return self.emitPortParamsHpp(10, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def _initFilesWrite(self, obj):
        self.openFile(self.FILE_NAME)

    def _startSourceFilesWrite(self, obj):
        c = hpp.hpp()
        self.init(obj, c)
        self.initTestImpl(obj, c)
        c.emit_port_params = self.emitPortParams
        c.emit_non_port_params = self.emitNonPortParams
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
