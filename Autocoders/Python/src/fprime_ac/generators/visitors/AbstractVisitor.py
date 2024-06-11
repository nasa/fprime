# ===============================================================================
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
# ===============================================================================
#
# Python standard modules
#
import logging
import os
import sys

from fprime_ac.utils.buildroot import (
    BuildRootMissingException,
    build_root_relative_path,
)

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
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.initFilesVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def startSourceFilesVisit(self, obj):
        """
        Defined to generate starting static code within files.
        """
        raise NotImplementedError(
            "# AbstractVisitor.startSourceFilesVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Port types
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.includesVisit1() - Implementation Error: you must supply your own concrete implementation."
        )

    def includes2Visit(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.includesVisit2() - Implementation Error: you must supply your own concrete implementation."
        )

    def namespaceVisit(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.namespaceVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.publicVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.protectedVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        raise NotImplementedError(
            "# AbstractVisitor.privateVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        raise NotImplementedError(
            "# AbstractVisitor.endSourceFilesVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def DictStartVisit(self, obj):
        """
        Defined to generate start of command Python class.
        """
        raise NotImplementedError(
            "# DictStartVisit.startCommandVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def DictHeaderVisit(self, obj):
        """
        Defined to generate header for Python command class.
        """
        raise NotImplementedError(
            "# DictStartVisit.commandHeaderVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def DictBodyVisit(self, obj):
        """
        Defined to generate body for Python command class.
        """
        raise NotImplementedError(
            "# DictStartVisit.commandBodyVisit() - Implementation Error: you must supply your own concrete implementation."
        )

    def isSync(self, str):
        return str is not None and str.lower() == "sync"

    def isAsync(self, str):
        return str is not None and str.lower() == "async"

    def isSerial(self, str):
        return str is not None and str.lower() == "serial"

    def isInput(self, str):
        return str is not None and str.lower() == "input"

    def relativePath(self):
        """
        If BUILD_ROOT is set, get the relative path to current execution location
        """
        path = os.getcwd()
        try:
            relative_path = build_root_relative_path(path)
        except BuildRootMissingException as bre:
            PRINT.info(
                f"ERROR: BUILD_ROOT and current execution path ({path}) not consistent! {str(bre)}"
            )
            sys.exit(-1)
        DEBUG.debug("Relative path: %s", relative_path)
        return relative_path
