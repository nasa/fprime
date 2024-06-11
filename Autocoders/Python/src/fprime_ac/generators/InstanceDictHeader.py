# ===============================================================================
# NAME: InstanceDictHeader
#
# DESCRIPTION:  The InstanceDictHeader class is the main entry point
#                 for generation of the command class header.
#
# USAGE: Nominally the InstanceDictHeader.__call__ is called by using the
#          instance name.  The instance name is the function
#        called with a suitable argument object containing
#        all needed model information to generate the code.
#
# AUTHOR: tcanham
# EMAIL:  tcanham@jpl.nasa.gov
# DATE CREATED  : Feb. 25, 2015
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging

# import os
# import sys
# import time
#
# Python extension modules and custom interfaces
#
from fprime_ac.generators.visitors import AbstractVisitor

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
# Module class or classes go here.


class InstanceDictHeader:
    """
    Defines the interface concrete class implementation that drives code generation.
    """

    __visitor_list = []
    __obj = None

    def __init__(self):
        """
        Constructor.
        """
        self.__visitor_list = []

    def __call__(self, args, topology_model):
        """
        Main execution point.
        Calls the accept method on each visitor to generate the code.
        """
        # Note that name handling for params goes
        # here so that the visitor in accept can
        # process all.
        self.__obj = args
        for v in self.__visitor_list:
            self.accept(v, topology_model)

    def accept(self, visitor, topology_model):
        """
        The operation in Visitor design pattern that takes a visitor as an argument
        and calls the visitor's method that corresponds to this element.
        @raise Exception: if the given visitor is not a subclass of AbstractVisitor
        """
        # visitor should be extended from the AbstractVisitor class
        if issubclass(visitor.__class__, AbstractVisitor.AbstractVisitor):
            visitor.DictHeaderVisit(self.__obj, topology_model)
        else:
            DEBUG.error(
                "InstanceDictHeaderVisit.accept() - the given visitor is not a subclass of AbstractVisitor!"
            )
            raise TypeError(
                "InstanceDictHeaderVisit.accept() - the given visitor is not a subclass of AbstractVisitor!"
            )

    def addVisitor(self, visitor):
        """
        Add a visitor to the list of visitors.
        @param visitor: the visitor to add, must be derived from AbstractVisitor.
        """
        if issubclass(visitor.__class__, AbstractVisitor.AbstractVisitor):
            self.__visitor_list.append(visitor)
        else:
            DEBUG.error(
                "InstanceDictHeaderVisit.addVisitor(v) - the given visitor is not a subclass of AbstractVisitor!"
            )
            raise TypeError(
                "InstanceDictHeaderVisit.addVisitor(v) - the given visitor is not a subclass of AbstractVisitor!"
            )

    def getObj(self):
        """
        Return the object to the visitor.
        """
        return self.__obj
