#!/bin/env python
#===============================================================================
# NAME: InstEventWriter.py
#
# DESCRIPTION: A writer responsible for the generation of component header
#              file.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED  : June 18, 2019
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
from fprime_ac.generators.writers import AbstractDictWriter
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
class InstEventWriter(AbstractDictWriter.AbstractDictWriter):
    """
    A writer class responsible for generation of  Event Python classes.
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
        DEBUG.info("InstEventWriter: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""


    def _writeTmpl(self, c, fp, write_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('InstEventWriter:%s' % write_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug('===================================')     
       
    def DictStartWrite(self, obj , topology_model):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the event model to write.
        """
    
        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/events"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
            init_file = output_dir + os.sep + "__init__.py"
            open(init_file, "w+")
        
        self.__fp = {};
        
        try:
            instance_obj_list = topology_model.get_base_id_dict()[obj.get_component_base_name()]
        except Exception:
            PRINT.info("ERROR: Could not find instance object for component " + obj.get_component_base_name() + ". Check topology model to see if the component was instanced.")
            raise
        
        for instance_obj in instance_obj_list:
            if instance_obj[3].get_dict_short_name() != None:
                fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , obj.get_name())
            elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                fname = obj.get_name()
            else:
                fname = "{}_{}".format(instance_obj[0] , obj.get_name())
                
            pyfile = "{}/{}.py".format(output_dir , fname)
            DEBUG.info('Open file: {}'.format(pyfile))
            fd = open(pyfile,'w')
            if fd == None:
                raise Exception("Could not open {} file.".format(pyfile))
            DEBUG.info('Completed {} open'.format(pyfile))
            self.__fp[fname] = fd        


    def DictHeaderWrite(self, obj , topology_model):
        """
        Defined to generate header for  event python class.
        @parms obj: the instance of the event model to operation on.
        """
        
        for fname in list(self.__fp.keys()):
            c = EventHeader.EventHeader()
            d = datetime.datetime.now()
            c.date = d.strftime("%A, %d %B %Y")
            c.user = os.environ['USER']
            c.source = obj.get_xml_filename()
            self._writeTmpl(c, self.__fp[fname], "eventHeaderWrite")

         
 
    def DictBodyWrite(self, obj , topology_model):
        """
        Defined to generate the body of the  Python event class
        @parms obj: the instance of the event model to operation on.
        """
        try:
            instance_obj_list = topology_model.get_base_id_dict()[obj.get_component_base_name()]
        except Exception:
            PRINT.info("ERROR: Could not find instance object for component " + obj.get_component_base_name() + ". Check topology model to see if the component was instanced.")
            raise
        
        for instance_obj in instance_obj_list:
            c = EventBody.EventBody()
            if instance_obj[3].get_dict_short_name() != None:
                fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , obj.get_name())
            elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                fname = obj.get_name()
            else:
                fname = "{}_{}".format(instance_obj[0] , obj.get_name())
            c.name = fname
            
            if len(obj.get_ids()) > 1:
                raise Exception("There is more than one event id when creating dictionaries. Check xml of {} or see if multiple explicit IDs exist in the AcConstants.ini file".format(fname))
            try:
                c.id = hex(instance_obj[1]  + int(float(obj.get_ids()[0])))
            except:
                c.id = hex(instance_obj[1]  + int(obj.get_ids()[0] , 16))
                
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
            self._writeTmpl(c, self.__fp[fname], "eventBodyWrite")
            self.__fp[fname].close()
            

if __name__ == '__main__':
    pass
