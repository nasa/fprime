#!/bin/env python
#===============================================================================
# NAME: ChannelListener.py
#
# DESCRIPTION: This module is the channel listener class
#              which listens for incoming channel telemetry items via
#              the event_listener thread that handles event messages.
#
#              When a message is received first the descriptor and size are
#              decoded.  Then the channel id is decoded.  The channel id
#              looks up a specific data classes that describes
#              the channel telemetry value and deserilize the value.
#
#              The channel telemetry panel and other panels act as
#              observers and update when new data from this listener
#              is avaliable.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: Apr. 28, 2015
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
import struct
import logging
import time
import Queue
from datetime import datetime

#@todo Add logging to disk of log events via the Logger module.
from fprime_gds.tkgui.controllers import stripchart_listener
from fprime_gds.tkgui.controllers import consumer
from fprime_gds.tkgui.controllers import channel_loader
from fprime_gds.tkgui.controllers import status_bar_updater

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize import u16_type
from fprime.common.models.serialize import u8_type

from fprime_gds.tkgui.utils import Logger

class ChannelListener(consumer.Consumer):
    """
    An channel listener class that reads incomming channel telemetry,
    decodes into values, and updates channel telemetry panels
    within the GUI.
    """
    __instance = None


    def __init__(self):
        """
        Constructor.
        WARNING: After the first instantiation setupLogging must be executed.
        """
        super(ChannelListener, self).__init__()

        # Get dictionary
        ch_loader = channel_loader.ChannelLoader.getInstance()
        self.__ch_obj_dict = ch_loader.getChDict()

        self.__stripchart_listener = stripchart_listener.StripChartListener.getInstance()

        # Instance of status bar updater
        self.__status_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()

        self.__current_ch_msg = None
        self.__opt = None

        self.__logger = None

        self.__csv_logger = None
        self.__csv_delim = '!'

    def getInstance():
        """
        Return instance of singleton.
        """
        if(ChannelListener.__instance is None):
            ChannelListener.__instance = ChannelListener()
        return ChannelListener.__instance

    #define static method
    getInstance = staticmethod(getInstance)


    def setupLogging(self, opt=None):
        """
        Set up logging once for the singleton to use.
        """

        if opt == None:
            p = os.environ['HOME'] + os.sep + 'logs' + os.sep + "channel"
        else:
            p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "channel"
        #
        logfile = 'Channel.log'

        self.__opt = opt
        #
        if not os.path.exists(p):
            os.makedirs(p)
        f = p + os.sep + logfile
        self.__logger = Logger.connectOutputLogger(f, 'channel')
        self.__logger.info("Created log: %s" % f)
        self.__logger.info("User: %s" % os.environ['USER'])


    def setupCsvLogging(self, opt=None):
        """
        Set up CSV logging once for the singleton to use.
        """

        if opt == None:
            p = os.environ['HOME'] + os.sep + 'logs' + os.sep + "channel"
        else:
            p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "channel"
        #
        logfile = 'CsvChannel.log'

        #
        if not os.path.exists(p):
            os.makedirs(p)
        f = p + os.sep + logfile
        self.__csv_logger = Logger.connectOutputLogger(f, 'csv_channel')


    def process_data(self, data):
      self.decode_ch(data)
      self.__log_info(self.getCurrentChannelTelemetryItem())
      self.__log_csv_info(self.getCurrentChannelTelemetryItem())

      # (Bytes received, bytes sent)
      # Descriptor and length of message have already been decoded
      self.__status_bar_updater.put_data((8 + len(data), 0))

    def decode_ch(self, msg):
        """
        Decode channel telemetry item using Channel object dictionary.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0

        # Decode channel ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
        #print "ID: 0x%04X" % i

        # Decode time...
        # Base
        u32_obj = u16_type.U16Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_base = u32_obj.val
        #print "Time Base: %d" % time_base

        # Decode context
        u8_obj = u8_type.U8Type()
        u8_obj.deserialize(msg,ptr)
        ptr += u8_obj.getSize()
        time_context = u8_obj.val
        #print "Time Context: %d" % time_context

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

        # Decode value here...
        # Look up correct Channel channel telemetry instance object for decoding
        if i in self.__ch_obj_dict:
            ch_obj = self.__ch_obj_dict[i]
            #
            # Set time here...
            ch_obj.setTime(time_base,time_context,time_secs,time_usecs)
            #
            # Deserialize to a tuple to stringify
            ch_value = ch_obj.deserialize(msg, ptr)
            #print "Value: %s" % ch_value
            #
            # Package NAME, ID, CH. Desc., time, and Value into tuple for views update.
            # '(Name, id, ch. desc., time_sec, time_usec, value)'
            name = ch_obj.getName()
            i    = ch_obj.getId()
            ch   = ch_obj.getChDesc()
            t    = ch_obj.getTime()
            f    = ch_obj.getFormatString()
            lr   = ch_obj.getLowRed()
            lo   = ch_obj.getLowOrange()
            ly   = ch_obj.getLowYellow()
            hy   = ch_obj.getHighYellow()
            ho   = ch_obj.getHighOrange()
            hr   = ch_obj.getHighRed()

            self.__current_ch_msg = (name, i, ch, t, ch_value,f,lr,lo,ly,hy,ho,hr)

            #print self.__current_ch_msg
        else:
            self.__current_ch_msg = ("Not_Defined", i, "", ("", "", "", ""), "Not_Defined","Not_Defined","NoLowRed","NoLowOrange","NoLowYellow","NoHighYellow","NoHighOrange","NoHighRed")


    def decode_ch_api(self, msg):
        """
        Decode channel telemetry item using Channel object dictionary.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0

        # Decode log channel ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        tlm_id = u32_obj.val
        #print "ID: %d" % i

        # Decode time...
        # Time base
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

        # Decode value here...
        # Look up correct Channel channel telemetry instance object for decoding
        if tlm_id in self.__ch_obj_dict:
          ch_obj = self.__ch_obj_dict[tlm_id]
          # Deserialize to a tuple to stringify
          ch_value = ch_obj.deserialize(msg, ptr)
          return (tlm_id, ch_value)
        else:
          return None


    def get_channel(self):
      # Get item off queue
      try:
        msg = self._queue.get_nowait()
      except Queue.Empty:
        # No Telemetry to get
        return None

      return self.decode_ch_api(msg)


    def getCurrentChannelTelemetryItem(self):
        """
        Used by channel telemetry panel update() to get the channel telemetry to update.
        """
        return self.__current_ch_msg


    def getCurrentChannelTelemetryType(self):
        return self.__current_ch_type


    def __log_info(self, s):
        """
        Update log message with time tag.
        """

        # NOTE(mereweth@jpl.nasa.gov) - gcgandhi saw a bug caused by __log_plot_info
            # being executed before setupPlotLogging. We should do the same check
            # here to make sure the logger has been initialized.
        if self.__logger is None:
            return

        if self.__opt.log_time == "local":
            t = time.strftime("%Y-%m-%d %H:%M:%S:", time.localtime())
        else:
            t = time.strftime("%Y-%m-%d %H:%M:%S:", time.gmtime())

        (channel_name, channel_id, desc, (time_base, time_context, time_seconds, time_usec), value, format_string, low_red, low_orange, low_yellow, high_yellow, high_orange, high_red) = s
        valString = ""
        if format_string == None:
            valString = str(value)
        else:
