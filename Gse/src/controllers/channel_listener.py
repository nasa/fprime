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
#@todo Add logging to disk of log events via the Logger module.
from controllers import stripchart_listener
from controllers import observer
from controllers import channel_loader
from models.serialize import type_base
from models.serialize import u32_type
from models.serialize import u16_type
from models.serialize import u8_type
#from views import main_panel
from utils import Logger



class ChannelListener(observer.Observed):
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
        #
        # Instance the channel loader here
        #
        self.__channel_loader = channel_loader.ChannelLoader.getInstance()
        self.__ch_obj_dict = self.__channel_loader.getChDict()
        #
        self.__stripchart_listener = stripchart_listener.StripChartListener.getInstance()

        self.__current_ch_msg = None
        self.__opt = None


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
            p = os.environ['HOME'] + os.sep + 'fprime_logs' + os.sep + "channel"
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


    def decode_ch(self, msg):
        """
        Decode channel telemetry item using Channel object dictionary.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0
        # Decode size here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        size = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Size = 0x%x" % size

        # Decode descriptor part of message
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        desc = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Desc = 0x%x" % desc

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
        # Decode size here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, 0)
        size = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Size = 0x%x" % size

        # Decode descriptor part of message
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        desc = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Desc = 0x%x" % desc

        # Decode log channel ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
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

        val = []
        # Decode value here...
        # Look up correct Channel channel telemetry instance object for decoding
        if i in self.__ch_obj_dict:
            ch_obj = self.__ch_obj_dict[i]
            #
            # Set time here...
            ch_obj.setTime(time_base,time_context, time_secs,time_usecs)
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
            self.__current_ch_msg = ("Not_Defined", i, "", ("","",""), "Not_Defined","NoLowRed","NoLowOrange","NoLowYellow","NoHighYellow","NoHighOrange","NoHighRed")
        return ch_value

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


    def update(self, serial_ch_telm):
        """
        Update channel telemetry here...
        1. Receive channel telemetry packet from thread.
        2. Decode channel values into object.
        3. Generate updates for all channel telemetry instances.
        """
        #
        # Observer update will be generated here...
        #
        if len(serial_ch_telm) > 0:
            self.decode_ch(serial_ch_telm)
            self.observers_notify()
            #print self.getCurrentChannelTelemetryItem()
            self.__log_info(self.getCurrentChannelTelemetryItem())
        return


if __name__ == "__main__":
    pass
