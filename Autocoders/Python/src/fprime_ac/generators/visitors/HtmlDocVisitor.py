# ===============================================================================
# NAME: HtmlDocVisitor.py
#
# DESCRIPTION: A visitor for generating component dictionary doc files
#
# AUTHOR: tcanham
# EMAIL:  tcanham@jpl.nasa.gov
# DATE CREATED  : August 31, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
import logging
import os
import sys

from fprime_ac.generators.visitors import ComponentVisitorBase

#
# Python extension modules and custom interfaces
#
# from Cheetah import Template
# from fprime_ac.utils import version

# from fprime_ac.utils import DiffAndRename


try:
    from fprime_ac.generators.templates.html import HtmlDocPage
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")


class HtmlDocVisitor(ComponentVisitorBase.ComponentVisitorBase):
    """
    A visitor for generating component dictionary doc files,
    """

    def __init__(self):
        super().__init__()
        self.initBase("HtmlDoc")

    def htmlStartPage(self, obj):
        output_dir = os.environ["HTML_DOC_SUBDIR"]
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
        htmlfile = output_dir + "/" + obj.get_name() + ".html"
        self.openFile(htmlfile)

    def htmlPageVisit(self, obj):
        c = HtmlDocPage.HtmlDocPage()
        self.init(obj, c)
        self._writeTmpl(c, "htmlPage")
