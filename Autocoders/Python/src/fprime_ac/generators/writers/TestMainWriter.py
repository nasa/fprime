#!/bin/env python
#===============================================================================
# NAME: TestMainWriter.py
#
# DESCRIPTION: A writer for generating main.cpp files for test component toDo() test case.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : July 8, 2019
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
import sys

from fprime_ac.generators.writers import TestWriterBase
from fprime_ac.generators.templates.test import test_main

class TestMainWriter(TestWriterBase.TestWriterBase):
    """
    A writer for generating component implemetation files.
    """
    
    FILE_NAME = "TestMain.cpp"
    
    def __init__(self):
        self.initBase("TestMainCpp")
    
    def emitCppParams(self, params):
        return self.emitNonPortParamsCpp(8, params)
    
    def emitCppPortParams(self, params):
        return self.emitPortParamsCpp(8, params)
    
    def _initFilesWrite(self, obj):
        self.openFile(self.FILE_NAME)
    
    def _startSourceFilesWrite(self, obj):
        c = test_main.test_main()
        self.initTest(obj, c)
        
        if not hasattr(self, "test_cases"):
            self.test_cases = []
            self.test_cases.append("toDo")
        
        tclist = []
        for case in self.test_cases:
            if not type(case) is tuple:
                tclist.append( (case[0].capitalize() + case[1:], case) )
            else:
                tclist.append( (case[0][0].capitalize() + case[0][1:], case[1]) )
        
        c.test_cases = tclist
        
        self._writeTmpl(c, "startSourceFilesWrite")
    
    def add_test_cases(self, test_cases):
        self.test_cases = test_cases
    
    def override_names(self, override_dict):
        for key in override_dict.keys():
            if len(self.test_cases) > override_dict[key]:
                self.test_cases[override_dict[key]] = (key, self.test_cases[override_dict[key]])
            else:
                print("ERROR: // @Testcase: decorator in Tester.hpp incorrectly formatted")
                sys.exit(-1)
    
    def write(self, obj):
        """
        Calls all of the write methods so that full file is made
        """
        self._initFilesWrite(obj)
        self._startSourceFilesWrite(obj)
        self.includes1Write(obj)
        self.includes2Write(obj)
        self.namespaceWrite(obj)
        self.publicWrite(obj)
        self.protectedWrite(obj)
        self.privateWrite(obj)
        self.finishSourceFilesWrite(obj)
    
    def toString(self):
        return self.FILE_NAME
