#!/bin/env python
#===============================================================================
# NAME: TestImplWriterBase.py
#
# DESCRIPTION: A base class for TestImpl writers
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.writers import ComponentWriterBase

class TestImplWriterBase(ComponentWriterBase.ComponentWriterBase):
    """
    A base class for TestImpl writers
    """

    def initTestImpl(self, obj, c):
        self.init(obj, c)
        c.component_base = c.name() + "ComponentBase"
        c.gtest_base = c.name() + "GTestBase"

