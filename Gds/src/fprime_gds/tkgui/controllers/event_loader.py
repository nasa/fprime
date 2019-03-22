#!/bin/env python
#===============================================================================
# NAME: EventLoader.py
#
# DESCRIPTION: This module contains a singleton for loading automatically
#              generated Python descriptor files of log events.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Jan. 9, 2015
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

from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.common.models.common import event
from fprime_gds.tkgui.controllers import module_loader

# Add the loadModules method using this decorator
@module_loader.ModuleLoader
class EventLoader(object):
    '''
    Singleton loader class used to capture event descriptor
    files and instance Event objects.  The objects are
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
        self.__dict_name          = dict()
        self.__dict_severity      = dict()
        self.__dict_format_string = dict()
        self.__dict_event_description = dict()
        self.__dict_arguments     = dict()
        self.__dict_events        = dict()
        self.__dict_events_by_name = dict()


    def getInstance():
        """
        Return instance of singleton.
        """
        if(EventLoader.__instance is None):
            EventLoader.__instance = EventLoader()
        return EventLoader.__instance

    #define static method
    getInstance = staticmethod(getInstance)


    def create(self, generated_events_path):
        """
        Generate all the dictionaries and instances of Command here...
        """
        name_list          = self.loadModules(generated_events_path, "ID", "NAME")
        severity_list      = self.loadModules(generated_events_path, "ID", "SEVERITY")
        format_string_list = self.loadModules(generated_events_path, "ID", "FORMAT_STRING")
        event_desc_list    = self.loadModules(generated_events_path, "ID", "EVENT_DESCRIPTION")
        args_list          = self.loadModules(generated_events_path, "ID", "ARGUMENTS")
        #
        # Change Severity to enum
        #
        severity_list = [(key, event.Severity.__members__[value]) for (key, value) in severity_list]

        #
        # Generate required dictionaries here
        #
        self.__dict_name = dict(name_list)
        self.__dict_severity = dict(severity_list)
        self.__dict_format_string = dict(format_string_list)
        self.__dict_event_description = dict(event_desc_list)
        self.__dict_arguments = dict(args_list)
        #
        for id in self.__dict_name:
            self.__dict_events[id] = event.Event( \
                                        self.__dict_name[id], \
                                        id, \
                                        self.__dict_severity[id], \
                                        self.__dict_format_string[id], \
                                        self.__dict_event_description[id], \
                                        self.__dict_arguments[id])

        for id, name in self.__dict_name.iteritems():
            self.__dict_events_by_name[name] = event.Event( \
                                        self.__dict_name[id], \
                                        id, \
                                        self.__dict_severity[id], \
                                        self.__dict_format_string[id], \
                                        self.__dict_event_description[id], \
                                        self.__dict_arguments[id])


    def getNameDict(self):
        return self.__dict_name

    def getSeverity(self):
        return self.__dict_severity

    def getFormatString(self):
        return self.__dict_format_string

    def getEventDesc(self):
        return self.__dict_arguments

    def getEventsDict(self):
        return self.__dict_events

    def getEventsDictByName(self):
        return self.__dict_events_by_name