#            valString = format_string%value
            try:
                valString = format_string%value
            except:
                sys.stderr.write ("%s (%d) Channel decode error\n"%(channel_name,channel_id))
        self.__logger.info("%s %s %s (%s(%s)-%s:%s) %s" % (t,channel_name, channel_id, time_base, time_context, time_seconds, time_usec, valString))


    def __log_csv_info(self, s):
        """
        Update csv log message with time tag.
        """

        if self.__csv_logger is None:
            return

        try:
            (channel_name, channel_id, desc, (time_base, time_context, time_seconds, time_usec), value, format_string, low_red, low_orange, low_yellow, high_yellow, high_orange, high_red) = s

            if self.__opt.log_time == "local":
                # NOTE(mereweth@jpl.nasa.gov) - use this if you can get time
                    # interpretation working through pyKST
                #t = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                t = str(time.time())
            else:
                # NOTE(mereweth@jpl.nasa.gov) - use this if you can get time
                    # interpretation working through pyKST
                #t = datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                t = str(time.time())
            # remote system time
            t += self.__csv_delim
            t += '%f' % (time_seconds + time_usec / 1000.0 / 1000.0)

            # channel name, value, Gse time, remote time
            self.__csv_logger.info(channel_name + self.__csv_delim +
                                   str(value) + self.__csv_delim + t)

        except Exception as e:
            sys.stderr.write ("%s (%d) CSV channel exception %s\n"%(channel_name,channel_id,e))
        except:
            sys.stderr.write ("%s (%d) CSV channel decode error\n"%(channel_name,channel_id))

        return

if __name__ == "__main__":
    pass
