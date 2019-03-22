#!/bin/env python
#===============================================================================
# NAME: EventListener.py
#
# DESCRIPTION: This module is the event listener class that is a thread
#              which listens for incomming log event messages. When a
#              message is received first the descriptor and size are
#              decoded.  Then the event id is decoded.  The event id
#              looks up a specific Event class instance that describes
#              the event id type and can deserilize it.
#
#              The event logging panel and other panels act as
#              observers and update when new data from the listen
#              thread is avaliable.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: Feb 9, 2014
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import collections
import sys
import os
import threading
import Queue
import struct
import logging
import time

#@todo Add logging to disk of log events via the Logger module.

from fprime_gds.tkgui.controllers import consumer
from fprime_gds.tkgui.controllers import event_loader
from fprime_gds.tkgui.controllers import status_bar_updater

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize import u16_type
from fprime.common.models.serialize import u8_type

from fprime_gds.tkgui.utils import Logger


class EventListener(consumer.Consumer):
    """
    An event listener class that monitors incoming events,
    decodes them into strings, and updates Logger panel
    within the GUI.
    @todo: For now connect directly to the socket but in future mediate packets.
    """
    __instance = None


    def __init__(self):
        """
        Constructor.
        WARNING: After the first instantiation setupLogging must be executed.
        """
        super(EventListener, self).__init__()

        # Instance the event loader here
        self.__event_loader = event_loader.EventLoader.getInstance()
        self.__event_obj_dict = self.__event_loader.getEventsDict()

        # Instance of status bar updater
        self.__status_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()

        # Current event log message string
        self.__current_event_log_msg = None

        # setup the bin file
        self.__binfile = None

        # store options
        self.__opt = None


    def getInstance():
        """
        Return instance of singleton.
        """
        if(EventListener.__instance is None):
            EventListener.__instance = EventListener()
        return EventListener.__instance


    #define static method
    getInstance = staticmethod(getInstance)


    def setupLogging(self, opt=None):
        """
        Set up logging once for the singleton to use.
        """

        if opt == None:
            p = os.environ['HOME'] + os.sep + 'logs' + os.sep + "event"
        else:
            p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "event"
        #
        logfile = 'Event.log'
        #
        self.__opt = opt

        if not os.path.exists(p):
            os.makedirs(p)
        f = p + os.sep + logfile
        self.__logger = Logger.connectOutputLogger(f, 'event_out')
        self.__logger.info("Created log: %s" % f)
        self.__logger.info("User: %s" % os.environ['USER'])


    def process_data(self, data):
      '''
      Decode event and setup log message
      '''

      self.decode_event(data)
      self.__log_info(self.getCurrentEventLogMsg())

      # (Bytes received, bytes sent)
      # Descriptor and length of message have already been decoded
      self.__status_bar_updater.put_data((8 + len(data), 0))


    def decode_event(self, msg):
        """
        Decode event log message using Event object dictionary.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0

        # Decode log event ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
        #print "ID: %d" % i

        # Decode time...
        # Base
        u32_obj = u16_type.U16Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_base = u32_obj.val
        #print "Time Base: %d" % time_base

        # Time context
        u8_obj = u8_type.U8Type()
        u8_obj.deserialize(msg,ptr)
        ptr += u8_obj.getSize()
        time_context = u8_obj.val

        # Seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_secs = u32_obj.val
        #print "Time Seconds: %d" % time_secs
        # Micro-seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_usecs = u32_obj.val
        #print "Time MicroSeconds: %d" % time_usecs

        # Decode arguments here...
        # Look up correct Event event log message instance object for decoding
        if i in self.__event_obj_dict:
            event_obj = self.__event_obj_dict[i]
            #
            # Deserialize to a tuple to stringify
            event_args = event_obj.deserialize(msg[ptr:])
            # Stringify to formatted message string for GUI updates
            fmt = event_obj.stringify(event_args)
            #
            # Package NAME, ID, SEVERITY, and formatted string into tuple for views update.
            # 'Name (id) Severity : formatted string'
            name = event_obj.getName()
            event_id = event_obj.getId()
            severity = event_obj.getSeverity()

            # See if set to workstation time
            if (time_base == 2) or (time_base == 3):
                if self.__opt.log_time == "local":
                  event_time = time.strftime("%m/%d-%H:%M:%S",time.localtime(time_secs))
                else:
                  event_time = time.strftime("%m/%d-%H:%M:%S",time.gmtime(time_secs))

            else:
              event_time = "%f" % (float(time_secs) + (float(time_usecs)/1000000))
              # Create tuple of log msg string and log msg object
            msg_tup = (event_time, name, event_id, severity, fmt)
            self.__current_event_log_msg = ("%s: %s (%d) %s : %s" % msg_tup, msg_tup)
            #print self.__current_event_log_msg
        else:
          self.__current_event_log_msg = "EVR ID Received but could not be decoded: %d" % i

    def decode_event_api(self, msg):
        """
        Decode event log message using Event object dictionary.
        @return (Event ID, Event Arguments)
        """

        ptr = 0

        # Decode log event ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        event_id = u32_obj.val
        #print "ID: %d" % i

        # Decode time...
        # Base
        u32_obj = u16_type.U16Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_base = u32_obj.val
        #print "Time Base: %d" % time_base

        # Time context
        u8_obj = u8_type.U8Type()
        u8_obj.deserialize(msg,ptr)
        ptr += u8_obj.getSize()
        time_context = u8_obj.val

        # Seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_secs = u32_obj.val
        #print "Time Seconds: %d" % time_secs
        # Micro-seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_usecs = u32_obj.val
        #print "Time MicroSeconds: %d" % time_usecs

        if event_id in self.__event_obj_dict:
          event_obj = self.__event_obj_dict[event_id]
          event_args = event_obj.deserialize(msg[ptr:])[1:]
          return (event_id, event_args)
        else:
          return None


    def get_event(self):
      # Get item off queue
      try:
        msg = self._queue.get_nowait()
      except Queue.Empty:
        # No Events to get
        return None

      return self.decode_event_api(msg)

    def getCurrentEventLogMsg(self):
        """
        Used by logger panel update() to get the string to display.
        """
        return self.__current_event_log_msg


    def __log_info(self, s):
        """
        Update log message with time tag.
        """
        if self.__opt.log_time == "local":
            t = time.strftime("%Y-%m-%d %H:%M:%S:", time.localtime())
        else:
            t = time.strftime("%Y-%m-%d %H:%M:%S:", time.gmtime())

        self.__logger.info("%s %s" % (t,s[0]))


if __name__ == "__main__":
    pass
