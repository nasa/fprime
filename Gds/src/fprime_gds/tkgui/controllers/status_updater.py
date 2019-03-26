#!/bin/env python
#===============================================================================
# NAME: StatusUpdater.py
#
# DESCRIPTION: This module is a status updater which uses the observer
#                pattern to update all instanced panels. This module will
#                utilize the logging module to create disk file logs of
#                status updates.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: Feb 27, 2014
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import sys
import os
import logging
import time

from fprime_gds.tkgui.controllers import observer
from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.tkgui.controllers import status_bar_updater

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize import u32_type
from fprime_gds.tkgui.utils import Logger

class StatusUpdater(observer.Observable):
    """
    A singleton class that will update all text widgets
    on all active instances of UI windows.  All the various
    objects within this program will use this to update
    status in the common text widget area of the panels.

    Also every update is saved in a default log file

    @todo: Future implementations will utilize ConfigManager
    making it possible to override the path and filename.
    """
    __instance = None


    def __init__(self):
        """
        Constructor.
        WARNING: After the first instantiation setupLogging must be executed.
        """
        super(StatusUpdater, self).__init__()
        #
        # Variables used by standard Update interface
        #
        self.__text_color = 'black'
        self.__status_string = ''
        self.__update_mode = "Update"
        self.__opt = None

        # Connect messages from status window to status bar
        self.__status_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()
        #
        setattr(self, "__text_color", 'black')
        setattr(self, "__status_string", '')
        setattr(self, "__update_mode", "Update")


    def getInstance():
        """
        Return instance of singleton.
        """
        if(StatusUpdater.__instance is None):
            StatusUpdater.__instance = StatusUpdater()
        return StatusUpdater.__instance

    #define static method
    getInstance = staticmethod(getInstance)

    def setupLogging(self, opt=None):
        """
        Set up logging once for the singleton to use.
        """

        if opt == None:
            p = os.environ['HOME'] + os.sep + 'logs' + os.sep + "status"
        else:
            p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "status"
        #
        self.__opt = opt
        logfile = 'Status.log'
        #
        if not os.path.exists(p):
            os.makedirs(p)
        f = p + os.sep + logfile
        self.__logger = Logger.connectOutputLogger(f)
        self.__logger.info("Created log: %s" % f)
        self.__logger.info("User: %s" % os.environ['USER'])

    def __log_info(self, s):
        """
        Update log message with time tag.
        """
        if self.__opt.log_time == "local":
            t = time.strftime("%y-%m-%d %H:%M:%S:", time.localtime())
        else:
            t = time.strftime("%y-%m-%d %H:%M:%S:", time.gmtime())
        self.__logger.info("%s %s" % (t,s))


    def update(self, text, color='black'):
        """
        Update status on the text widget.
        By appending text to the end.
        Calls statusUpdate within each panel.
        """
        self.text_color = color
        self.status_string = text
        self.update_mode = 'Update'
        self.notifyObservers(self.update_mode)

        self.__status_bar_updater.update_message(text, color=color)
        self.__log_info(text)


    def clear(self):
        """
        Clear the text widget.
        """
        self.text_color = 'black'
        self.status_string = ""
        self.update_mode = 'Clear'
        self.notifyObservers(self.update_mode)

    def setText(self, text):
        """
        Reset the text in the status text widget.
        This clears any text and puts new text in.
        """
        self.text_color = 'black'
        self.status_string = text
        self.update_mode = "Text"
        self.notifyObservers(self.update_mode)
        self.__log_info(text)
    #
    # Interfaces for observer update method to utilize
    @property
    def text_color(self):
        return self.__text_color

    @text_color.setter
    def text_color(self, c):
        self.__text_color = c

    @property
    def status_string(self):
        return self.__status_string

    @status_string.setter
    def status_string(self, s):
        self.__status_string = s

    @property
    def update_mode(self):
        return self.__update_mode

    @update_mode.setter
    def update_mode(self, u):
        self.__update_mode = u


if __name__ == "__main__":
    pass
