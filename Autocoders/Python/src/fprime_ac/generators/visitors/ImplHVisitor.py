#!/bin/env python
#===============================================================================
# NAME: ImplHVisitor.py
#
# DESCRIPTION: A visitor class for generating component implementation
# header files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED: October 12, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.utils import ConfigManager

from fprime_ac.generators.templates.impl import hpp
from fprime_ac.generators.visitors import ImplVisitorBase

class ImplHVisitor(ImplVisitorBase.ImplVisitorBase):
    """
    A visitor class for generating component implementation header files.
    """
    __config   = None

    def __init__(self):
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.initBase("ImplH")

    def buildFileName(self, obj):
        return obj.get_name() + self.__config.get("component", "ImplH")

    def emitPortParams(self, params):
        return self.emitPortParamsHpp(10, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def startSourceFilesVisit(self, obj):
        c = hpp.hpp()
        self.init(obj, c)
        self.initImpl(obj, c)
        c.emit_port_params = self.emitPortParams
        c.emit_non_port_params = self.emitNonPortParams
        self._writeTmpl(c, "startSourceFilesVisit")
