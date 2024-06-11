# ===============================================================================
# NAME:  AbstractGenerator.py
#
# DESCRIPTION: The abstract generator defines the interface
#                for each element or in this case
#              portion of code to be generated.  We use
#              the python __call__ method to make
#              direct calls to generators.
#
# USAGE: Normally these will be instanced by a Factory
#        then each one will have the needed visitors
#          added.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb. 5, 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging

#
# Python extension modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")

#
# Module class or classes go here.


class AbstractGenerator:
    """
    Defines the interface (element) classes
    used for code generation.  All these
    use the python __call__ method as the
    main call to produce code generation by visitors.
    """

    def __init__(self):
        """
        Constructor.
        """

    def __call__(self, args):
        """
        Main execution point.
        Calls the accept method on each visitor to generate the code.
        """
        raise NotImplementedError(
            "AbstractGenerator.__call__() - Implementation Error: you must supply your own concrete implementation."
        )

    def accept(self, visitor):
        """
        Execute the visit call on this object.
        """
        raise NotImplementedError(
            "AbstractFace.accept.accept(v) - Implementation Error: you must supply your own concrete implementation."
        )

    def addVisitor(self, visitor):
        """
        Method to add the visitor to a list of visitors.
        """
        raise NotImplementedError(
            "AbstractFace.accept.addVisitor(v) - Implementation Error: you must supply your own concrete implementation."
        )
