#!/usr/bin/env python3
# ===============================================================================
# NAME: AbstractVisitor.py
#
# DESCRIPTION: This is a factory class for instancing the interface
#              and connecting the appropriate visitors to produce code.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb. 5 2013
#
# Copyright 2013, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#

import logging
import socket

from fprime_ac.utils import Logger

from . import (
    DictBody,
    DictHeader,
    DictStart,
    FinishSource,
    HtmlDocPage,
    HtmlStartPage,
    Includes1,
    Includes2,
    InitFiles,
    InstanceDictBody,
    InstanceDictHeader,
    InstanceDictStart,
    MdDocPage,
    MdStartPage,
    Namespace,
    Private,
    Protected,
    Public,
    StartSource,
)
from .visitors import (
    ChannelVisitor,
    CommandVisitor,
    ComponentCppVisitor,
    ComponentHVisitor,
    ComponentTestCppVisitor,
    ComponentTestHVisitor,
    EventVisitor,
    GTestCppVisitor,
    GTestHVisitor,
    HtmlDocVisitor,
    ImplCppVisitor,
    ImplHVisitor,
    InstanceChannelVisitor,
    InstanceCommandVisitor,
    InstanceEventVisitor,
    InstanceSerializableVisitor,
    InstanceTopologyChannelsHTMLVisitor,
    InstanceTopologyCmdHTMLVisitor,
    InstanceTopologyCppVisitor,
    InstanceTopologyEventsHTMLVisitor,
    InstanceTopologyHVisitor,
    MdDocVisitor,
    PortCppVisitor,
    PortHVisitor,
    SerialCppVisitor,
    SerialHVisitor,
    SerializableVisitor,
    TestImplCppVisitor,
    TestImplCppHelpersVisitor,
    TestImplHVisitor,
    TestMainVisitor,
    TopologyCppVisitor,
    TopologyHVisitor,
    TopologyIDVisitor,
)

# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
HOST = socket.gethostname()


