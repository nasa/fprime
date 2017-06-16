#!/bin/env python
#===============================================================================
# NAME: MdDocVisitor.py
#
# DESCRIPTION: A visitor for generating component dictionary doc files in MarkDown
#
# AUTHOR: tcanham
# EMAIL:  tcanham@jpl.nasa.gov
# DATE CREATED  : April 21, 20167
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
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
#from utils import version
from utils import ConfigManager
from models import ModelParser
#from utils import DiffAndRename
from utils import DictTypeConverter
from generators.visitors import AbstractVisitor
from generators import formatters

from generators.visitors import ComponentVisitorBase
from generators.templates.md import MdDocPage

PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

class MdDocVisitor(ComponentVisitorBase.ComponentVisitorBase):
    """
    A visitor for generating component dictionary doc files in MarkDown
    """

    def __init__(self):
        self.initBase("MdDoc")
        
    def mdStartPage(self, obj):
        output_dir = os.environ["MD_DOC_SUBDIR"]
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
        htmlfile = output_dir + "/" + obj.get_name() + ".md"
        self.openFile(htmlfile)
        
    def mdPageVisit(self, obj):
        c = MdDocPage.MdDocPage()
        self.init(obj, c)
        self._writeTmpl(c, "mdPage")
