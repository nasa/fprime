#!/bin/env python
#===============================================================================
# NAME: ImplWriterBase.py
#
# DESCRIPTION: A base class for Impl writers
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : August 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.generators.writers import ComponentWriterBase

class ImplWriterBase(ComponentWriterBase.ComponentWriterBase):
    """
    A base class for Impl writers
    """

    def initImpl(self, obj, c):
        self.init(obj, c)
        c.component_base = c.name() + "ComponentBase"
        c.impl = c.name() + "Impl"

