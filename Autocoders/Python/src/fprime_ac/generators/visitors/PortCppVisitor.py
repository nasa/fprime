# ===============================================================================
# NAME: PortCppVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of each port
#              class source code file.
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
import logging
import sys

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
        finishPortCpp,
        includes1PortCpp,
        namespacePortCpp,
        privatePortCpp,
        publicPortCpp,
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
class PortCppVisitor(AbstractVisitor.AbstractVisitor):
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
        DEBUG.info("PortCppVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _get_args_proto_string(self, obj):
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
            elif TypesList.isPrimitiveType(t) or isEnum:
                t = t + " "
            else:
                t = "const " + t + " &"

            arg_str += "{}{}".format(t, arg.get_name())
            arg_str += ", "
        arg_str = arg_str.strip(", ")
        return arg_str

    def _get_args_string(self, obj):
        """
        Return a string of name args, comma separated
        for use in templates that generate method or function calls.
        """
        args = obj.get_args()
        arg_str = ""
        for arg in args:
            arg_str += "%s" % arg.get_name()
            arg_str += ", "
        arg_str = arg_str.strip(", ")
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
        DEBUG.debug("PortCppVisitor:%s" % visit_str)
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
            filename = obj.get_type() + self.__config.get("port", "PortCpp")
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
                filename = x[0].split(s[0])[0] + self.__config.get("port", "PortCpp")
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

    def includes1Visit(self, obj):
        """
        Defined to generate includes within a file.
        Usually used for the base classes but also for Port types
        @param args: the instance of the concrete element to operation on.
        """
        relative_path = self.relativePath()
        #
        DEBUG.debug("Relative path: %s", relative_path)
        #
        c = includes1PortCpp.includes1PortCpp()
        c.include_file = obj.get_xml_filename().replace("PortAi.xml", "PortAc.hpp")
        #
        # Added configurable override for includes for testing
        #
        if self.__config.get("includes", "port_include_path") == "None":
            if relative_path is not None:
                c.port_include_path = relative_path
            else:
                c.port_include_path = obj.get_namespace()
        else:
            c.port_include_path = self.__config.get("includes", "port_include_path")
        #
        self._writeTmpl(c, "includes1Visit")

    def includes2Visit(self, obj):
        """
        Defined to generate internal includes within a file.
        Usually used for data type includes and system includes.
        @param args: the instance of the concrete element to operation on.
        """

    def namespaceVisit(self, obj):
        """
        Defined to generate namespace code within a file.
        Also any pre-condition code is generated.
        @param args: the instance of the concrete element to operation on.
        """
        c = namespacePortCpp.namespacePortCpp()
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        c.name = obj.get_type()

        c.arg_list = self._get_args_list(obj)

        self._writeTmpl(c, "namespaceVisit")

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = publicPortCpp.publicPortCpp()
        c.name = obj.get_type()
        c.args_proto_string = self._get_args_proto_string(obj)
        c.args_string = self._get_args_string(obj)
        c.args = [a.get_name() for a in obj.get_args()]
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
        self._writeTmpl(c, "publicVisit")

    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """

    def _replace_enum(self, t):
        """
        For list of [(name, type),...] args get just the enum type.
        """
        t2 = []
        for i in t:
            if isinstance(i[1], tuple):
                if i[1][0][0] == "ENUM":
                    i2 = (i[0], i[1][0][1])
            else:
                i2 = i
            t2.append(i2)
        return t2

    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = privatePortCpp.privatePortCpp()
        c.name = obj.get_type()
        tmp = [(a.get_name(), a.get_type(), a.get_modifier()) for a in obj.get_args()]
        # Make an enum marker list here for template to use...
        c.enum_marker = []
        for i in tmp:
            if isinstance(i[1], tuple):
                if i[1][0][0].upper() == "ENUM":
                    c.enum_marker.append("ENUM")
                else:
                    c.enum_marker.append(None)
            else:
                c.enum_marker.append(None)
        c.enum_marker
        #
        # Make a pointer marker list here for template to use...
        c.pointer_marker = []
        for i in tmp:
            if i[2] == "pointer":
                c.pointer_marker.append(True)
            else:
                c.pointer_marker.append(False)
        c.pointer_marker
        tmp
        tmp = self._replace_enum(tmp)
        c.args = tmp
        c.args_string = self._get_args_string(obj)
        #
        c.ret_flag = obj.get_return()
        if c.ret_flag is not None:
            c.ret_flag = True
        else:
            c.ret_flag = False
        #
        self._writeTmpl(c, "privateVisit")

    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        c = finishPortCpp.finishPortCpp()
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        c.name = obj.get_type()
        c.args_string = self._get_args_string(obj)
        c.args_proto_string = self._get_args_proto_string(obj)
        c.args = [
            (a.get_name(), a.get_type(), a.get_modifier()) for a in obj.get_args()
        ]
        # Make an enum marker list here for template to use...
        c.enum_marker = []
        for i in c.args:
            if isinstance(i[1], tuple):
                if i[1][0][0].upper() == "ENUM":
                    c.enum_marker.append("ENUM")
                else:
                    c.enum_marker.append(None)
            else:
                c.enum_marker.append(None)
        #
        # Make a pointer marker list here for template to use...
        c.pointer_marker = []
        for i in c.args:
            if i[2] == "pointer":
                c.pointer_marker.append(True)
            else:
                c.pointer_marker.append(False)
        #
        r = obj.get_return()
        # Include optional return type here...
        if r is not None:
            c.ret_flag = True
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
            c.ret_flag = False
            return_type = "void"
            return_modifier = " "
        c.return_type = return_type + return_modifier
        #
        self._writeTmpl(c, "finishSourceFilesVisit")
        self.__fp.close()
