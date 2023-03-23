# ===============================================================================
# NAME: InstanceTopologyHVisitor.py
#
# DESCRIPTION: A visitor responsible for the generation of component
#              base class source code file.
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
from fprime_ac.models import ModelParser

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
    from fprime_ac.generators.templates.topology import (
        includes1TopologyH,
        publicInstanceTopologyH,
    )
except ImportError:
    print("ERROR: must generate python templates first.")
    sys.exit(-1)

# from fprime_ac.generators.templates import finishTopologyCpp
#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
# Module class or classes go here.
class InstanceTopologyHVisitor(AbstractVisitor.AbstractVisitor):
    """
    A visitor class responsible for generation of component header
    classes in C++.
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
        self.__form = formatters.Formatters()
        self.__form_comment = formatters.CommentFormatters()
        self.__model_parser = ModelParser.ModelParser.getInstance()
        DEBUG.info("InstanceTopologyHVisitor: Instanced.")
        self.bodytext = ""
        self.prototypetext = ""

    def _writeTmpl(self, c, visit_str):
        """
        Wrapper to write tmpl to files desc.
        """
        DEBUG.debug("InstanceTopologyHVisitor:%s" % visit_str)
        DEBUG.debug("===================================")
        DEBUG.debug(c)
        self.__fp.writelines(c.__str__())
        DEBUG.debug("===================================")

    def initFilesVisit(self, obj):
        """
        Defined to generate files for generated code products.
        @param obj: the instance of the component model to visit.
        """
        # Build filename here...
        if len(obj.get_comp_list()) > 0:
            xml_file = obj.get_comp_list()[0].get_xml_filename()
            x = xml_file.split(".")
            s = self.__config.get("assembly", "TopologyXML").split(".")
            l = len(s[0])
            #
            if (x[0][-l:] == s[0]) & (x[1] == s[1]):
                filename = x[0].split(s[0])[0] + self.__config.get(
                    "assembly", "TopologyH"
                )
                PRINT.info(
                    "Generating code filename: %s topology, using default XML filename prefix..."
                    % filename
                )
            else:
                msg = (
                    "XML file naming format not allowed (must be XXXAppAi.xml), Filename: %s"
                    % xml_file
                )
                PRINT.info(msg)
                raise ValueError(msg)
            #
            # Get the partN, partition label from XML file name if there is one.
            # For no partition it is simply None.  This is used only for ARINC653
            # demo to prepend partition prefix to instance names of components.
            #
            if xml_file.find("part") > 0:
                self.partition = "part" + xml_file.split("part")[1].split("AppAi")[0]
            else:
                self.partition = None
            #
            # Open file for writing here...
            DEBUG.info("Open file: %s" % filename)
            self.__fp = open(filename, "w")
            DEBUG.info("Completed")
        else:
            PRINT.info("ERROR: NO COMPONENTS FOUND IN TOPOLOGY XML FILE...")
            sys.exit(-1)

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
        c = includes1TopologyH.includes1TopologyH()
        temp = obj.get_comp_list()

        # Only generate port connections
        c.connect_only = False
        if obj.connect_only:
            c.connect_only = True
        # Generate Components as pointers
        c.is_ptr = False
        if obj.is_ptr:
            c.is_ptr = True
        else:
            if not obj.connect_only:
                c.is_ptr = True
                obj.is_ptr = True

        c.component_header_list = []
        for component in temp:
            #
            # Hack to fix the include file so it is consistent...
            if self.__config.get("component", "XMLDefaultFileName") == "False":
                namespace = ""
            else:
                namespace = component.get_namespace()
            #
            # Added configurable override for includes for testing
            if self.__config.get("includes", "comp_include_path") == "None":
                if relative_path is not None:
                    path = relative_path
                else:
                    path = component.get_namespace()
            else:
                path = self.__config.get("includes", "comp_include_path")
            c.path = path
            c.component_header_list.append((path, namespace, component.get_kind()))
        #
        # Build list of unique component types here...
        comp_types = [k[2] for k in c.component_header_list]
        comp_types = self.__model_parser.uniqueList(comp_types)
        comp_headers = c.component_header_list
        #
        # Recreate component_header_list here with only unique types...
        c.component_header_list = []
        for k in comp_types:
            for comp in comp_headers:
                if k == comp[2]:
                    c.component_header_list.append(comp)
                    break

        #
        ## Create Component Declarations
        component_list = []
        for component in temp:
            d = {
                "ns": component.get_namespace(),
                "name": component.get_name(),
                "kind": component.get_kind(),
            }
            component_list.append(dict(d))

        c.component_import_list = []
        for xml_name in obj.get_instance_header_dict():
            if obj.get_instance_header_dict()[xml_name] is not None:
                xml_path = obj.get_instance_header_dict()[xml_name]
            else:
                xml_path = xml_name
                xml_path = xml_path.strip()
                xml_path = xml_path.replace("i.xml", "")
                xml_path += "c.hpp"
            c.component_import_list.append(xml_path)

        c.component_declarations = []

        for component in component_list:
            if obj.is_ptr:
                declaration_template = """{ns}::{kind}Impl* {name}_ptr = 0;""".format(
                    **component
                )
            else:
                declaration_template = """{ns}::{kind}Impl {name}("{name}");""".format(
                    **component
                )
            c.component_declarations.append(declaration_template)

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

    def publicVisit(self, obj):
        """
        Defined to generate public stuff within a class.
        @param args: the instance of the concrete element to operation on.
        """
        c = publicInstanceTopologyH.publicInstanceTopologyH()
        # Added hack for ARINC demo...
        part = self.partition
        #
        component_list = []
        connection_list = []

        c.name = obj.get_name()
        c.kind = obj
        c.component_declarations = []
        c.component_inits = []
        c.port_connections = []
        c.component_startups = []
        c.component_teardowns = []
        c.command_registrations = []
        c.component_reference_ids = []

        # Only generate port connections
        c.connect_only = False
        if obj.connect_only:
            c.connect_only = True
        # Generate Components as pointers
        c.is_ptr = False
        if obj.is_ptr:
            c.is_ptr = True
        else:
            if not obj.connect_only:
                c.is_ptr = True
                obj.is_ptr = True

        for component in obj.get_comp_list():
            d = {
                "ns": component.get_namespace(),
                "name": component.get_name(),
                "kind": component.get_kind2(),
            }
            component_list.append(dict(d))

            for port in component.get_ports():
                d = {
                    "comment": port.get_comment(),
                    "comp": component.get_name(),
                    "name": port.get_name(),
                    "type": port.get_type(),
                    "direction": port.get_direction(),
                    "num": port.get_source_num(),
                    "tcomp": port.get_target_comp(),
                    "tname": port.get_target_port(),
                    "ttype": port.get_target_type(),
                    "tdirection": port.get_target_direction(),
                    "tnum": port.get_target_num(),
                    "modeler": component.get_modeler(),
                }
                connection_list.append(dict(d))

        #
        # Generate Component Declarations
        for component in component_list:
            # Partition instance names
            if part is None:
                pass
            else:
                component["name"] = part + "_" + component["name"]
            #

            if obj.is_ptr:
                declaration_template = (
                    """{name}_ptr = new {ns}::{ns}Impl("{name}");""".format(**component)
                )
                c.component_declarations.append(declaration_template)
            else:
                pass  ## If objects are generated as instances the object was instantiated in includes
        #

        #
        # Generate Set Window/Base ID Method
        for id_tuple in obj.get_base_id_list():
            n = id_tuple[0]
            base_id = id_tuple[1]

            if obj.is_ptr:
                declaration_template = """{}_ptr->setIdBase({});""".format(n, base_id)
            else:
                declaration_template = """{}.setIdBase({});""".format(n, base_id)

            c.component_reference_ids.append(declaration_template)
        #

        #
        # Generate Component Initializations
        for component in component_list:
            if obj.is_ptr:
                init_template = """{name}_ptr->init(10);""".format(**component)
            else:
                init_template = """{name}.init(10);""".format(**component)
            c.component_inits.append(init_template)
        #

        # Generate Port Connections
        for connection in connection_list:
            if connection["type"] == "Serial":
                connection["type"] = "Serialize"
            if connection["ttype"] == "Serialize":
                connection["ttype"] = "Serialize"

            comment = "//{comment}".format(**connection)
            if obj.is_ptr:
                connection = """{comp}_ptr->set_{name}_OutputPort({num}, {tcomp}_ptr->get_{tname}_InputPort({tnum}));""".format(
                    **connection
                )
                connection_template = (comment, connection)
            else:
                connection = """{comp}.set_{name}_OutputPort({num}, {tcomp}.get_{tname}_InputPort({tnum}));""".format(
                    **connection
                )
                connection_template = (comment, connection)
            c.port_connections.append(connection_template)

        #
        # Generate Component Command Registration
        for connection in connection_list:
            if connection["type"] == "CmdReg" and connection["direction"] == "output":
                if obj.is_ptr:
                    registration = """{comp}_ptr->regCommands();""".format(**connection)
                else:
                    registration = """{comp}.regCommands();""".format(**connection)
                c.command_registrations.append(registration)
            # c.command_registrations.append(connection['comp']+" "+str(connection['type'])+" "+str(connection['direction']))

        #
        # Generate Component Startup
        for component in component_list:
            startup_template = ""
            if component["kind"] == "active":
                if obj.is_ptr:
                    startup_template = """{name}_ptr->start();""".format(**component)
                else:
                    startup_template = """{name}.start();""".format(**component)
                c.component_startups.append(startup_template)
        #

        #
        # Generate Component Teardown
        for component in component_list:
            teardown_template = ""
            if component["kind"] == "active":
                if obj.is_ptr:
                    teardown_template = """{name}_ptr->exit();""".format(**component)
                else:
                    teardown_template = """{name}.exit();""".format(**component)
                c.component_teardowns.append(teardown_template)
        #

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
        # c = finishComponentCpp.finishComponentCpp()
        # self._writeTmpl(c, "finishSourceFilesVisit")
        self.__fp.close()
