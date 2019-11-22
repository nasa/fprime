#!/bin/env python
#===============================================================================
# NAME: ImplHWriter.py
#
# DESCRIPTION: A writer class for generating component implementation
# header files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : August 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.utils import ConfigManager

from fprime_ac.generators.templates.impl import hpp
from fprime_ac.generators.writers import ImplWriterBase

class ImplHWriter(ImplWriterBase.ImplWriterBase):
    """
    A writer class for generating component implementation header files.
    """
    __config   = None

    def __init__(self):
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.initBase("ImplH")

    def emitPortParams(self, params):
        return self.emitPortParamsHpp(10, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def _startSourceFilesWrite(self, obj):
        c = hpp.hpp()
        self.init(obj, c)
        self.initImpl(obj, c)
        c.emit_port_params = self.emitPortParams
        c.emit_non_port_params = self.emitNonPortParams
        self._writeTmpl(c, "startSourceFilesVisit")
    
    def write(self, obj):
        """
        Calls all of the write methods so that full file is made
        """
        self.setFileName(obj)
        self.initFilesWrite(obj)
        self._startSourceFilesWrite(obj)
        self.includes1Write(obj)
        self.includes2Write(obj)
        self.namespaceWrite(obj)
        self.publicWrite(obj)
        self.protectedWrite(obj)
        self.privateWrite(obj)
        self.finishSourceFilesWrite(obj)

    def setFileName(self, obj):
        self.FILE_NAME = obj.get_name() + self.__config.get("component", "ImplH")
    
    def toString(self):
        return self.FILE_NAME
