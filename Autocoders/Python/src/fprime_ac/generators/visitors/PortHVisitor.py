# ===============================================================================
# NAME: PortHVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of header file
#              for each port class.
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
from fprime_ac.utils import ConfigManager, TypesList

#
# Import precompiled templates here
#
try:
    from fprime_ac.generators.templates.port import (
        finishPortH,
        includes1PortH,
        includes2PortH,
        namespacePortH,
        privatePortH,
        protectedPortH,
        publicPortH,
        startPortH,
    )
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
class PortHVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of component header
    classes in C++.
    """

    __instance = None
    __config = None
    __fp = None
    __form = None
    __form_comment = None

    def __init__(self):
        """
        Constructor.
        """
        super().__init__()
        self.__config = ConfigManager.ConfigManager.getInstance()
        self.__form = formatters.Formatters()
        self.__form_comment = formatters.CommentFormatters()
        DEBUG.info("PortHVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _get_args_string(self, obj):
        """
        Return a string of (type, name) args, comma separated
        for use in templates that generate prototypes.
        """
        args = obj.get_args()
        arg_str = ""
        for arg in args:
            t = arg.get_type()
            isEnum = False
            #
            # Grab enum type here...
            if isinstance(t, tuple):
                if t[0][0].upper() == "ENUM":
                    t = t[0][1]
                    isEnum = True
                else:
                    PRINT.info(
                        "ERROR: Ill formed enumeration type...(name: %s, type: %s"
                        % (arg.get_name(), arg.get_type())
                    )
                    sys.exit(-1)
            else:
                t = arg.get_type()

            if t == "string":
                t = arg.get_name() + "String"
            if t == "buffer":
                t = arg.get_name() + "Buffer"
            #
            # Add modifier here - if any...
            if arg.get_modifier() == "pointer":
                t = t + " *"
            elif arg.get_modifier() == "reference":
                t = t + " &"
            elif arg.get_modifier() == "value":
                t = t + " "
            elif TypesList.isPrimitiveType(t) or isEnum:
                t = t + " "
            else:
                t = "const " + t + " &"

            arg_str += "{}{}".format(t, arg.get_name())
            arg_str += ", "
        arg_str = arg_str.strip(", ")
        return arg_str

    def _get_args_sum_string(self, obj):
        """
        Return a string of sizeof calls that sum to the port
        buffer size for use within the input ports.
        """
        args = obj.get_args()
        arg_str = ""
        # empty list then void args
        if len(args) == 0:
            arg_str = "0"
            return arg_str
        for arg in args:
            t = arg.get_type()

            if t == "string":
                t = arg.get_name() + "String"
            if t == "buffer":
                t = arg.get_name() + "Buffer"

            #
            # Make size for pointer modifier here...
            if arg.get_modifier() == "pointer":
                cl = " *)"
            else:
                cl = ")"
            #
            if isinstance(t, tuple):
                if t[0][0].upper() == "ENUM":
                    t = "sizeof(NATIVE_INT_TYPE)"
                else:
                    PRINT.info(
                        "ERROR: Ill formed enumeration type...(name: %s, type: %s"
                        % (arg.get_name(), arg.get_type())
                    )
                    sys.exit(-1)
            elif t in [
                "U8",
                "U16",
                "U32",
                "U64",
                "I8",
                "I16",
                "I32",
                "I64",
                "F32",
                "F64",
                "bool",
                "FwBuffSizeType",
                "FwChanIdType",
                "FwEnumStoreType",
                "FwEventIdType",
                "FwIndexType",
                "FwOpcodeType",
                "FwPacketDescriptorType",
                "FwPrmIdType",
                "FwSizeType",
                "FwTimeBaseStoreType",
                "FwTimeContextStoreType",
                "FwTlmPacketizeIdType",
                "NATIVE_INT_TYPE",
                "NATIVE_UINT_TYPE",
                "POINTER_CAST",
            ]:
                t = "sizeof(" + t + cl
            else:
                if arg.get_modifier() == "pointer":
                    t = "sizeof(" + t + "*)"
                else:
                    t = t + "::SERIALIZED_SIZE"
            arg_str += t
            arg_str += " + "
        arg_str = arg_str.strip(" + ")
        return arg_str

    def _get_args_list(self, obj):
        """
        Return a list of port argument tuples
        """
        arg_list = []

        for arg in obj.get_args():
            n = arg.get_name()
            t = arg.get_type()
            m = arg.get_modifier()
            s = arg.get_size()
            c = arg.get_comment()
            arg_list.append((n, t, m, s, c))

        return arg_list

    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug("PortHVisitor:%s" % visit_str)
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        self.__fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def initFilesVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @param args: the instance of the concrete element to operation on.
        """
        # Build filename here...
        if self.__config.get("port", "XMLDefaultFileName") == "True":
            filename = obj.get_type() + self.__config.get("port", "PortH")
            PRINT.info(
                f"Generating code filename: {filename}, using XML namespace and name attributes..."
            )
        else:
            xml_file = obj.get_xml_filename()
            x = xml_file.split(".")
            s = self.__config.get("port", "PortXML").split(".")
            l = len(s[0])
            #
            if (x[0][-l:] == s[0]) & (x[1] == s[1]):
                filename = x[0].split(s[0])[0] + self.__config.get("port", "PortH")
                PRINT.info(
                    "Generating code filename: %s, using default XML filename prefix..."
                    % filename
                )
            else:
                msg = (
                    "XML file naming format not allowed (must be XXXPortAi.xml), Filename: %s"
                    % xml_file
                )
                PRINT.info(msg)
                raise ValueError(msg)

        # Open file for writing here...
        DEBUG.info("Open file: %s" % filename)
        self.__fp = open(filename, "w")
        DEBUG.info("Completed")

    def startSourceFilesVisit(self, obj):
        """
        Defined to generate starting static code within files.
        """
        c = startPortH.startPortH()
        c.name = obj.get_type
        c.name_space = obj.get_namespace()
        d = datetime.datetime.now()
        c.date = d.strftime("%A, %d %B %Y")
        c.user = getuser()
        self._writeTmpl(c, "startSourceFilesVisit")

    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Port types
        @param args: the instance of the concrete element to operation on.
        """
        c = includes1PortH.includes1PortH()
        self._writeTmpl(c, "includes1Visit")

    def includes2Visit(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @param args: the instance of the concrete element to operation on.
        """
        c = includes2PortH.includes2PortH()
        c.c_includes_list = obj.get_includes()
        if False in [x[-3:] == "hpp" or x[-1:] == "h" for x in c.c_includes_list]:
            PRINT.info(
                "ERROR: Only .hpp or .h files can be given within <include_header> tag!!!"
            )
            sys.exit(-1)
        c.xml_includes_list = obj.get_serial_includes() + obj.get_enum_includes()
        if False in [x[-6:] == "Ai.xml" for x in c.xml_includes_list]:
            PRINT.info(
                "ERROR: Only xml files can be given within <import_serializable_type> tag!!!"
            )
            sys.exit(-1)
        c.xml_includes_list = [
            x.replace("Ai.xml", "Ac.hpp") for x in c.xml_includes_list
        ]
        self._writeTmpl(c, "includes2Visit")

    def _get_enum_string_list(self, enum_list):
        """"""
        enum_tuple = enum_list[0]
        enum_list = enum_list[1]
        enum_str_list = []
        for e in enum_list:
            # No value, No comment
            if (e[1] is None) and (e[2] is None):
                s = "%s," % (e[0])
            # No value, With comment
            elif (e[1] is None) and (e[2] is not None):
                s = "{},  // {}".format(e[0], e[2])
            # With value, No comment
            elif (e[1] is not None) and (e[2] is None):
                s = "{} = {},".format(e[0], e[1])
            # With value and comment
            elif (e[1] is not None) and (e[2] is not None):
                s = "%s = %s,  // %s" % (e)
            else:
                pass
            enum_str_list.append(s)

        return (enum_tuple, enum_str_list)

    def namespaceVisit(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @param args: the instance of the concrete element to operation on.
        """
        c = namespacePortH.namespacePortH()
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        c.name = obj.get_type()
        c.desc = obj.get_comment()
        #
        # Get any enum types here for the option generation of typedef's
        # enum_type_list is enum types of form
        # [(('ENUM', name of typedef), [ list of (name, value, comment) enumerations]), (...), (...), ...]
        #
        c.enum_type_list = []
        t = [x.get_type() for x in obj.get_args()]
        # if a return type add it to the list to scan for enum
        r = obj.get_return()
        if r is not None:
            t += obj.get_return()
        enum_list = [x for x in t if type(x) == tuple]
        for e in enum_list:
            c.enum_type_list.append(self._get_enum_string_list(e))
        # print c.enum_type_list

        c.arg_list = self._get_args_list(obj)

        self._writeTmpl(c, "namespaceVisit")

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = publicPortH.publicPortH()
        c.name = obj.get_type()
        c.args_string = self._get_args_string(obj)
        c.args_sum_str = self._get_args_sum_string(obj)
        r = obj.get_return()
        # Include optional return type here...
        if r is not None:
            return_type = r[0]
            if isinstance(return_type, tuple):
                return_type = return_type[0][1]
            if r[1] == "pointer":
                return_modifier = " *"
            elif r[1] == "reference":
                return_modifier = " &"
            else:
                return_modifier = " "
        else:
            return_type = "void"
            return_modifier = " "
        c.return_type = return_type + return_modifier
        #
        self._writeTmpl(c, "publicVisit")

    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = protectedPortH.protectedPortH()
        self._writeTmpl(c, "protectedVisit")

    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = privatePortH.privatePortH()
        c.name = obj.get_type()
        self._writeTmpl(c, "privateVisit")

    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        c = finishPortH.finishPortH()
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        c.name = obj.get_type()
        c.name_space = obj.get_namespace()
        c.desc = obj.get_comment()
        c.args_string = self._get_args_string(obj)
        r = obj.get_return()
        # Include optional return type here...
        if r is not None:
            return_type = r[0]
            if isinstance(return_type, tuple):
                return_type = return_type[0][1]
            if r[1] == "pointer":
                return_modifier = " *"
            elif r[1] == "reference":
                return_modifier = " &"
            else:
                return_modifier = " "
        else:
            return_type = "void"
            return_modifier = " "
        c.return_type = return_type + return_modifier
        self._writeTmpl(c, "finishSourceFilesVisit")
        self.__fp.close()
