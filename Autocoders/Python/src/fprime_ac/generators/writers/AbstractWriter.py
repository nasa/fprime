#!/bin/env python
#===============================================================================
# NAME: AbstractWriter.py
#
# DESCRIPTION: Defines the writer interface for all code generation.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
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
class AbstractWriter:

    __generate_empty_file = False

    """
    Defines the writer public methods.
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
    
    def write(self, obj):
        """
        Defined to call all other write methods at once
        @params args: the instance of the concrete element to operate on.
        """
        raise Exception('# AbstractWriter.write() - Implementation Error: you must supply your own concrete implementation.')

    def initFilesWrite(self, obj):
        """
        Defined to generate files for generated code products.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.initFilesWrite() - Implementation Error: you must supply your own concrete implementation.')

    def startSourceFilesWrite(self, obj):
        """
        Defined to generate starting static code within files.
        """
        raise Exception('# AbstractWriter.startSourceFilesWrite() - Implementation Error: you must supply your own concrete implementation.')

    def includes1Write(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Port types
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.includesWrite1() - Implementation Error: you must supply your own concrete implementation.')

    def includes2Write(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.includesWrite2() - Implementation Error: you must supply your own concrete implementation.')

    def namespaceWrite(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.namespaceWrite() - Implementation Error: you must supply your own concrete implementation.')

    def publicWrite(self, obj):
        """
        Defined to generate public stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.publicWrite() - Implementation Error: you must supply your own concrete implementation.')

    def protectedWrite(self, obj):
        """
        Defined to generate protected stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.protectedWrite() - Implementation Error: you must supply your own concrete implementation.')

    def privateWrite(self, obj):
        """
        Defined to generate private stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        raise Exception('# AbstractWriter.privateWrite() - Implementation Error: you must supply your own concrete implementation.')

    def finishSourceFilesWrite(self, obj):
        """
        Defined to generate ending static code within files.
        """
        raise Exception('# AbstractWriter.endSourceFilesWrite() - Implementation Error: you must supply your own concrete implementation.')


    def DictStartWrite(self, obj):
        """
        Defined to generate start of command Python class.
        """
        raise Exception('# DictStartWrite.startCommandWrite() - Implementation Error: you must supply your own concrete implementation.')

    def DictHeaderWrite(self, obj):
        """
        Defined to generate header for Python command class.
        """
        raise Exception('# DictStartWrite.commandHeaderWrite() - Implementation Error: you must supply your own concrete implementation.')

    def DictBodyWrite(self, obj):
        """
        Defined to generate body for Python command class.
        """
        raise Exception('# DictStartWrite.commandBodyWrite() - Implementation Error: you must supply your own concrete implementation.')


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
