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
    from fprime_ac.generators.templates.events import EventBody, EventHeader
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
class InstanceEventVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of  Event Python classes.
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
        DEBUG.info("InstanceEventVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _writeTmpl(self, c, fp, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug("InstanceEventVisitor:%s" % visit_str)
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def DictStartVisit(self, obj, topology_model):
        """
        Defined to generate files for generated code products.
        @param obj: the instance of the event model to visit.
        """

        # Build filename here...
        # Make dictionary directly if it doesn't exist
        output_dir = os.environ["DICT_DIR"] + "/events"
        if not (os.path.isdir(output_dir)):
            os.makedirs(output_dir)
            init_file = os.path.join(output_dir, "__init__.py")
            open(init_file, "w+").close()

        self.__fp = {}

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

        for instance_obj in instance_obj_list:
            if instance_obj[3].get_dict_short_name() is not None:
                fname = "{}_{}".format(
                    instance_obj[3].get_dict_short_name(), obj.get_name()
                )
            elif (
                not topology_model.get_prepend_instance_name()
                and len(instance_obj_list) == 1
            ):
                fname = obj.get_name()
            else:
                fname = "{}_{}".format(instance_obj[0], obj.get_name())

            pyfile = "{}/{}.py".format(output_dir, fname)
            DEBUG.info("Open file: {}".format(pyfile))
            fd = open(pyfile, "w")
            DEBUG.info("Completed {} open".format(pyfile))
            self.__fp[fname] = fd

    def DictHeaderVisit(self, obj, topology_model):
        """
        Defined to generate header for  event python class.
        @param obj: the instance of the event model to operation on.
        """

        for fname in list(self.__fp.keys()):
            c = EventHeader.EventHeader()
            d = datetime.datetime.now()
            c.date = d.strftime("%A, %d %B %Y")
            c.user = getuser()
            c.source = obj.get_xml_filename()
            self._writeTmpl(c, self.__fp[fname], "eventHeaderVisit")

    def DictBodyVisit(self, obj, topology_model):
        """
        Defined to generate the body of the  Python event class
        @param obj: the instance of the event model to operation on.
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

        for instance_obj in instance_obj_list:
            c = EventBody.EventBody()
            if instance_obj[3].get_dict_short_name() is not None:
                fname = "{}_{}".format(
                    instance_obj[3].get_dict_short_name(), obj.get_name()
                )
            elif (
                not topology_model.get_prepend_instance_name()
                and len(instance_obj_list) == 1
            ):
                fname = obj.get_name()
            else:
                fname = "{}_{}".format(instance_obj[0], obj.get_name())
            c.name = fname

            if len(obj.get_ids()) > 1:
                raise ValueError(
                    "There is more than one event id when creating dictionaries. Check xml of {} or see if multiple explicit IDs exist in the AcConstants.ini file".format(
                        fname
                    )
                )
            try:
                c.id = hex(instance_obj[1] + int(float(obj.get_ids()[0])))
            except (TypeError, ValueError):
                c.id = hex(instance_obj[1] + int(obj.get_ids()[0], 16))

            c.severity = obj.get_severity()
            c.format_string = obj.get_format_string()
            c.description = obj.get_comment()
            c.component = obj.get_component_name()

            c.arglist = []
            c.ser_import_list = []
            for arg_num, arg_obj in enumerate(obj.get_args()):
                n = arg_obj.get_name()
                t = arg_obj.get_type()
                s = arg_obj.get_size()
                d = arg_obj.get_comment()
                # convert XML types to Python classes
                (
                    type_string,
                    ser_import,
                    type_name,
                    dontcare,
                ) = DictTypeConverter.DictTypeConverter().convert(t, s)
                if ser_import is not None:
                    c.ser_import_list.append(ser_import)
                # convert format specifier if necessary
                if type_name == "enum":
                    format_string = (
                        DictTypeConverter.DictTypeConverter().format_replace(
                            c.format_string, arg_num, "d", "s"
                        )
                    )
                    # check for an error
                    if format_string is None:
                        PRINT.info(
                            "Event %s in component %s had error processing format specifier"
                            % (c.name, c.component)
                        )
                        sys.exit(-1)
                    else:
                        c.format_string = format_string

                c.arglist.append((n, d, type_string))
            self._writeTmpl(c, self.__fp[fname], "eventBodyVisit")
            self.__fp[fname].close()
