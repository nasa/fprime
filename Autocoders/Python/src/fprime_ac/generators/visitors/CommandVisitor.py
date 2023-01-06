# ===============================================================================
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

# from fprime_ac.utils import DiffAndRename
from fprime_ac.generators.visitors import AbstractVisitor
from fprime_ac.models import Command, Parameter

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
    from fprime_ac.generators.templates.commands import CommandBody, CommandHeader
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
class CommandVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of  command Python classes.
    """

    __instance = None
    __config = None
    __fp1 = None
    __fp2 = None
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
        DEBUG.info("CommandVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _writeTmpl(self, c, fp, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug(f"CommandVisitor:{visit_str}")
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def DictStartVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @param obj: the instance of the command model to visit.
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
            self.__fp1 = []

            if len(obj.get_opcodes()) == 1:
                pyfile = "{}/{}.py".format(output_dir, obj.get_mnemonic())
                fd = open(pyfile, "w")
                self.__fp1.append(fd)
            else:
                for inst, opcode in enumerate(obj.get_opcodes()):
                    pyfile = "%s/%s_%d.py" % (output_dir, obj.get_mnemonic(), inst)
                    DEBUG.info(f"Open file: {pyfile}")
                    fd = open(pyfile, "w")
                    DEBUG.info(f"Completed {pyfile} open")
                    self.__fp1.append(fd)
        elif type(obj) is Parameter.Parameter:
            self.__fp1 = []
            self.__fp2 = []
            # Command stem will be component name minus namespace converted to uppercase
            self.__stem = obj.get_name().upper()

            if len(obj.get_set_opcodes()) == 1:
                # set/save opcode numbers had better match
                if len(obj.get_set_opcodes()) != len(obj.get_save_opcodes()):
                    raise ValueError("set/save opcode quantities do not match!")
                pyfile = "{}/{}_PRM_SET.py".format(output_dir, self.__stem)
                fd = open(pyfile, "w")
                self.__fp1.append(fd)

                pyfile = "{}/{}_PRM_SAVE.py".format(output_dir, self.__stem)
                fd = open(pyfile, "w")
                self.__fp2.append(fd)
            else:
                for inst, opcode in enumerate(obj.get_set_opcodes()):
                    pyfile = "%s/%s_%d_PRM_SET.py" % (output_dir, self.__stem, inst)
                    DEBUG.info(f"Open file: {pyfile}")
                    fd = open(pyfile, "w")
                    self.__fp1.append(fd)
                    DEBUG.info(f"Completed {pyfile} open")

                    pyfile = "%s/%s_%d_PRM_SAVE.py" % (output_dir, self.__stem, inst)
                    DEBUG.info(f"Open file: {pyfile}")
                    fd = open(pyfile, "w")
                    self.__fp2.append(fd)
                    DEBUG.info(f"Completed {pyfile} open")

        else:
            print("Invalid type %s" % type(obj))
            sys.exit(-1)

        # Open file for writing here...

    def DictHeaderVisit(self, obj):
        """
        Defined to generate header for  command python class.
        @param obj: the instance of the command model to visit.
        """
        if type(obj) is Command.Command:
            for inst, opcode in enumerate(obj.get_opcodes()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = getuser()
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[inst], "commandHeaderVisit")

        elif type(obj) is Parameter.Parameter:
            # SET Command header
            for inst, opcode in enumerate(obj.get_set_opcodes()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = getuser()
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[inst], "commandHeaderVisit")

            # SAVE Command header
            for inst, opcode in enumerate(obj.get_save_opcodes()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = getuser()
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp2[inst], "commandHeaderVisit")

    def DictBodyVisit(self, obj):
        """
        Defined to generate the body of the  Python command class
        @param obj: the instance of the command model to operation on.
        """
        if type(obj) is Command.Command:
            for inst, opcode in enumerate(obj.get_opcodes()):
                c = CommandBody.CommandBody()
                # only add the suffix if there is more than one opcode per command
                if len(obj.get_opcodes()) > 1:
                    c.mnemonic = obj.get_mnemonic() + "_%d" % inst
                else:
                    c.mnemonic = obj.get_mnemonic()

                c.opcode = opcode
                c.description = obj.get_comment()
                c.component = obj.get_component_name()

                c.arglist = []
                c.ser_import_list = []

                for arg_obj in obj.get_args():
                    # convert XML types to Python classes
                    (
                        type_string,
                        ser_import,
                        dontcare,
                        dontcare2,
                    ) = DictTypeConverter.DictTypeConverter().convert(
                        arg_obj.get_type(), arg_obj.get_size()
                    )
                    if ser_import is not None:
                        c.ser_import_list.append(ser_import)
                    c.arglist.append(
                        (arg_obj.get_name(), arg_obj.get_comment(), type_string)
                    )
                self._writeTmpl(c, self.__fp1[inst], "commandBodyVisit")
                self.__fp1[inst].close()
        if type(obj) is Parameter.Parameter:
            for inst, opcode in enumerate(obj.get_set_opcodes()):
                # Set Command
                c = CommandBody.CommandBody()
                if len(obj.get_set_opcodes()) > 1:
                    c.mnemonic = "%s_%d_PRM_SET" % (self.__stem, inst)
                else:
                    c.mnemonic = f"{self.__stem}_PRM_SET"

                c.opcode = opcode
                c.description = obj.get_comment()
                c.component = obj.get_component_name()

                c.arglist = []
                c.ser_import_list = []

                # convert XML types to Python classes
                (
                    type_string,
                    ser_import,
                    dontcare,
                    dontcare2,
                ) = DictTypeConverter.DictTypeConverter().convert(
                    obj.get_type(), obj.get_size()
                )
                if ser_import is not None:
                    c.ser_import_list.append(ser_import)
                c.arglist.append((obj.get_name(), obj.get_comment(), type_string))
                self._writeTmpl(c, self.__fp1[inst], "commandBodyVisit")
                self.__fp1[inst].close()

            for inst, opcode in enumerate(obj.get_save_opcodes()):
                # Save Command
                c = CommandBody.CommandBody()
                if len(obj.get_save_opcodes()) > 1:
                    c.mnemonic = "%s_%d_PRM_SAVE" % (self.__stem, inst)
                else:
                    c.mnemonic = f"{self.__stem}_PRM_SAVE"
                c.opcode = opcode
                c.description = obj.get_comment()
                c.component = obj.get_component_name()

                c.arglist = []
                c.ser_import_list = []

                self._writeTmpl(c, self.__fp2[inst], "commandBodyVisit")
                self.__fp2[inst].close()