class GenFactory:
    """
    This is a concrete factory method object used to create the interface
    to the code generation.  A single call to create is made that returns
    a list of instances that are the elements that generate the code based
    on visitors attached here.
    """

    __instance = None
    __configured_visitors = None
    __visitor_list = None

    class VisitorConfig:
        """
        The up front visitor configuration options.
        """

        __enabled = None
        __generate_code = None
        __type = None

        def __init__(self, the_type, enabled, generate_code):
            """
            Set the configuration options for a visitor.
            """
            self.__type = the_type
            self.setEnabled(enabled)
            self.setGenerateCode(generate_code)

        def getEnabled(self):
            """"""
            return self.__enabled

        def getGenerateCode(self):
            """"""
            return self.__generate_code

        def setEnabled(self, enabled):
            """"""
            if enabled not in (True, False):
                raise AttributeError("GenFactory: bad is enabled option")
            self.__enabled = enabled

        def setGenerateCode(self, generate_code):
            """"""
            if generate_code not in (True, False):
                raise AttributeError("GenFactory: bad generate code option")
            self.__generate_code = generate_code

        def Instance(self):
            """
            Returns a NEW instance of type specified by type string.
            """
            if self.__type == "ComponentCppVisitor":
                inst = ComponentCppVisitor.ComponentCppVisitor()
            elif self.__type == "ComponentHVisitor":
                inst = ComponentHVisitor.ComponentHVisitor()
            elif self.__type == "ImplCppVisitor":
                inst = ImplCppVisitor.ImplCppVisitor()
            elif self.__type == "ImplHVisitor":
                inst = ImplHVisitor.ImplHVisitor()
            elif self.__type == "TestImplCppVisitor":
                inst = TestImplCppVisitor.TestImplCppVisitor()
            elif self.__type == "TestImplCppHelpersVisitor":
                inst = TestImplCppHelpersVisitor.TestImplCppHelpersVisitor()
            elif self.__type == "TestImplHVisitor":
                inst = TestImplHVisitor.TestImplHVisitor()
            elif self.__type == "TestMainVisitor":
                inst = TestMainVisitor.TestMainVisitor()
            elif self.__type == "PortCppVisitor":
                inst = PortCppVisitor.PortCppVisitor()
            elif self.__type == "PortHVisitor":
                inst = PortHVisitor.PortHVisitor()
            elif self.__type == "SerialCppVisitor":
                inst = SerialCppVisitor.SerialCppVisitor()
            elif self.__type == "SerialHVisitor":
                inst = SerialHVisitor.SerialHVisitor()
            elif self.__type == "TopologyCppVisitor":
                inst = TopologyCppVisitor.TopologyCppVisitor()
            elif self.__type == "TopologyHVisitor":
                inst = TopologyHVisitor.TopologyHVisitor()
            elif self.__type == "InstanceTopologyCppVisitor":
                inst = InstanceTopologyCppVisitor.InstanceTopologyCppVisitor()
            elif self.__type == "InstanceTopologyHVisitor":
                inst = InstanceTopologyHVisitor.InstanceTopologyHVisitor()
            elif self.__type == "InstanceTopologyCmdHTMLVisitor":
                inst = InstanceTopologyCmdHTMLVisitor.InstanceTopologyCmdHTMLVisitor()
            elif self.__type == "InstanceTopologyChannelsTMLVisitor":
                inst = (
                    InstanceTopologyChannelsHTMLVisitor.InstanceTopologyChannelsHTMLVisitor()
                )
            elif self.__type == "InstanceTopologyEventsHTMLVisitor":
                inst = (
                    InstanceTopologyEventsHTMLVisitor.InstanceTopologyEventsHTMLVisitor()
                )
            elif self.__type == "ComponentTestHVisitor":
                inst = ComponentTestHVisitor.ComponentTestHVisitor()
            elif self.__type == "ComponentTestCppVisitor":
                inst = ComponentTestCppVisitor.ComponentTestCppVisitor()
            elif self.__type == "GTestHVisitor":
                inst = GTestHVisitor.GTestHVisitor()
            elif self.__type == "GTestCppVisitor":
                inst = GTestCppVisitor.GTestCppVisitor()
            elif self.__type == "CommandVisitor":
                inst = CommandVisitor.CommandVisitor()
            elif self.__type == "EventVisitor":
                inst = EventVisitor.EventVisitor()
            elif self.__type == "ChannelVisitor":
                inst = ChannelVisitor.ChannelVisitor()
            elif self.__type == "SerializableVisitor":
                inst = SerializableVisitor.SerializableVisitor()
            elif self.__type == "InstanceCommandVisitor":
                inst = InstanceCommandVisitor.InstanceCommandVisitor()
            elif self.__type == "InstanceEventVisitor":
                inst = InstanceEventVisitor.InstanceEventVisitor()
            elif self.__type == "InstanceChannelVisitor":
                inst = InstanceChannelVisitor.InstanceChannelVisitor()
            elif self.__type == "InstanceSerializableVisitor":
                inst = InstanceSerializableVisitor.InstanceSerializableVisitor()
            elif self.__type == "HtmlDocVisitor":
                inst = HtmlDocVisitor.HtmlDocVisitor()
            elif self.__type == "MdDocVisitor":
                inst = MdDocVisitor.MdDocVisitor()
            elif self.__type == "TopologyIDVisitor":
                inst = TopologyIDVisitor.TopologyIDVisitor()
            else:
                s = f"VisitorConfig.getInstance: unsupported visitor type ({self.__type})"
                PRINT.info(s)
                raise ValueError(s)
            return inst

    def __init__(self):
        """
        Private Constructor (singleton pattern)
        """
        self.__instance = None
        self.__configured_visitors = {}

    def getInstance():
        """
        Return instance of singleton.
        """
        # TKC - Removed singleton since component and command instances were clashing.
        #         if(GenFactory.__instance is None) :
        #             GenFactory.__instance = GenFactory()
        #
        #         return GenFactory.__instance
        return GenFactory()

    # define static method
    getInstance = staticmethod(getInstance)

    def create(self, the_type):
        """
        Create and return a code generator that generates a specific portion
        of code for all the visitor producers. Each of the producer visitors
        provides an interface to generate a portion of the code. The following
        portions are supported -- referenced by a string name.

        initFiles
        startSource
        includes1
        includes2
        namespace
        public
        protected
        private
        finishSource
        StartCommand
        CommandHeader

        The visitors correspond to source code files that are generated and
        kept here as a hardwired list of classes. The project parameter can
        be used to hardwire a different set of code generators.
        """

        # Get the list of code producing visitors. The hardwired list is
        # based on the project argument. A different set of visitors can
        # be assembled by project.

        project_visitor_list = self._buildVisitorList()

        # Instance the needed code snippet generator here.

        if the_type == "initFiles":
            code_section_generator = InitFiles.InitFiles()
        elif the_type == "startSource":
            code_section_generator = StartSource.StartSource()
        elif the_type == "includes1":
            code_section_generator = Includes1.Includes1()
        elif the_type == "includes2":
            code_section_generator = Includes2.Includes2()
        elif the_type == "namespace":
            code_section_generator = Namespace.Namespace()
        elif the_type == "public":
            code_section_generator = Public.Public()
        elif the_type == "protected":
            code_section_generator = Protected.Protected()
        elif the_type == "private":
            code_section_generator = Private.Private()
        elif the_type == "finishSource":
            code_section_generator = FinishSource.FinishSource()

        elif the_type == "DictStart":
            code_section_generator = DictStart.DictStart()
        elif the_type == "DictHeader":
            code_section_generator = DictHeader.DictHeader()
        elif the_type == "DictBody":
            code_section_generator = DictBody.DictBody()

        elif the_type == "InstanceDictStart":
            code_section_generator = InstanceDictStart.InstanceDictStart()
        elif the_type == "InstanceDictHeader":
            code_section_generator = InstanceDictHeader.InstanceDictHeader()
        elif the_type == "InstanceDictBody":
            code_section_generator = InstanceDictBody.InstanceDictBody()

        elif the_type == "HtmlStart":
            code_section_generator = HtmlStartPage.HtmlStartPage()
        elif the_type == "HtmlDoc":
            code_section_generator = HtmlDocPage.HtmlDocPage()

        elif the_type == "MdStart":
            code_section_generator = MdStartPage.MdStartPage()
        elif the_type == "MdDoc":
            code_section_generator = MdDocPage.MdDocPage()

        else:
            print(f"GenFactory: unsupported code section ({the_type}).")
            return None

        self._addVisitor(code_section_generator, project_visitor_list)

        return code_section_generator

    def configureVisitor(self, instance, the_type, enable=True, gencode=True):
        """
        Call this method to configure the specified visitor. This must be
        called before the factory create method. These configuration options
        are saved and applied when the code producers are created. Each
        producer is referenced by its string instance name and individual
        instances are created utilizing a string type name.  The type names are:

            ComponentCppVisitor
            ComponentHVisitor
            PortCppVisitor
            PortHVisitor

        Note: The configuration for all producers must be set before the
        generator create method is called. Otherwise, the entire producer
        is disabled and no code will be produced from that generator.

        Note: instance names must be unique or the old instance
        will be overwritten and not exist in the generated code.
        """
        if instance not in self.__configured_visitors:
            config = GenFactory.VisitorConfig(the_type, enable, gencode)
            self.__configured_visitors[instance] = config
        else:
            self.__configured_visitors[instance].setEnabled(enable)
            self.__configured_visitors[instance].setGenerateCode(gencode)

    def _buildVisitorList(self):
        """
        Build the visitor list. This is where the
        visitors (code producers) are instanced.
        """
        # Instance this list only once for all elements
        if self.__visitor_list is None:
            self.__visitor_list = []
            #
            # Insert each of the code producer visitors into the list of
            # visitors. The visitor must be in the list of
            # configured visitors before it is inserted. Otherwise, it is
            # considered disabled, and not placed in the list.
            #
            for inst in self.__configured_visitors:
                config = self.__configured_visitors[inst]
                # If enabled instance it and place in list
                if config.getEnabled():
                    visitor = config.Instance()
                    if not config.getGenerateCode():
                        visitor.setGenerateEmptyFile()
                    self.__visitor_list.append(visitor)
        #
        return self.__visitor_list

    def _addVisitor(self, type_obj, visitor_list):
        """
        Method to add the visitors to the created object.
        @param type_obj: input element object instance.
        @param list    : list of visitor objects.
        """
        for v in visitor_list:
            type_obj.addVisitor(v)


def main():

    # Configures output only to stdout.
    Logger.connectOutputLogger(None)

    GenFactory.getInstance().configureVisitor(
        "TestComponentH", "ComponentHVisitor", True, True
    )
    GenFactory.getInstance().configureVisitor(
        "TestComponentCpp", "ComponentCppVisitor", True, True
    )

    initfiles = GenFactory.getInstance().create("initFiles")
    print(initfiles)
    initfiles("object args")


if __name__ == "__main__":
    main()
