#!/bin/env python
#===============================================================================
# NAME: ChannelWriter.py
#
# DESCRIPTION: This is the writer class within the dictgen tool that generates
# the channel Python dictionaries for the GDS.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 18, 2019
#
# Copyright 2018, California Institute of Technology.
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

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
#
# Module class or classes go here.

class ChannelWriter(AbstractDictWriter.AbstractDictWriter):
    """
    This class generates GDS Python channel dictionaries
    """
    __config   = None
    __fp       = None
    __form     = None
    __form_comment = None
    __model_parser = None
    
    def __init__():
        """
        Constructor.
        """
        self.__config       = ConfigManager.ConfigManager.getInstance()
        self.__form         = formatters.Formatters.getInstance()
        self.__form_comment = formatters.CommentFormatters()
        DEBUG.info("ChannelWriter: Instanced.")
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
        
        
    def DictStartWrite(self, obj):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the channel model to write.
        """

        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/channels"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
        pyfile = output_dir + "/" + obj.get_name() + ".py"
        
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

    def DictHeaderWrite(self, obj):
        """
        Defined to generate header for channel python class.
        """
        inst = 0
        for id in obj.get_ids():
            c = ChannelHeader.ChannelHeader()
            d = datetime.datetime.now()
            c.date = d.strftime("%A, %d %B %Y")
            c.user = os.environ['USER']
            c.source = obj.get_xml_filename()
            self._writeTmpl(c, self.__fp[inst], "channelHeaderWrite")
            inst += 1

    def DictBodyWrite(self, obj):
        """
        Defined to generate the body of the Python channel class
        @parms obj: the instance of the channel model to operation on.
        """
        inst = 0
        for id in obj.get_ids():
            c = ChannelBody.ChannelBody()
            if len(obj.get_ids()) > 1:
                c.name = obj.get_name() + "_%d"%inst
            else:
                c.name = obj.get_name()
            c.id = id
            c.description = obj.get_comment()
            c.format_string = obj.get_format_string()
            c.component = obj.get_component_name()
            (c.low_red,c.low_orange,c.low_yellow,c.high_yellow,c.high_orange,c.high_red) = obj.get_limits()
                                        
            c.ser_import = None
                                            
            (c.type,c.ser_import,type_name,dontcare) = DictTypeConverter.DictTypeConverter().convert(obj.get_type(),obj.get_size())
            # special case for enums and Gse GUI. Needs to convert %d to %s
            if type_name == "enum":
                c.format_string = "%s"
                                                    
            self._writeTmpl(c, self.__fp[inst], "channelBodyWrite")
            self.__fp[inst].close()
            inst += 1


if __name__ == '__main__':
    pass
