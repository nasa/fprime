# ===============================================================================
# NAME: ComponentHVisitor.py
#
# DESCRIPTION: A visitor for generating component header files
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 24, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.visitors import ComponentVisitorBase

try:
    from fprime_ac.generators.templates.component import hpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class ComponentHVisitor(ComponentVisitorBase.ComponentVisitorBase):
    """
    A visitor for generating component header files.
    """

    def __init__(self):
        super().__init__()
        self.initBase("ComponentH")

    def startSourceFilesVisit(self, obj):
        c = hpp.hpp()
        self.init(obj, c)
        self._writeTmpl(c, "startSourceFilesVisit")
