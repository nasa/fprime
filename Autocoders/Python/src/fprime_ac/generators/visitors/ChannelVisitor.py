# ===============================================================================
# NAME: ChannelVisitor.py
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
# ===============================================================================
#
# Python standard modules
#
import datetime
import logging
import os
import sys
from getpass import getuser

from fprime_ac.generators import formatters
from fprime_ac.generators.visitors import AbstractVisitor

# from fprime_ac.utils import DiffAndRename
#
# Python extension modules and custom interfaces
#
# from Cheetah import Template
# from fprime_ac.utils import version
from fprime_ac.utils import ConfigManager, DictTypeConverter

#
# Import precompiled templates here
#
try:
    from fprime_ac.generators.templates.channels import ChannelBody, ChannelHeader
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
# Module class or classes go here.
class ChannelVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of channel Python classes.
    """

    __instance = None
    __config = None
    __fp = None
    __form = None
    __form_comment = None
    __model_parser = None

    def __init__(self):
        """
        Constructor.
        """
        super().__init__()
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.__form = formatters.Formatters.getInstance()
        self.__form_comment = formatters.CommentFormatters()
        DEBUG.info("ChannelVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _writeTmpl(self, c, fp, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug(f"ChannelVisitor:{visit_str}")
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def DictStartVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @param obj: the instance of the channel model to visit.
        """

        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/channels"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)

        self.__fp = []

        if len(obj.get_ids()) == 1:
            pyfile = "{}/{}.py".format(output_dir, obj.get_name())
            fd = open(pyfile, "w")
            self.__fp.append(fd)
        else:
            for inst, id in enumerate(obj.get_ids()):
                pyfile = "%s/%s_%d.py" % (output_dir, obj.get_name(), inst)
                DEBUG.info(f"Open file: {pyfile}")
                fd = open(pyfile, "w")
                DEBUG.info(f"Completed {pyfile} open")
                self.__fp.append(fd)

    def DictHeaderVisit(self, obj):
        """
        Defined to generate header for channel python class.
        """
        for inst, id in enumerate(obj.get_ids()):
            c = ChannelHeader.ChannelHeader()
            d = datetime.datetime.now()
            c.date = d.strftime("%A, %d %B %Y")
            c.user = getuser()
            c.source = obj.get_xml_filename()
            self._writeTmpl(c, self.__fp[inst], "channelHeaderVisit")

    def DictBodyVisit(self, obj):
        """
        Defined to generate the body of the Python channel class
        @param obj: the instance of the channel model to operation on.
        """
        for inst, id in enumerate(obj.get_ids()):
            c = ChannelBody.ChannelBody()
            if len(obj.get_ids()) > 1:
                c.name = obj.get_name() + "_%d" % inst
            else:
                c.name = obj.get_name()
            c.id = id
            c.description = obj.get_comment()
            c.format_string = obj.get_format_string()
            c.component = obj.get_component_name()
            (
                c.low_red,
                c.low_orange,
                c.low_yellow,
                c.high_yellow,
                c.high_orange,
                c.high_red,
            ) = obj.get_limits()

            c.ser_import = None

            (
                c.type,
                c.ser_import,
                type_name,
                dontcare,
            ) = DictTypeConverter.DictTypeConverter().convert(
                obj.get_type(), obj.get_size()
            )
            # special case for enums and Gse GUI. Needs to convert %d to %s
            if type_name == "enum":
                c.format_string = "%s"

            self._writeTmpl(c, self.__fp[inst], "channelBodyVisit")
            self.__fp[inst].close()
