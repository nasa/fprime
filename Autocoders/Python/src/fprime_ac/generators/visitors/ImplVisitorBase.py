#!/bin/env python
#===============================================================================
# NAME: ImplVisitorBase.py
#
# DESCRIPTION: A base class for Impl visitors
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED: October 12, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import ComponentVisitorBase

class ImplVisitorBase(ComponentVisitorBase.ComponentVisitorBase):
    """
    A base class for Impl visitors
    """

    def initImpl(self, obj, c):
        self.init(obj, c)
        c.component_base = c.name() + "ComponentBase"
        c.impl = c.name() + "Impl"

