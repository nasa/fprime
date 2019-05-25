#!/bin/env python
#===============================================================================
# NAME: CommandVisitor.py
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
from fprime_ac.models import Command
from fprime_ac.models import Parameter
from fprime_ac.utils import DictTypeConverter

#from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.visitors import AbstractVisitor
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
class CommandVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of  command Python classes.
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
        DEBUG.info("CommandVisitor: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""


    def _writeTmpl(self, c, fp, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('CommandVisitor:%s' % visit_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug('===================================')     
        
        
    def DictStartVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the command model to visit.
        """
    
        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/commands"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
        # could be command or parameter object
        if type(obj) is Command.Command:
            # open files for commands and opcodes
            # open a file for each opcode in a 
            # multi-instance component. If there is only
            # one instance, use the opcode directly.
            # Otherwise, it will be the opcode + instance
            self.__fp1 = list();
            
            if len(obj.get_opcodes()) == 1:
                pyfile = "%s/%s.py" % (output_dir,obj.get_mnemonic())
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open %s file." % pyfile)
                self.__fp1.append(fd)
            else:
                inst = 0
                for opcode in obj.get_opcodes():
                    pyfile = "%s/%s_%d.py" % (output_dir,obj.get_mnemonic(),inst)
                    inst+= 1
                    DEBUG.info('Open file: %s' % pyfile)
                    fd = open(pyfile,'w')
                    if fd == None:
                        raise Exception("Could not open %s file." % pyfile)
                    DEBUG.info('Completed %s open'%pyfile)
                    self.__fp1.append(fd)
        elif type(obj) is Parameter.Parameter:
            self.__fp1 = list();
            self.__fp2 = list();
            # Command stem will be component name minus namespace converted to uppercase
            self.__stem = obj.get_name().upper()
            
            if len(obj.get_set_opcodes()) == 1:
                # set/save opcode numbers had better match
                if len(obj.get_set_opcodes()) != len(obj.get_save_opcodes()):
                    raise Exception("set/save opcode quantities do not match!")
                pyfile = "%s/%s_PRM_SET.py" %(output_dir,self.__stem)
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open %s file." % pyfile)
                self.__fp1.append(fd)

                pyfile = "%s/%s_PRM_SAVE.py" %(output_dir,self.__stem)
                fd = open(pyfile,'w')
                if fd == None:
                    raise Exception("Could not open %s file." % pyfile)
                self.__fp2.append(fd)
            else:
                inst = 0
                for opcode in obj.get_set_opcodes():
                    pyfile = "%s/%s_%d_PRM_SET.py" %(output_dir,self.__stem,inst)
                    DEBUG.info('Open file: %s' % pyfile)
                    fd = open(pyfile,'w')
                    if fd == None:
                        raise Exception("Could not open %s file." % pyfile)
                    self.__fp1.append(fd)
                    DEBUG.info('Completed %s open'%pyfile)
                    
                    pyfile = "%s/%s_%d_PRM_SAVE.py" %(output_dir,self.__stem,inst)
                    DEBUG.info('Open file: %s' % pyfile)
                    fd = open(pyfile,'w')
                    if fd == None:
                        raise Exception("Could not open %s file." % pyfile)
                    self.__fp2.append(fd)
                    inst+= 1
                    DEBUG.info('Completed %s open'%pyfile)

        else:
            print(("Invalid type %s"%type(obj)))
            sys.exit(-1)
        
        # Open file for writing here...


    def DictHeaderVisit(self, obj):
        """
        Defined to generate header for  command python class.
        @parms obj: the instance of the command model to visit.
        """
        if type(obj) is Command.Command:
            inst = 0
            for opcode in obj.get_opcodes():
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = os.environ['USER']
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[inst], "commandHeaderVisit")
                inst += 1
                
        elif type(obj) is Parameter.Parameter:
            # SET Command header
            inst = 0
            for opcode in obj.get_set_opcodes():
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = os.environ['USER']
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[inst], "commandHeaderVisit")
                inst += 1

            # SAVE Command header
            inst = 0
            for opcode in obj.get_save_opcodes():
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = os.environ['USER']
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp2[inst], "commandHeaderVisit")
                inst += 1
 
    def DictBodyVisit(self, obj):
        """
        Defined to generate the body of the  Python command class
        @parms obj: the instance of the command model to operation on.
        """
        if type(obj) is Command.Command:
            inst = 0
            for opcode in obj.get_opcodes():
                c = CommandBody.CommandBody()
                # only add the suffix if there is more than one opcode per command
                if (len(obj.get_opcodes())> 1):
                    c.mnemonic = obj.get_mnemonic() + "_%d"%inst
                else:
                    c.mnemonic = obj.get_mnemonic()
                    
                c.opcode = opcode
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
                self._writeTmpl(c, self.__fp1[inst], "commandBodyVisit")
                self.__fp1[inst].close()
                inst += 1
        if type(obj) is Parameter.Parameter:
            inst = 0
            for opcode in obj.get_set_opcodes():
                # Set Command
                c = CommandBody.CommandBody()
                if len(obj.get_set_opcodes()) > 1:
                    c.mnemonic = "%s_%d_PRM_SET" %(self.__stem,inst)
                else:
                    c.mnemonic = "%s_PRM_SET" %(self.__stem)
                    
                c.opcode = opcode
                c.description = obj.get_comment()
                c.component = obj.get_component_name()
                
                c.arglist = list()
                c.ser_import_list = list()
            
                # convert XML types to Python classes                    
                (type_string,ser_import,dontcare,dontcare2) = DictTypeConverter.DictTypeConverter().convert(obj.get_type(),obj.get_size())
                if ser_import != None:
                    c.ser_import_list.append(ser_import)
                c.arglist.append((obj.get_name(),obj.get_comment(),type_string))
                self._writeTmpl(c, self.__fp1[inst], "commandBodyVisit")
                self.__fp1[inst].close()
                inst += 1
            
            inst = 0
            for opcode in obj.get_save_opcodes():
                # Save Command
                c = CommandBody.CommandBody()
                if (len(obj.get_save_opcodes()) > 1):
                    c.mnemonic = "%s_%d_PRM_SAVE" %(self.__stem,inst)
                else:
                    c.mnemonic = "%s_PRM_SAVE" %(self.__stem)
                c.opcode = opcode
                c.description = obj.get_comment()
                c.component = obj.get_component_name()
                
                c.arglist = list()
                c.ser_import_list = list()
                
                self._writeTmpl(c, self.__fp2[inst], "commandBodyVisit")
                self.__fp2[inst].close()
                inst += 1
            

if __name__ == '__main__':
    pass
