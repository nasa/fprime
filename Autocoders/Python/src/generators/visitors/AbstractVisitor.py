#!/bin/env python
#===============================================================================
# NAME: AbstractVisitor.py
#
# DESCRIPTION: Defines the visitor interface for all code generation.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb. 5 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import logging
import os
import sys
import time
#
# Python extention modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

# Abstract base class definition
class AbstractVisitor:

    __generate_empty_file = False

    """
    Defines the visitor public methods.
    """
    def __init__(self):

        self.__generate_empty_file = False


    def setGenerateEmptyFile(self):
        """
        This method is used to set a flag indicating that an empty file
        should be generated. This is just a flag. It is up to the derived
        class to ensure an empty file is generated for the code snippet
        producers.
        """
        self.__generate_empty_file = True

    def generateEmptyFile(self):
        """
        Return flag indicating if a file should be created.
        """
        return self.__generate_empty_file

    def initFilesVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.initFilesVisit() - Implementation Error: you must supply your own concrete implementation.')

    def startSourceFilesVisit(self, obj):
        """
        Defined to generate starting static code within files.
        """
        raise Exception('# AbstractVisitor.startSourceFilesVisit() - Implementation Error: you must supply your own concrete implementation.')

    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Port types
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.includesVisit1() - Implementation Error: you must supply your own concrete implementation.')

    def includes2Visit(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.includesVisit2() - Implementation Error: you must supply your own concrete implementation.')

    def namespaceVisit(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.namespaceVisit() - Implementation Error: you must supply your own concrete implementation.')

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.publicVisit() - Implementation Error: you must supply your own concrete implementation.')

    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.protectedVisit() - Implementation Error: you must supply your own concrete implementation.')

    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractVisitor.privateVisit() - Implementation Error: you must supply your own concrete implementation.')

    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        raise Exception('# AbstractVisitor.endSourceFilesVisit() - Implementation Error: you must supply your own concrete implementation.')


    def DictStartVisit(self, obj):
        """
        Defined to generate start of command Python class.
        """
        raise Exception('# DictStartVisit.startCommandVisit() - Implementation Error: you must supply your own concrete implementation.')

    def DictHeaderVisit(self, obj):
        """
        Defined to generate header for Python command class.
        """
        raise Exception('# DictStartVisit.commandHeaderVisit() - Implementation Error: you must supply your own concrete implementation.')

    def DictBodyVisit(self, obj):
        """
        Defined to generate body for Python command class.
        """
        raise Exception('# DictStartVisit.commandBodyVisit() - Implementation Error: you must supply your own concrete implementation.')


    def isSync(self, str):
        return str != None and str.lower() == "sync"


    def isAsync(self, str):
        return str != None and str.lower() == "async"


    def isSerial(self, str):
        return str != None and str.lower() == "serial"


    def isInput(self, str):
        return str != None and str.lower() == "input"


if __name__ == '__main__':
    pass
