#!/bin/env python
#===============================================================================
# NAME: ModuleLoader.py
#
# DESCRIPTION: This module is a decorator for loading modules from a target
#              directory.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb. 9, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
from __future__ import print_function
import glob
import os
import sys
import exceptions
#
# adding the loadModules call.
def ModuleLoader(Class):
    setattr(Class, "module_path", None)
    def loadModules(self, module_path, attr1, attr2):
        """
        Decorator method to load all modules found
        within a specified path.
        @param module_path: directory path to modules
        @param attr1: module attribute used as key to list
        @param attr2: module attribute used as value to list
        @return: a list of (key, value) tuples from the modules   
        """
        #
        # First add modules target directory to python path
        if not os.path.isdir(module_path):
            raise exceptions.GseControllerUndefinedDirectoryException(module_path)
        sys.path.append(module_path)
        
        #
        # Get package name from path here
        pkg = os.path.split(module_path)[-1]

        #
        # Get the modules
        #
        modules = glob.glob(module_path + os.sep + '*.py')
        #
        # Iterate over them and import them
        #
        file_list = []
        for imp in modules:
            fi = os.path.split(imp)[-1]
            if fi != "__init__.py":
                file_list.append(fi)
        #
        # Import the modules here
        #
        loaded_modules = sys.modules.keys()
        module_list = []
        for m in file_list:
            m = m[:-3]
            # Use package name qualifier so that modules of same name get reloaded by unique path
            exec "from %s import %s" % (pkg, m) in globals()
            module_list.append(m)

        attr_list = []
        id_list = [] # to look for duplicates
        for m in module_list:
            id = eval("%s.%s" % (m,attr1))
            if id in id_list:
                print("[WARNING] duplicate id %d found" % id, file=sys.stderr)
            else:
                id_list.append(id)
            attr_list.append((id,eval("%s.%s" % (m,attr2))))


        return attr_list

    setattr(Class, "loadModules", loadModules)
    return Class
    
