#!/bin/env python
#===============================================================================
# NAME: InstCommandWriter
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
from fprime_ac.models import Command
from fprime_ac.models import Parameter
from fprime_ac.utils import DictTypeConverter

#from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.writers import AbstractDictWriter
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.commands import CommandHeader
from fprime_ac.generators.templates.commands import CommandBody
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
#
# Module class or classes go here.
class InstCommandWriter(AbstractDictWriter.AbstractDictWriter):
    """
    A writer class responsible for generation of  command Python classes.
    """
    __instance = None
    __config   = None
    __fp1       = None
    __fp2       = None
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
        DEBUG.info("InstCommandWriter: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""


    def _writeTmpl(self, c, fp, write_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('InstCommandWriter:%s' % write_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug('===================================')     
        
        
    def DictStartWrite(self, obj , topology_model):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the command model to write.
        """
    
        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/commands"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
            init_file = output_dir + os.sep + "__init__.py"
            open(init_file, "w+")
        
        try:
            instance_obj_list = topology_model.get_base_id_dict()[obj.get_component_base_name()]
        except Exception:
            if type(obj) == Parameter.Parameter or type(obj) == Command.Command:
                PRINT.info("ERROR: Could not find instance object for component " + obj.get_component_base_name() + ". Check topology model to see if the component was instanced.")
            else:
                PRINT.info("ERROR: Could not find instance object for the current component and the current component is not of Parameter or Command type, which are the only two supported command dictionary generation types. Check everything!")
            raise
        
        if type(obj) is Command.Command:
            # open files for commands and instance names
            # open a file for each instance in a 
            # multi-instance component. If there is only
            # one instance, use the command name directly.
            # Otherwise, it will be the instance name + command name
            self.__fp1 = {}
            for instance_obj in instance_obj_list:
                if instance_obj[3].get_dict_short_name() != None:
                    fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , obj.get_mnemonic())
                elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                    fname = obj.get_mnemonic()
                else:
                    fname = "{}_{}".format(instance_obj[0] , obj.get_mnemonic())
                pyfile = "{}/{}.py".format(output_dir , fname)
                DEBUG.info('Open file: {}'.format(pyfile))
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open {} file.".format(pyfile))
                DEBUG.info('Completed {} open'.format(pyfile))
                self.__fp1[fname] = fd
            
           
        elif type(obj) is Parameter.Parameter:
            self.__fp1 = {}
            self.__fp2 = {}
            # Command stem will be component name minus namespace converted to uppercase
            self.__stem = obj.get_name().upper()
            
    
            for instance_obj in instance_obj_list:
                if instance_obj[3].get_dict_short_name() != None:
                    fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , self.__stem)
                elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                    fname = self.__stem
                else:
                    fname = "{}_{}".format(instance_obj[0] , self.__stem)
                pyfile = "{}/{}_PRM_SET.py".format(output_dir,fname)
                DEBUG.info('Open file: {}'.format(pyfile))
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open {} file.".format(pyfile))
                self.__fp1[fname] = fd
                DEBUG.info('Completed {} open'.format(pyfile))
                
                pyfile = "{}/{}_PRM_SAVE.py".format(output_dir,fname)
                DEBUG.info('Open file: {}'.format(pyfile))
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open {} file.".format(pyfile))
                self.__fp2[fname] = fd
                DEBUG.info('Completed {} open'.format(pyfile))

        else:
            print(("Invalid type {}".format(obj)))
            sys.exit(-1)
        
        # Open file for writing here...


    def DictHeaderWrite(self, obj , topology_model):
        """
        Defined to generate header for  command python class.
        @parms obj: the instance of the command model to write.
        """
        
        if type(obj) is Command.Command:
            for fname in list(self.__fp1.keys()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = os.environ['USER']
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[fname], "commandHeaderWrite")
                
        elif type(obj) is Parameter.Parameter:
            # SET Command header
            for fname in list(self.__fp1.keys()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = os.environ['USER']
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[fname], "commandHeaderWrite")

            # SAVE Command header
            for fname in list(self.__fp2.keys()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = os.environ['USER']
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp2[fname], "commandHeaderWrite")
 
    def DictBodyWrite(self, obj , topology_model):
        """
        Defined to generate the body of the  Python command class
        @parms obj: the instance of the command model to operation on.
        """
        try:
            instance_obj_list = topology_model.get_base_id_dict()[obj.get_component_base_name()]
        except Exception:
            PRINT.info("ERROR: Could not find instance object for component " + obj.get_component_base_name() + ". Check topology model to see if the component was instanced.")
            raise
        
        if type(obj) is Command.Command:
            for instance_obj in instance_obj_list:
                c = CommandBody.CommandBody()
                # only add the suffix if there is more than one opcode per command
                if instance_obj[3].get_dict_short_name() != None:
                    fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , obj.get_mnemonic())
                elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                    fname = obj.get_mnemonic()
                else:
                    fname = "{}_{}".format(instance_obj[0] , obj.get_mnemonic())
                c.mnemonic = fname
                try:
                    c.opcode = hex(int(float(obj.get_base_opcode())) + instance_obj[1])
                except:
                    c.opcode = hex(int(obj.get_base_opcode() , 16) + instance_obj[1])
                c.description = obj.get_comment()
                c.component = obj.get_component_name()
                
                c.arglist = list()
                c.ser_import_list = list()
                
                for arg_obj in obj.get_args():
                    # convert XML types to Python classes                    
                    (type_string,ser_import,dontcare,dontcare2) = DictTypeConverter.DictTypeConverter().convert(arg_obj.get_type(),arg_obj.get_size())
                    if ser_import != None:
                        c.ser_import_list.append(ser_import)
                    c.arglist.append((arg_obj.get_name(),arg_obj.get_comment(),type_string))
                self._writeTmpl(c, self.__fp1[fname], "commandBodyWrite")
                self.__fp1[fname].close()
                
        if type(obj) is Parameter.Parameter:
            for instance_obj in instance_obj_list:
                # Set Command
                c = CommandBody.CommandBody()
                
                if instance_obj[3].get_dict_short_name() != None:
                    fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , self.__stem)
                elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                    fname = self.__stem
                else:
                    fname = "{}_{}".format(instance_obj[0] , self.__stem)
                
                c.mnemonic = fname + "_PRM_SET"
                
                try:
                    c.opcode = hex(int(float(obj.get_base_setop())) + instance_obj[1])
                except:
                    c.opcode = hex(int(obj.get_base_setop() , 16) + instance_obj[1])
                    
                c.description = obj.get_comment()
                c.component = obj.get_component_name()
                
                c.arglist = list()
                c.ser_import_list = list()
            
                # convert XML types to Python classes                    
                (type_string,ser_import,dontcare,dontcare2) = DictTypeConverter.DictTypeConverter().convert(obj.get_type(),obj.get_size())
                if ser_import != None:
                    c.ser_import_list.append(ser_import)
                c.arglist.append((obj.get_name(),obj.get_comment(),type_string))
                self._writeTmpl(c, self.__fp1[fname], "commandBodyWrite")
                self.__fp1[fname].close()
            
            for instance_obj in instance_obj_list:
                # Set Command
                c = CommandBody.CommandBody()
                if instance_obj[3].get_dict_short_name() != None:
                    fname = "{}_{}".format(instance_obj[3].get_dict_short_name() , self.__stem)
                elif not topology_model.get_prepend_instance_name() and len(instance_obj_list) == 1:
                    fname = self.__stem
                else:
                    fname = "{}_{}".format(instance_obj[0] , self.__stem)
                
                c.mnemonic = fname + "_PRM_SAVE"
                
                try:
                    c.opcode = hex(int(float(obj.get_base_saveop())) + instance_obj[1])
                except:
                    c.opcode = hex(int(obj.get_base_saveop() , 16) + instance_obj[1])
                c.description = obj.get_comment()
                c.component = obj.get_component_name()
                
                c.arglist = list()
                c.ser_import_list = list()         

                self._writeTmpl(c, self.__fp2[fname], "commandBodyWrite")
                self.__fp2[fname].close()


if __name__ == '__main__':
    pass
