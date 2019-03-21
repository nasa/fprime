#!/bin/env python
#===============================================================================
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
#===============================================================================

from fprime_ac.generators.visitors import ComponentVisitorBase
from fprime_ac.generators.templates.component import hpp

class ComponentHVisitor(ComponentVisitorBase.ComponentVisitorBase):
    """
    A visitor for generating component header files.
    """

    def __init__(self):
        self.initBase("ComponentH")

    def startSourceFilesVisit(self, obj):
      c = hpp.hpp()
      self.init(obj, c)
      self._writeTmpl(c, "startSourceFilesVisit")
