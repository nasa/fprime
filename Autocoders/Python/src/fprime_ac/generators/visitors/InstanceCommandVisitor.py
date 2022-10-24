# ===============================================================================
# NAME: ComponentHVisitor.py
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
class InstanceCommandVisitor(AbstractVisitor.AbstractVisitor):
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
        DEBUG.info("InstanceCommandVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _writeTmpl(self, c, fp, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug("InstanceCommandVisitor:%s" % visit_str)
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def DictStartVisit(self, obj, topology_model):
        """
        Defined to generate files for generated code products.
        @param obj: the instance of the command model to visit.
        """

        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/commands"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
            init_file = os.path.join(output_dir, "__init__.py")
            open(init_file, "w+").close()

        try:
            instance_obj_list = topology_model.get_base_id_dict()[
                obj.get_component_base_name()
            ]
        except Exception:
            if isinstance(obj, (Parameter.Parameter, Command.Command)):
                PRINT.info(
                    "ERROR: Could not find instance object for component "
                    + obj.get_component_base_name()
                    + ". Check topology model to see if the component was instanced."
                )
            else:
                PRINT.info(
                    "ERROR: Could not find instance object for the current component and the current component is not of Parameter or Command type, which are the only two supported command dictionary generation types. Check everything!"
                )
            raise

        if type(obj) is Command.Command:
            # open files for commands and instance names
            # open a file for each instance in a
            # multi-instance component. If there is only
            # one instance, use the command name directly.
            # Otherwise, it will be the instance name + command name
            self.__fp1 = {}
            for instance_obj in instance_obj_list:
                if instance_obj[3].get_dict_short_name() is not None:
                    fname = "{}_{}".format(
                        instance_obj[3].get_dict_short_name(), obj.get_mnemonic()
                    )
                elif (
                    not topology_model.get_prepend_instance_name()
                    and len(instance_obj_list) == 1
                ):
                    fname = obj.get_mnemonic()
                else:
                    fname = "{}_{}".format(instance_obj[0], obj.get_mnemonic())
                pyfile = "{}/{}.py".format(output_dir, fname)
                DEBUG.info("Open file: {}".format(pyfile))
                fd = open(pyfile, "w")
                DEBUG.info("Completed {} open".format(pyfile))
                self.__fp1[fname] = fd

        elif type(obj) is Parameter.Parameter:
            self.__fp1 = {}
            self.__fp2 = {}
            # Command stem will be component name minus namespace converted to uppercase
            self.__stem = obj.get_name().upper()

            for instance_obj in instance_obj_list:
                if instance_obj[3].get_dict_short_name() is not None:
                    fname = "{}_{}".format(
                        instance_obj[3].get_dict_short_name(), self.__stem
                    )
                elif (
                    not topology_model.get_prepend_instance_name()
                    and len(instance_obj_list) == 1
                ):
                    fname = self.__stem
                else:
                    fname = "{}_{}".format(instance_obj[0], self.__stem)
                pyfile = "{}/{}_PRM_SET.py".format(output_dir, fname)
                DEBUG.info("Open file: {}".format(pyfile))
                fd = open(pyfile, "w")
                self.__fp1[fname] = fd
                DEBUG.info("Completed {} open".format(pyfile))

                pyfile = "{}/{}_PRM_SAVE.py".format(output_dir, fname)
                DEBUG.info("Open file: {}".format(pyfile))
                fd = open(pyfile, "w")
                self.__fp2[fname] = fd
                DEBUG.info("Completed {} open".format(pyfile))

        else:
            print("Invalid type {}".format(obj))
            sys.exit(-1)

        # Open file for writing here...

    def DictHeaderVisit(self, obj, topology_model):
        """
        Defined to generate header for  command python class.
        @param obj: the instance of the command model to visit.
        """

        if type(obj) is Command.Command:
            for fname in list(self.__fp1.keys()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = getuser()
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[fname], "commandHeaderVisit")

        elif type(obj) is Parameter.Parameter:
            # SET Command header
            for fname in list(self.__fp1.keys()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = getuser()
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp1[fname], "commandHeaderVisit")

            # SAVE Command header
            for fname in list(self.__fp2.keys()):
                c = CommandHeader.CommandHeader()
                d = datetime.datetime.now()
                c.date = d.strftime("%A, %d %B %Y")
                c.user = getuser()
                c.source = obj.get_xml_filename()
                self._writeTmpl(c, self.__fp2[fname], "commandHeaderVisit")

    def DictBodyVisit(self, obj, topology_model):
        """
        Defined to generate the body of the  Python command class
        @param obj: the instance of the command model to operation on.
        """
        try:
            instance_obj_list = topology_model.get_base_id_dict()[
                obj.get_component_base_name()
            ]
        except Exception:
            PRINT.info(
                "ERROR: Could not find instance object for component "
                + obj.get_component_base_name()
                + ". Check topology model to see if the component was instanced."
            )
            raise

        if type(obj) is Command.Command:
            for instance_obj in instance_obj_list:
                c = CommandBody.CommandBody()
                # only add the suffix if there is more than one opcode per command
                if instance_obj[3].get_dict_short_name() is not None:
                    fname = "{}_{}".format(
                        instance_obj[3].get_dict_short_name(), obj.get_mnemonic()
                    )
                elif (
                    not topology_model.get_prepend_instance_name()
                    and len(instance_obj_list) == 1
                ):
                    fname = obj.get_mnemonic()
                else:
                    fname = "{}_{}".format(instance_obj[0], obj.get_mnemonic())
                c.mnemonic = fname
                try:
                    c.opcode = hex(int(float(obj.get_base_opcode())) + instance_obj[1])
                except (TypeError, ValueError):
                    c.opcode = hex(int(obj.get_base_opcode(), 16) + instance_obj[1])
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
                self._writeTmpl(c, self.__fp1[fname], "commandBodyVisit")
                self.__fp1[fname].close()

        if type(obj) is Parameter.Parameter:
            for instance_obj in instance_obj_list:
                # Set Command
                c = CommandBody.CommandBody()

                if instance_obj[3].get_dict_short_name() is not None:
                    fname = "{}_{}".format(
                        instance_obj[3].get_dict_short_name(), self.__stem
                    )
                elif (
                    not topology_model.get_prepend_instance_name()
                    and len(instance_obj_list) == 1
                ):
                    fname = self.__stem
                else:
                    fname = "{}_{}".format(instance_obj[0], self.__stem)

                c.mnemonic = fname + "_PRM_SET"

                try:
                    c.opcode = hex(int(float(obj.get_base_setop())) + instance_obj[1])
                except (ValueError, TypeError):
                    c.opcode = hex(int(obj.get_base_setop(), 16) + instance_obj[1])

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
                self._writeTmpl(c, self.__fp1[fname], "commandBodyVisit")
                self.__fp1[fname].close()

            for instance_obj in instance_obj_list:
                # Set Command
                c = CommandBody.CommandBody()
                if instance_obj[3].get_dict_short_name() is not None:
                    fname = "{}_{}".format(
                        instance_obj[3].get_dict_short_name(), self.__stem
                    )
                elif (
                    not topology_model.get_prepend_instance_name()
                    and len(instance_obj_list) == 1
                ):
                    fname = self.__stem
                else:
                    fname = "{}_{}".format(instance_obj[0], self.__stem)

                c.mnemonic = fname + "_PRM_SAVE"

                try:
                    c.opcode = hex(int(float(obj.get_base_saveop())) + instance_obj[1])
                except (ValueError, TypeError):
                    c.opcode = hex(int(obj.get_base_saveop(), 16) + instance_obj[1])
                c.description = obj.get_comment()
                c.component = obj.get_component_name()

                c.arglist = []
                c.ser_import_list = []

                self._writeTmpl(c, self.__fp2[fname], "commandBodyVisit")
                self.__fp2[fname].close()
