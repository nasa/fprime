# ===============================================================================
# NAME: ComponentCppVisitor.py
#
# DESCRIPTION: A visitor for generating component implementation files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 26, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import sys

from fprime_ac.generators.visitors import ComponentVisitorBase

try:
    from fprime_ac.generators.templates.component import cpp
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)


class ComponentCppVisitor(ComponentVisitorBase.ComponentVisitorBase):
    """
    A visitor for generating component implementation files.
    """

    def __init__(self):
        super().__init__()
        self.initBase("ComponentCpp")

    def startSourceFilesVisit(self, obj):
        c = cpp.cpp()
        self.init(obj, c)
        self._writeTmpl(c, "startSourceFilesVisit")
