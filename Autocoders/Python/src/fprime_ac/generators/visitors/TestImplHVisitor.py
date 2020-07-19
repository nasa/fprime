# ===============================================================================
# NAME: TestImplHVisitor.py
#
# DESCRIPTION: A visitor class for generating test implementation
# header files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED: October 12, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.visitors import TestImplVisitorBase
from fprime_ac.utils import ConfigManager

try:
    from fprime_ac.generators.templates.test_impl import hpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class TestImplHVisitor(TestImplVisitorBase.TestImplVisitorBase):
    """
    A visitor class for generating test implementation header files.
    """

    __config = None

    def __init__(self):
        super().__init__()
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.initBase("TestImplH")

    def emitPortParams(self, params):
        return self.emitPortParamsHpp(10, params)

    def emitNonPortParams(self, params):
        return self.emitNonPortParamsHpp(10, params)

    def initFilesVisit(self, obj):
        self.openFile("Tester.hpp")

    def startSourceFilesVisit(self, obj):
        c = hpp.hpp()
        self.init(obj, c)
        self.initTestImpl(obj, c)
        c.emit_port_params = self.emitPortParams
        c.emit_non_port_params = self.emitNonPortParams
        self._writeTmpl(c, "startSourceFilesVisit")
