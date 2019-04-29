#!/bin/env python
#===============================================================================
# NAME: ComponentCppVisitor.py
#
# DESCRIPTION: A visitor for generating component implemetation files.
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED  : August 26, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import ComponentVisitorBase
from fprime_ac.generators.templates.component import cpp

class ComponentCppVisitor(ComponentVisitorBase.ComponentVisitorBase):
    """
    A visitor for generating component implementation files.
    """

    def __init__(self):
        self.initBase("ComponentCpp")

    def startSourceFilesVisit(self, obj):
        c = cpp.cpp()
        self.init(obj, c)
        self._writeTmpl(c, "startSourceFilesVisit")
