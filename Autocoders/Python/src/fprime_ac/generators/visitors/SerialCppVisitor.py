#!/bin/env python
#===============================================================================
# NAME: SerialCppVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of each serializable
#              class source code file.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : June 4, 2013
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
import datetime
from optparse import OptionParser
#
# Python extention modules and custom interfaces
#
#from Cheetah import Template
#from fprime_ac.utils import version
from fprime_ac.utils import ConfigManager
from fprime_ac.models import ModelParser
#from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.visitors import AbstractVisitor
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.serialize import includes1SerialCpp
from fprime_ac.generators.templates.serialize import namespaceSerialCpp
from fprime_ac.generators.templates.serialize import publicSerialCpp
#from fprime_ac.generators.templates import privateSerialCpp
from fprime_ac.generators.templates.serialize import finishSerialCpp
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

typelist = ['U8','I8','U16','I16','U32','I32','U64','I64','F32','F64',"bool"]

#
# Module class or classes go here.
class SerialCppVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of component header
    classes in C++.
    """
    __instance = None
    __config   = None
    __fp       = None
    __form     = None
    __form_comment = None

    def __init__(self):
        """
        Constructor.
        """
        self.__config       = ConfigManager.ConfigManager.getInstance()
        self.__form         = formatters.Formatters()
        self.__form_comment = formatters.CommentFormatters()
        DEBUG.info("SerialCppVisitor: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""

    def _get_args_proto_string(self, obj):
        """
        Return a string of (type, name) args, comma seperated
        for use in templates that generate prototypes.
        """
        arg_str = ""
        for (name,mtype,size,format,comment) in obj.get_members():
            typename = mtype
            if type(mtype) == type(tuple()):
                typename = mtype[0][1]
                arg_str += "%s %s, "%(mtype[0][1],name)
            elif mtype == "string":
                arg_str += "const %s::%sString& %s, " % (obj.get_name(),name, name)
            elif mtype not in typelist:
                arg_str += "const %s& %s, " %(mtype,name)
            elif size != None:
                arg_str += "const %s* %s, " % (mtype, name)
                arg_str += "NATIVE_INT_TYPE %sSize, " % (name)
            else:
                arg_str += "%s %s" % (mtype, name)
                arg_str += ", "

        arg_str = arg_str.strip(', ')
        return arg_str

    def _get_args_string(self, obj, prefix=""):
        """
        Return a string of name args, comma seperated
        for use in templates that generate method or function calls.
        """
        args = obj.get_members()
        arg_str = ""
        for arg in args:
            if arg[2] != None and arg[1] != "string":
                arg_str += prefix + "%s" % arg[0]
                arg_str += ", "
                arg_str += "%s" % arg[2]
                arg_str += ", "
            else:
                arg_str += prefix + "%s" % arg[0]
                arg_str += ", "

        arg_str = arg_str.strip(', ')
        return arg_str

    def _get_conv_mem_list(self, obj):
        """
        Return a list of struct member tuples
        """
        arg_list = list()

        for (name,mtype,size,format,comment) in obj.get_members():
            typeinfo = None
            if type(mtype) == type(tuple()):
                mtype = mtype[0][1]
                typeinfo = "enum"
            elif mtype == "string":
                mtype = "%s::%sString" %(obj.get_name(),name)
                typeinfo = "string"
            elif mtype not in typelist:
                typeinfo = "extern"

            arg_list.append((name,mtype,size,format,comment,typeinfo))

        return arg_list




    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('SerialCppVisitor:%s' % visit_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        self.__fp.writelines(c.__str__())
        DEBUG.debug('===================================')

    def initFilesVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @parms args: the instance of the concrete element to operation on.
        """
        # Build filename here...
        if self.__config.get("serialize","XMLDefaultFileName") == "True":
            namespace = "".join(obj.get_namespace().split('::'))
            filename = namespace + obj.get_name() + self.__config.get("serialize","SerializableCpp")
            PRINT.info("Generating code filename: %s, using XML namespace and name attributes..." % filename)
        else:
            xml_file = obj.get_xml_filename()
            x = xml_file.split(".")
            s = self.__config.get("serialize","SerializableXML").split(".")
            l = len(s[0])
            #
            if (x[0][-l:] == s[0]) & (x[1] == s[1]):
                filename = x[0].split(s[0])[0] + self.__config.get("serialize","SerializableCpp")
                PRINT.info("Generating code filename: %s, using default XML filename prefix..." % filename)
            else:
                msg = "XML file naming format not allowed (must be XXXSerializableAi.xml), Filename: %s" % xml_file
                PRINT.info(msg)
                sys.exit(-1)

        # Open file for writting here...
        DEBUG.info('Open file: %s' % filename)
        self.__fp = open(filename,'w')
        if self.__fp == None:
            raise Exception("Could not open %s file.") % filename
        DEBUG.info('Completed')


    def startSourceFilesVisit(self, obj):
        """
        Defined to generate starting static code within files.
        """
        pass


    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Serial types
        @parms args: the instance of the concrete element to operation on.
        """
        # If BUILD_ROOT is set get me the relative path to current execution location
        relative_path = None
        path = os.getcwd()
        # normalize path to Linux separators - TKC
        path = path.replace("\\","/")
        if ModelParser.BUILD_ROOT != None:
            path = os.path.normpath(os.path.realpath(path))
            build_root = os.path.normpath(os.path.realpath(ModelParser.BUILD_ROOT))
            if path[:len(build_root)].lower() == build_root.lower():
                relative_path = path[len(build_root+'/'):]
            else:
                PRINT.info("ERROR: BUILD_ROOT (%s) and current execution path (%s) not consistent!" % (ModelParser.BUILD_ROOT,path))
                sys.exit(-1)
        #
        DEBUG.debug("Relative path: %s", relative_path)
        #
        c = includes1SerialCpp.includes1SerialCpp()
        c.name = obj.get_name()
        #
        # Hack to fix the include file so it is consistent...
        if self.__config.get("serialize","XMLDefaultFileName") == "False":
            c.name = obj.get_xml_filename().split("SerializableAi.xml")[0]
            c.namespace = ""
        #
        # Added configurable override for includes for testing
        #
        if self.__config.get("includes","serial_include_path") == "None":
            if relative_path != None:
                c.include_path = relative_path
            else:
                c.include_path = obj.get_namespace()
        else:
            c.include_path = self.__config.get("includes","serial_include_path")
        self._writeTmpl(c, "includes1Visit")


    def includes2Visit(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @parms args: the instance of the concrete element to operation on.
        """
        pass


    def namespaceVisit(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @parms args: the instance of the concrete element to operation on.
        """
        c = namespaceSerialCpp.namespaceSerialCpp()
        if obj.get_namespace() == None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split('::')
        c.name = obj.get_name()
        c.mem_list = obj.get_members()

        self._writeTmpl(c, "namespaceVisit")


    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        c = publicSerialCpp.publicSerialCpp()
        c.name = obj.get_name()
        c.args_proto_string = self._get_args_proto_string(obj)
        c.args_string = self._get_args_string(obj)
        c.args_mstring = self._get_args_string(obj, "src.m_")
        c.args_mstring_ptr = self._get_args_string(obj, "src->m_")
        c.members = self._get_conv_mem_list(obj)
        self._writeTmpl(c, "publicVisit")


    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        pass


    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        pass


    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        c = finishSerialCpp.finishSerialCpp()
        if obj.get_namespace() == None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split('::')
        self._writeTmpl(c, "finishSourceFilesVisit")
        self.__fp.close()

if __name__ == '__main__':
    pass
