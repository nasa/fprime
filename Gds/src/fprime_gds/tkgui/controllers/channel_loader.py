#!/bin/env python
#===============================================================================
# NAME: ChannelLoader.py
#
# DESCRIPTION: This module contains a singleton for loading automatically
#              generated Python descriptor files of channelized telemetry.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Apr. 27, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import glob
import os
import sys
import collections
from collections import OrderedDict

from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.common.models.common import channel_telemetry
from fprime_gds.tkgui.controllers import module_loader

# Add the loadModules method using this decorator
@module_loader.ModuleLoader
class ChannelLoader(object):
    '''
    Singleton loader class used to capture channel descriptor
    files and instance Channel objects.  The objects are
    then stored in a dictionary of form { ID : Event Instance }
    so the listener can this for decoding and formatting the
    event into a string for display.
    '''
    __instance = None

    def __init__(self):
        '''
        Constructor
        '''
        # Main dictionaries with key always id name.
        self.__dict_name           = dict()
        self.__dict_ch_description = dict()
        self.__dict_types           = dict()
        self.__dict_channels       = dict()
        self.__dict_channels_by_name = dict()
        self.__dict_format_string = dict()

    def getInstance():
        """
        Return instance of singleton.
        """
        if(ChannelLoader.__instance is None):
            ChannelLoader.__instance = ChannelLoader()
        return ChannelLoader.__instance

    #define static method
    getInstance = staticmethod(getInstance)

    def create(self, generated_ch_path):
        """
        Generate all the dictionaries and instances of Channel here...
        """
        name_list          = self.loadModules(generated_ch_path, "ID", "NAME")
        component_list     = self.loadModules(generated_ch_path, "ID", "COMPONENT")
        ch_desc_list       = self.loadModules(generated_ch_path, "ID", "CHANNEL_DESCRIPTION")
        type_list          = self.loadModules(generated_ch_path, "ID", "TYPE")
        format_string_list = self.loadModules(generated_ch_path, "ID", "FORMAT_STRING")
        low_red_list       = self.loadModules(generated_ch_path, "ID", "LOW_RED")
        low_orange_list    = self.loadModules(generated_ch_path, "ID", "LOW_ORANGE")
        low_yellow_list    = self.loadModules(generated_ch_path, "ID", "LOW_YELLOW")
        high_yellow_list   = self.loadModules(generated_ch_path, "ID", "HIGH_YELLOW")
        high_orange_list    = self.loadModules(generated_ch_path, "ID", "HIGH_ORANGE")
        high_red_list      = self.loadModules(generated_ch_path, "ID", "HIGH_RED")

        # Remove duplicates from component list
        component_list = list(OrderedDict.fromkeys(component_list))

        #
        # Generate required dictionaries here
        #
        self.__dict_name = dict(name_list)
        self.__dict_component = dict(component_list)
        self.__dict_ch_desc = dict(ch_desc_list)
        self.__dict_types = dict(type_list)
        self.__dict_format_string = dict(format_string_list)
        self.__dict_low_red = dict(low_red_list)
        self.__dict_low_orange = dict(low_orange_list)
        self.__dict_low_yellow = dict(low_yellow_list)
        self.__dict_high_yellow = dict(high_yellow_list)
        self.__dict_high_orange = dict(high_orange_list)
        self.__dict_high_red = dict(high_red_list)
        #
        for id in self.__dict_name:
            # print self.__dict_name[id], id, self.__dict_ch_desc[id], self.__dict_types[id]
            self.__dict_channels[id] = channel_telemetry.Channel(
                                        self.__dict_name[id],
                                        id,
                                        self.__dict_component[id],
                                        self.__dict_ch_desc[id],
                                        self.__dict_types[id],
                                        self.__dict_format_string[id],
                                        self.__dict_low_red[id],
                                        self.__dict_low_orange[id],
                                        self.__dict_low_yellow[id],
                                        self.__dict_high_yellow[id],
                                        self.__dict_high_orange[id],
                                        self.__dict_high_red[id]
                                        )

        for id, name in self.__dict_name.iteritems():
            self.__dict_channels_by_name[name] = channel_telemetry.Channel(
                                        self.__dict_name[id],
                                        id,
                                        self.__dict_component[id],
                                        self.__dict_ch_desc[id],
                                        self.__dict_types[id],
                                        self.__dict_format_string[id],
                                        self.__dict_low_red[id],
                                        self.__dict_low_orange[id],
                                        self.__dict_low_yellow[id],
                                        self.__dict_high_yellow[id],
                                        self.__dict_high_orange[id],
                                        self.__dict_high_red[id]
                                        )

    def getNameDict(self):
        return self.__dict_name

    def getCompDict(self):
        return self.__dict_component

    def getChDescDict(self):
        return self.__dict_ch_desc

    def getTypesDict(self):
        return self.__dict_types

    def getFormatStringDict(self):
        return self.__dict_format_string

    def getLowRedDict(self):
        return self.__dict_low_red

    def getLowOrangeDict(self):
        return self.__dict_low_orange

    def getLowYellowDict(self):
        return self.__dict_low_yellow

    def getHighYellowDict(self):
        return self.__dict_high_yellow

    def getHighOrangeDict(self):
        return self.__dict_high_orange

    def getHighRedDict(self):
        return self.__dict_high_red

    def getChDict(self):
        return self.__dict_channels

    def getChDictByName(self):
        return self.__dict_channels_by_name
