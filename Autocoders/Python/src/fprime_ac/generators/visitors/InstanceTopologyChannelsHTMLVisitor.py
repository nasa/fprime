#!/bin/env python
#===============================================================================
# NAME: InstanceTopologyHTMLVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of HTML tables
#              of channel ID's, etc.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Sep. 13, 2016
#
# Copyright 2016, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import logging
import os
import sys
import time
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
from fprime_ac.generators.visitors import ComponentVisitorBase
from fprime_ac.generators import formatters
#
# Import precompiled templates here
#
from fprime_ac.generators.templates.html import HtmlChannelTablePage
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')
#
# Module class or classes go here.
class InstanceTopologyChannelsHTMLVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of component header
    classes in C++.
    """
    __instance = None
    __config   = None
    __fp_dict  = None
    __form     = None
    __form_comment = None
    __model_parser = None

    def __init__(self):
        """
        Constructor.
        """
        self.__config       = ConfigManager.ConfigManager.getInstance()
        self.__form         = formatters.Formatters()
        self.__form_comment = formatters.CommentFormatters()
        self.__model_parser = ModelParser.ModelParser.getInstance()
        self.__cmd_dir = "channels"
        DEBUG.info("InstanceTopologyChannelHTMLVisitor: Instanced.")
        self.bodytext       = ""
        self.prototypetext  = ""
        self.__fp_dict      = dict() # dictionary of instance name keyword to file handle pointer


    def _writeTmpl(self, instance, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug('InstanceTopologyChannelHTMLVisitor:%s' % visit_str)
        DEBUG.debug('===================================')
        DEBUG.debug(c)
        self.__fp_dict[instance].writelines(c.__str__())
        DEBUG.debug('===================================')


    def initFilesVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @parms obj: the instance of the model to visit.
        """
        # Check for command dir here and if none creat it but always switch into it
        if not os.path.exists(self.__cmd_dir):
            os.mkdir(self.__cmd_dir)
        os.chdir(self.__cmd_dir)
        # Iterate over types
        for k in list(obj.get_base_id_dict().keys()):
            tlist = obj.get_base_id_dict()[k]
            #print "Type: %s\n" % k,
            # Iterate over instances and get name
            # Open file if commands exist if not do nothing
            for t in tlist:
                # print "\tInstance: %s, Base ID: %s\n" % (t[0],t[1])
                name = t[0]
                ch_list = t[3].get_comp_xml().get_channels()
                if len(ch_list) > 0:
                    filename = "%s_channels.html" % t[0]
                    # Open file for writing here...
                    DEBUG.info('Open file: %s' % filename)
                    try:
                        self.__fp_dict[name] = open(filename,'w')
                        DEBUG.info('Completed')
                    except IOError:
                        PRINT.info("Could not open %s file." % filename)
                        sys.exit(-1)
                    DEBUG.info("Generating HTML Channels Table for %s:%s component instance..." % (t[0], k))
        os.chdir("..")

    def startSourceFilesVisit(self, obj):
        """
        Defined to generate starting static code within files.
        """
        pass


    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Port types
        @parms args: the instance of the concrete element to operation on.
        """
        pass


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
        pass

    def initTelemetryParams(self, obj, c):
        '''
        Telemetry function parameters for code generation
        '''
        c.param_tlm_id = (
            "id",
            "const FwChanIdType",
            "The channel ID"
        )
        c.param_val = (
            "val",
            "Fw::TlmBuffer&",
            "The channel value"
        )

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @parms args: the instance of the concrete element to operation on.
        """
        c = HtmlChannelTablePage.HtmlChannelTablePage()
        for k in list(obj.get_base_id_dict().keys()):
            tlist = obj.get_base_id_dict()[k]
            #print "Type: %s\n" % k,
            for t in tlist:
                if (t[0] in list(self.__fp_dict.keys())):
                    # print "\tInstance: %s, Base ID: %s\n" % (t[0],t[1])
                    cobj = t[3].get_comp_xml()
                    c.name = "%s:%s" % (t[0], k)
                    c.base_id = t[1]
                    c.has_telemetry = len(cobj.get_channels()) > 0
                    c.has_channels = c.has_telemetry
                    c.channel_enums = self.__model_parser.getTelemEnumList(cobj)
                    c.channels = self.__model_parser.getChannelsList(cobj)
                    self.initTelemetryParams(cobj, c)
                    self._writeTmpl(t[0], c, "InstanceTopologyChannelsHTML_Visitor")


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
        for fp in list(self.__fp_dict.keys()):
            self.__fp_dict[fp].close()
        PRINT.info("Completed generating HTML channel tables...")


if __name__ == '__main__':
    pass
