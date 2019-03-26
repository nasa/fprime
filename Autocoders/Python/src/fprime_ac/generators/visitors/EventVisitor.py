#!/bin/env python
#===============================================================================
# NAME: ComponentHVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of component header
#              file.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb 5, 2007
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
from fprime_ac.utils import DictTypeConverter
#from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.visitors import AbstractVisitor
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.events import EventHeader
from fprime_ac.generators.templates.events import EventBody
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
#
# Module class or classes go here.
class EventVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of  Event Python classes.
    """
    __instance = None
    __config   = None
    __fp       = None
    __form     = None
    __form_comment = None
    __model_parser = None

    def __init__(self):
        """
        Constructor.
        """
        self.__config       = ConfigManager.ConfigManager.getInstance()
        self.__form         = formatters.Formatters.getInstance()
        self.__form_comment = formatters.CommentFormatters()
        DEBUG.info("EventVisitor: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""


    def _writeTmpl(self, c, fp, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('EventVisitor:%s' % visit_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug('===================================')     
        
    def DictStartVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the event model to visit.
        """
    
        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/events"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
        
        self.__fp = list();
        
        if len(obj.get_ids()) == 1:
            pyfile = "%s/%s.py" % (output_dir,obj.get_name())
            fd = open(pyfile,'w')
            if fd == None:
                raise Exception("Could not open %s file." % pyfile)
            self.__fp.append(fd)
        else:
            inst = 0
            for id in obj.get_ids():
                pyfile = "%s/%s_%d.py" % (output_dir,obj.get_name(),inst)
                inst+= 1
                DEBUG.info('Open file: %s' % pyfile)
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open %s file." % pyfile)
                DEBUG.info('Completed %s open'%pyfile)
                self.__fp.append(fd)
        


    def DictHeaderVisit(self, obj):
        """
        Defined to generate header for  event python class.
        @parms obj: the instance of the event model to operation on.
        """
        inst = 0
        for id in obj.get_ids():
            c = EventHeader.EventHeader()
            d = datetime.datetime.now()
            c.date = d.strftime("%A, %d %B %Y")
            c.user = os.environ['USER']
            c.source = obj.get_xml_filename()
            self._writeTmpl(c,self.__fp[inst], "eventHeaderVisit")
            inst += 1
         
 
    def DictBodyVisit(self, obj):
        """
        Defined to generate the body of the  Python event class
        @parms obj: the instance of the event model to operation on.
        """
        inst = 0
        for id in obj.get_ids():
            c = EventBody.EventBody()
            if len(obj.get_ids()) > 1:
                c.name = obj.get_name() + "_%d"%inst 
            else:
                c.name = obj.get_name()
            c.id = id
            c.severity = obj.get_severity()
            c.format_string = obj.get_format_string()
            c.description = obj.get_comment()
            c.component = obj.get_component_name()
            
            c.arglist = list()
            c.ser_import_list = list()
            arg_num = 0
            
            for arg_obj in obj.get_args():
                n = arg_obj.get_name()
                t = arg_obj.get_type()
                s = arg_obj.get_size()
                d = arg_obj.get_comment()
                # convert XML types to Python classes                    
                (type_string,ser_import,type_name,dontcare) = DictTypeConverter.DictTypeConverter().convert(t,s)
                if ser_import != None:
                    c.ser_import_list.append(ser_import)
                # convert format specifier if necessary
                if type_name == "enum":
                    format_string = DictTypeConverter.DictTypeConverter().format_replace(c.format_string,arg_num,'d','s')
                    # check for an error
                    if format_string == None:
                        PRINT.info("Event %s in component %s had error processing format specifier"%(c.name,c.component))
                        sys.exit(-1)
                    else:
                        c.format_string = format_string
                    
                c.arglist.append((n,d,type_string))
                arg_num += 1
            self._writeTmpl(c, self.__fp[inst], "eventBodyVisit")
            self.__fp[inst].close()
            inst += 1

if __name__ == '__main__':
    pass
