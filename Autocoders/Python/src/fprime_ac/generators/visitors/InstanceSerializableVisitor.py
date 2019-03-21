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
from fprime_ac.utils import DictTypeConverter
#from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.visitors import AbstractVisitor
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.serialize import SerialHeader
from fprime_ac.generators.templates.serialize import SerialImport
from fprime_ac.generators.templates.serialize import SerialBody
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
class InstanceSerializableVisitor(AbstractVisitor.AbstractVisitor):
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
        DEBUG.info("InstanceSerializableVisitor: Instanced.")
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
        enum_str_list[-1] = enum_str_list[-1].replace(',','')

        return (enum_tuple, enum_str_list)
    


    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('InstanceSerializableVisitor:%s' % visit_str)
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
        
        # file location will be based on namespace
        
        namespace = obj.get_namespace()
        typename = obj.get_name()
        dict_dir = os.environ["DICT_DIR"]
        
        if namespace == None:
            output_dir = "%s/serializable/"%(dict_dir)
        else:
            output_dir = "%s/serializable/%s"%(dict_dir,namespace.replace("::","/"))
        
        # make directory
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
        pyfile = output_dir + "/" + obj.get_name() + ".py"
        
        # make empty __init__.py
        open("%s/%s"%(output_dir,"__init__.py"),'w').close()
               
        # Open file for writting here...
        DEBUG.info('Open file: %s' % pyfile)
        self.__fp = open(pyfile,'w')
        if self.__fp == None:
            raise Exception("Could not open %s file.") % pyfile
        DEBUG.info('Completed')
        
    def startSourceFilesVisit(self, obj):
        """
        Defined to generate header for  command python class.
        @parms obj: the instance of the command model to visit.
        """
        c = SerialHeader.SerialHeader()
        d = datetime.datetime.now()
        c.date = d.strftime("%A, %d %B %Y")
        c.user = os.environ['USER']
        c.source = obj.get_xml_filename()
        self._writeTmpl(c, "startSourceFilesVisit")

    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Serial types
        @parms args: the instance of the concrete element to operation on.
        """
        c = SerialImport.SerialImport()
        self._writeTmpl(c, "includes1Visit")

    def includes2Visit(self, obj):
        pass

    def namespaceVisit(self, obj):
        pass

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        c = SerialBody.SerialBody()
        c.name = obj.get_name()
        c.mem_list = list()
        for (n,t,s,f,comment) in obj.get_members():
            # convert XML types to Python classes                    
            (type_string,dontcare,type_name,use_size) = DictTypeConverter.DictTypeConverter().convert(t,s)
            if type_name == "enum":
                format_string = DictTypeConverter.DictTypeConverter().format_replace(f,0,'d','s')
                # check for an error
                if format_string == None:
                    PRINT.info("Member %s in serializable %s had error processing format specifier \"%s\""%(n,c.name,f))
                    sys.exit(-1)
                else:
                    f = format_string            
            c.mem_list.append((n,type_string,f,int(s) if use_size else 1))
            
        self._writeTmpl(c, "publicVisit")


    def protectedVisit(self, obj):
        pass

    def privateVisit(self, obj):
        pass

    def finishSourceFilesVisit(self, obj):
        self.__fp.close()


if __name__ == '__main__':
    pass
