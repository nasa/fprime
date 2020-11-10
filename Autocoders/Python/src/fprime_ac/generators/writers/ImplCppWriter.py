# ===============================================================================
# NAME: ImplCppWriter.py
#
# DESCRIPTION: A writer class for generating component implementation cpp files.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : August 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.writers import ImplWriterBase
from fprime_ac.utils import ConfigManager

try:
    from fprime_ac.generators.templates.impl import cpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class ImplCppWriter(ImplWriterBase.ImplWriterBase):
    """
    A writer class for generating component implementation cpp files.
    """

    __config = None

    def __init__(self):
        super().__init__()
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.initBase("ImplCpp")

    def emitPortParams(self, params):
        return self.emitPortParamsCpp(8, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsCpp(8, params)

    def _startSourceFilesWrite(self, obj):
        c = cpp.cpp()
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
        self.FILE_NAME = obj.get_name() + self.__config.get("component", "ImplCpp")

    def toString(self):
        return self.FILE_NAME
