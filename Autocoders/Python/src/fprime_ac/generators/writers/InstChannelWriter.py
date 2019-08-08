#!/bin/env python
#===============================================================================
# NAME: InstChannelWriter.py
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
#from fprime_ac.utils import DiffAndRename
from fprime_ac.utils import DictTypeConverter
from fprime_ac.generators.writers import AbstractDictWriter
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.channels import ChannelHeader
from fprime_ac.generators.templates.channels import ChannelBody

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
#
# Module class or classes go here.
class InstChannelWriter(AbstractDictWriter.AbstractDictWriter):
    """
    A writer class responsible for generation of  channel Python classes.
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
        DEBUG.info("InstChannelWriter: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""


    def _writeTmpl(self, c, fp, write_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('ChannelWriter:%s' % write_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug('===================================')     
        
        
    def DictStartWrite(self, obj , topology_model):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the channel model to write.
        """
    
        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/channels"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
            init_file = output_dir + os.sep + "__init__.py"
            open(init_file, "w+")
            
        self.__fp = {}
        
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
        Defined to generate header for  channel python class.
        """
        
        for fname in list(self.__fp.keys()):
            c = ChannelHeader.ChannelHeader()
            d = datetime.datetime.now()
            c.date = d.strftime("%A, %d %B %Y")
            c.user = os.environ['USER']
            c.source = obj.get_xml_filename()
            self._writeTmpl(c, self.__fp[fname], "channelHeaderWrite")
      
 
    def DictBodyWrite(self, obj , topology_model):
        """
        Defined to generate the body of the  Python channel class
        @parms obj: the instance of the channel model to operation on.
        """
        try:
            instance_obj_list = topology_model.get_base_id_dict()[obj.get_component_base_name()]
        except Exception:
            PRINT.info("ERROR: Could not find instance object for component " + obj.get_component_base_name() + ". Check topology model to see if the component was instanced.")
            raise
        
        for instance_obj in instance_obj_list:
            c = ChannelBody.ChannelBody()
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
            
            c.description = obj.get_comment()
            c.format_string = obj.get_format_string()
            c.component = obj.get_component_name()
            (c.low_red,c.low_orange,c.low_yellow,c.high_yellow,c.high_orange,c.high_red) = obj.get_limits()

            c.ser_import = None
            
            (c.type,c.ser_import,type_name,dontcare) = DictTypeConverter.DictTypeConverter().convert(obj.get_type(),obj.get_size())
            # special case for enums and Gse GUI. Needs to convert %d to %s
            if type_name == "enum":
                c.format_string = "%s"            
    
            self._writeTmpl(c, self.__fp[fname], "channelBodyWrite")
            self.__fp[fname].close()
                
            

if __name__ == '__main__':
    pass
