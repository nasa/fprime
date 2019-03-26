#!/bin/env python
#===============================================================================
# NAME: SerialHVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of header file
#              for each serializable class.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : June 4, 2007
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
#from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.visitors import AbstractVisitor
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.serialize import startSerialH
from fprime_ac.generators.templates.serialize import includes1SerialH
from fprime_ac.generators.templates.serialize import includes2SerialH
from fprime_ac.generators.templates.serialize import namespaceSerialH
from fprime_ac.generators.templates.serialize import publicSerialH
from fprime_ac.generators.templates.serialize import protectedSerialH
from fprime_ac.generators.templates.serialize import privateSerialH
from fprime_ac.generators.templates.serialize import finishSerialH
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
class SerialHVisitor(AbstractVisitor.AbstractVisitor):
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
        DEBUG.info("SerialHVisitor: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""


    def _get_args_string(self, obj):
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

    def _get_conv_mem_list(self, obj):
        """
        Return a list of port argument tuples
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


    def _get_enum_string_list(self, enum_list):
        """
        """
        enum_tuple = enum_list[0]
        enum_list = enum_list[1]
        enum_str_list = []
        for e in enum_list:
            # No value, No comment
            if (e[1] == None) and (e[2] == None):
                s = "%s," % (e[0])
            # No value, With comment
            elif (e[1] == None) and (e[2] != None):
                s = "%s,  // %s" % (e[0],e[2])
            # With value, No comment
            elif (e[1] != None) and (e[2] == None):
                s = "%s = %s," % (e[0],e[1])
            # With value and comment
            elif (e[1] != None) and (e[2] != None):
                s = "%s = %s,  // %s" % (e)
            else:
                pass
            enum_str_list.append(s)

        return (enum_tuple, enum_str_list)



    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('SerialHVisitor:%s' % visit_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        self.__fp.writelines(c.__str__())
        DEBUG.debug('===================================')


    def initFilesVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the concrete element to operation on.
        """
        # Build filename here...
        if self.__config.get("serialize","XMLDefaultFileName") == "True":
            namespace = "".join(obj.get_namespace().split('::'))
            filename = namespace + obj.get_name() + self.__config.get("serialize","SerializableH")
            PRINT.info("Generating code filename: %s, using XML namespace and name attributes..." % filename)
        else:
            xml_file = obj.get_xml_filename()
            x = xml_file.split(".")
            s = self.__config.get("serialize","SerializableXML").split(".")
            l = len(s[0])
            #
            if (x[0][-l:] == s[0]) & (x[1] == s[1]):
                filename = x[0].split(s[0])[0] + self.__config.get("serialize","SerializableH")
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
        c = startSerialH.startSerialH()
        c.name = obj.get_name()
        d = datetime.datetime.now()
        c.date = d.strftime("%A, %d %B %Y")
        c.user = os.environ['USER']
        self._writeTmpl(c, "startSourceFilesVisit")


    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Serial types
        @parms args: the instance of the concrete element to operation on.
        """
        c = includes1SerialH.includes1SerialH()
        self._writeTmpl(c, "includes1Visit")


    def includes2Visit(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @parms args: the instance of the concrete element to operation on.
        """
        c = includes2SerialH.includes2SerialH()
        c.xml_includes_list = obj.get_xml_includes()
        if False in [x[-6:] == 'Ai.xml' for x in c.xml_includes_list]:
            PRINT.info("ERROR: Only Ai.xml files can be given within <import_serializable_type> tag!!!")
            sys.exit(-1)
        c.xml_includes_list = [x.replace('Ai.xml','Ac.hpp') for x in  c.xml_includes_list]
        c.c_includes_list = obj.get_c_includes()
        if False in [x[-3:] == 'hpp' or x[-1:] == 'h' for x in c.c_includes_list]:
            PRINT.info("ERROR: Only .hpp or .h files can be given within <include_header> tag!!!")
            sys.exit(-1)
        #
        self._writeTmpl(c, "includes2Visit")


    def namespaceVisit(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @parms args: the instance of the concrete element to operation on.
        """
        c = namespaceSerialH.namespaceSerialH()
        if obj.get_namespace() == None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split('::')

        c.enum_type_list = []
        t = [x[1] for x in obj.get_members()]
        enum_list = [x for x in t if type(x) == type(tuple())]
        for e in enum_list:
            c.enum_type_list.append(self._get_enum_string_list(e))

        c.mem_list = obj.get_members()

        c.name = obj.get_name()
        self._writeTmpl(c, "namespaceVisit")


    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        c = publicSerialH.publicSerialH()
        c.name = obj.get_name()
        c.args_proto = self._get_args_string(obj)
        c.members = self._get_conv_mem_list(obj)
        self._writeTmpl(c, "publicVisit")


    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        c = protectedSerialH.protectedSerialH()
        c.uuid = obj.get_typeid()
        c.name = obj.get_name()
        c.members = self._get_conv_mem_list(obj)
        self._writeTmpl(c, "protectedVisit")


    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        c = privateSerialH.privateSerialH()
        self._writeTmpl(c, "privateVisit")


    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        c = finishSerialH.finishSerialH()
        c.name = obj.get_name()
        if obj.get_namespace() == None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split('::')
        self._writeTmpl(c, "finishSourceFilesVisit")
        self.__fp.close()


if __name__ == '__main__':
    pass
