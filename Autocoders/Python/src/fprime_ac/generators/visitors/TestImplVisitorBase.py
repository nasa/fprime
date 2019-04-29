#!/bin/env python
#===============================================================================
# NAME: TestImplVisitorBase.py
#
# DESCRIPTION: A base class for TestImpl visitors
#
# AUTHOR: bocchino
# EMAIL:  bocchino@jpl.nasa.gov
# DATE CREATED: November 14, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.visitors import ComponentVisitorBase

class TestImplVisitorBase(ComponentVisitorBase.ComponentVisitorBase):
    """
    A base class for TestImpl visitors
    """

    def initTestImpl(self, obj, c):
        self.init(obj, c)
        c.component_base = c.name() + "ComponentBase"
        c.gtest_base = c.name() + "GTestBase"

