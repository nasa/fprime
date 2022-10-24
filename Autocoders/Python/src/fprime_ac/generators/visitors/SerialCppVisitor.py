# ===============================================================================
# NAME: SerialCppVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of each serializable
#              class source code file.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : June 4, 2013
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
from fprime_ac.utils import ConfigManager

#
# Import precompiled templates here
#
try:
    # from fprime_ac.generators.templates import privateSerialCpp
    from fprime_ac.generators.templates.serialize import (
        finishSerialCpp,
        includes1SerialCpp,
        namespaceSerialCpp,
        publicSerialCpp,
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

typelist = ["U8", "I8", "U16", "I16", "U32", "I32", "U64", "I64", "F32", "F64", "bool"]

#
# Module class or classes go here.
class SerialCppVisitor(AbstractVisitor.AbstractVisitor):
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
        DEBUG.info("SerialCppVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _get_args_proto_string(self, obj):
        """
        Return a string of (type, name) args, comma separated
        for use in templates that generate prototypes.
        """
        arg_str = ""
        for (
            name,
            mtype,
            array_size,
            size,
            format,
            comment,
            default,
        ) in obj.get_members():
            if isinstance(mtype, tuple):
                arg_str += "{} {}, ".format(mtype[0][1], name)
            elif mtype == "string" and array_size is None:
                arg_str += "const {}::{}String& {}, ".format(obj.get_name(), name, name)
            elif mtype == "string" and array_size is not None:
                arg_str += "const {}::{}String* {}, ".format(obj.get_name(), name, name)
                arg_str += "NATIVE_INT_TYPE %sSize, " % (name)
            elif mtype not in typelist:
                arg_str += "const {}& {}, ".format(mtype, name)
            elif array_size is not None:
                arg_str += "const {}* {}, ".format(mtype, name)
                arg_str += "NATIVE_INT_TYPE %sSize, " % (name)
            else:
                arg_str += "{} {}".format(mtype, name)
                arg_str += ", "

        arg_str = arg_str.strip(", ")
        return arg_str

    def _get_args_string(self, obj, prefix=""):
        """
        Return a string of name args, comma separated
        for use in templates that generate method or function calls.
        """
        args = obj.get_members()
        arg_str = ""
        for arg in args:
            if arg[2] is not None:
                arg_str += prefix + "%s" % arg[0]
                arg_str += ", "
                arg_str += "%s" % arg[2]
                arg_str += ", "
            else:
                arg_str += prefix + "%s" % arg[0]
                arg_str += ", "

        arg_str = arg_str.strip(", ")
        return arg_str

    def _get_conv_mem_list(self, obj):
        """
        Return a list of struct member tuples
        """
        arg_list = []

        for (
            name,
            mtype,
            array_size,
            size,
            format,
            comment,
            default,
        ) in obj.get_members():
            typeinfo = None
            if isinstance(mtype, tuple):
                mtype = mtype[0][1]
                typeinfo = "enum"
            elif mtype == "string":
                mtype = "{}::{}String".format(obj.get_name(), name)
                typeinfo = "string"
            elif mtype not in typelist:
                typeinfo = "extern"

            arg_list.append(
                (name, mtype, array_size, size, format, comment, default, typeinfo)
            )
        return arg_list

    def _get_args_proto_string_scalar_init(self, obj):
        """
        Return a string of (type, name) args, comma separated
        for use in templates that generate prototypes where the array
        arguments are represented by single element values. If no arguments
        are arrays, function returns None.
        """
        arg_str = ""
        contains_array = False
        for (
            name,
            mtype,
            array_size,
            size,
            format,
            comment,
            default,
        ) in obj.get_members():
            if isinstance(mtype, tuple):
                arg_str += "{} {}, ".format(mtype[0][1], name)
            elif mtype == "string":
                arg_str += "const {}::{}String& {}, ".format(obj.get_name(), name, name)
            elif mtype not in typelist:
                arg_str += "const {}& {}, ".format(mtype, name)
            elif array_size is not None:
                arg_str += "const {} {}, ".format(mtype, name)
                contains_array = True
            else:
                arg_str += "{} {}".format(mtype, name)
                arg_str += ", "
        if not contains_array:
            return None
        arg_str = arg_str.strip(", ")
        return arg_str

    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug("SerialCppVisitor:%s" % visit_str)
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
        if self.__config.get("serialize", "XMLDefaultFileName") == "True":
            namespace = "".join(obj.get_namespace().split("::"))
            filename = (
                namespace
                + obj.get_name()
                + self.__config.get("serialize", "SerializableCpp")
            )
            PRINT.info(
                f"Generating code filename: {filename}, using XML namespace and name attributes..."
            )
        else:
            xml_file = obj.get_xml_filename()
            x = xml_file.split(".")
            s = self.__config.get("serialize", "SerializableXML").split(".")
            l = len(s[0])
            #
            if (x[0][-l:] == s[0]) & (x[1] == s[1]):
                filename = x[0].split(s[0])[0] + self.__config.get(
                    "serialize", "SerializableCpp"
                )
                PRINT.info(
                    "Generating code filename: %s, using default XML filename prefix..."
                    % filename
                )
            else:
                msg = (
                    "XML file naming format not allowed (must be XXXSerializableAi.xml), Filename: %s"
                    % xml_file
                )
                PRINT.info(msg)
                sys.exit(-1)

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
        Usually used for the base classes but also for Serial types
        @param args: the instance of the concrete element to operation on.
        """
        relative_path = self.relativePath()
        #
        DEBUG.debug("Relative path: %s", relative_path)
        #
        c = includes1SerialCpp.includes1SerialCpp()
        c.name = obj.get_name()
        #
        # Hack to fix the include file so it is consistent...
        if self.__config.get("serialize", "XMLDefaultFileName") == "False":
            c.name = obj.get_xml_filename().split("SerializableAi.xml")[0]
            c.namespace = ""
        #
        # Added configurable override for includes for testing
        #
        if self.__config.get("includes", "serial_include_path") == "None":
            if relative_path is not None:
                c.include_path = relative_path
            else:
                c.include_path = obj.get_namespace()
        else:
            c.include_path = self.__config.get("includes", "serial_include_path")
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
        c = namespaceSerialCpp.namespaceSerialCpp()
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        c.name = obj.get_name()
        c.mem_list = obj.get_members()

        self._writeTmpl(c, "namespaceVisit")

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = publicSerialCpp.publicSerialCpp()
        c.name = obj.get_name()
        c.args_proto_string = self._get_args_proto_string(obj)
        c.args_string = self._get_args_string(obj)
        c.args_mstring = self._get_args_string(obj, "src.m_")
        c.args_mstring_ptr = self._get_args_string(obj, "src->m_")
        c.args_scalar_array_string = self._get_args_proto_string_scalar_init(obj)
        c.members = self._get_conv_mem_list(obj)
        self._writeTmpl(c, "publicVisit")

    def protectedVisit(self, obj):
        """
        Defined to generate protected stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """

    def privateVisit(self, obj):
        """
        Defined to generate private stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """

    def finishSourceFilesVisit(self, obj):
        """
        Defined to generate ending static code within files.
        """
        c = finishSerialCpp.finishSerialCpp()
        if obj.get_namespace() is None:
            c.namespace_list = None
        else:
            c.namespace_list = obj.get_namespace().split("::")
        self._writeTmpl(c, "finishSourceFilesVisit")
        self.__fp.close()
