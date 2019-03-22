#!/bin/env python
#===============================================================================
# NAME: SocketListener.py
#
# DESCRIPTION: This module is the socket listener class that is a thread
#              which listens for incomming fprime messages. When a
#              message is received first the descriptor and size are
#              decoded.  The message is then placed on the appropriate
#              listener's queue.
#
# AUTHOR: Brian Campuzano
# DATE CREATED: July 10, 2017
#
# Copyright 2017, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
from __future__ import print_function
import collections
import sys
import os
import threading
import Queue
import struct
import logging
import time

from fprime_gds.tkgui.controllers import observer
from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import channel_listener
from fprime_gds.tkgui.controllers import packetize_channel_listener
from fprime_gds.tkgui.controllers import status_bar_updater
from fprime_gds.tkgui.controllers import stripchart_listener

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize import u16_type
from fprime.common.models.serialize import u8_type

from fprime_gds.tkgui.utils import Logger

class SocketListener:
    """
    A socket listener class that monitors incoming packets,
    decodes them into strings, and updates event and telemetry listeners
    within the GUI.
    @todo: For now connect directly to the socket but in future mediate packets.
    """
    __instance = None


    def __init__(self, sock=None):
        """
        Constructor.
        WARNING: After the first instantiation setupLogging must be executed.
        """

        # Socket when connection established
        self.__sock = sock

        # Thread handler
        self.__thread = threading.Thread()

        # Instance the event listener here
        self.__event_listen = event_listener.EventListener.getInstance()

        # Instance the channel telemetry listener here
        self.__channel_listen = channel_listener.ChannelListener.getInstance()
        
        # Instance the packetized channel telemetry listener
        self.__packetized_channel_listen = packetize_channel_listener.PacketizeChannelListener.getInstance()

        # Status bar observer
        self.__status_bar_updater  = status_bar_updater.StatusBarUpdater.getInstance()

        # setup the bin file
        self.__binfile = None

        # store options
        self.__opt = None

    def getInstance():
        """
        Return instance of singleton.
        """
        if(SocketListener.__instance is None):
            SocketListener.__instance = SocketListener()
        return SocketListener.__instance

    #define static method
    getInstance = staticmethod(getInstance)

    def setupBinaryLogging(self, opt=None):
        """
        Set up binary data logging once for the singleton to use.
        """

        # don't log binary data if flag has been set to 0
        if Logger.BIN_LOGGER_FLAG == 0:
            return

        if opt == None:
          p = os.environ['HOME'] + os.sep + 'logs' + os.sep + "binary"
        else:
          p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "binary"

        self.__opt = opt

        if not os.path.exists(p):
          os.makedirs(p)
          logfilename = 'RAW.bin'
          outfile = p + os.sep + logfilename
          #bufsize of 0 tells python not to buffer data when writing to file, just flush
        bufsize = 0
        self.__binfile = open(outfile, "wb", bufsize)

    def close_bin_logfile(self):
      if self.__binfile:
        self.__binfile.close()
        self.__binfile = None

    def register_main_panel(self, the_main_panel):
        """
        Register the main panel to get socket handle
        if one is set.  This is an indication of connection.
        """
        self.__the_main_panel = the_main_panel
        self.__sock = self.__the_main_panel.getSock()

    def register_status_bar(self, status_bar):
      self.__status_bar = status_bar

    def receive_packet(self, sock):
        """
        Receive packet by first reading 4 byte size,
        then 4 byte desc. Then recieve the rest of message.
        Send size, desc, id, and optional args for decoding.
        """
        pkt_len = sock.recv(4)
        if self.__binfile != None:
          self.__binfile.write(pkt_len)

        pkt_desc = sock.recv(4)
        if self.__binfile != None:
          self.__binfile.write(pkt_desc)

        desc = int(struct.unpack(">I",pkt_desc)[0])
        size = int(struct.unpack(">I",pkt_len)[0])

        data = sock.recv(size - u32_type.U32Type().getSize())

        if self.__binfile != None:
          self.__binfile.write(data)

        return (size, desc, data)

    def enqueue_output(self, sock, event_listen, channel_listen, packet_channel_listen):
        """
        Queue up socket data for TK processing
        """
        while 1:
            try:
              length, descriptor, data = self.receive_packet(sock)
              if descriptor == 1:
                channel_listen.put_data(data)
              elif descriptor == 2:
                event_listen.put_data(data)
              elif descriptor == 3: # idle packet
                pass
              elif descriptor == 4: # packetized telemetry
                packet_channel_listen.put_data(data)
              else:
                print("Unknown descriptor: " + str(descriptor))
            except:
              print("Socket connection terminated")
              #raise IOError("Socket connection terminated")
              break
        return

    def connect(self, sock):
        """
        Start thread that is connected to sock talking to TCPThreadServer.py
        THis is called from the TCP Server menu Connect... item.
        """
        self.__sock = sock

        if self.__thread.isAlive() == True:
            ##print "LISTENER THREAD IS ALIVE!"
            return

        # create background listener thread
        self.__thread = threading.Thread(
          target=self.enqueue_output,
          args=(self.__sock, self.__event_listen, self.__channel_listen, self.__packetized_channel_listen)
        )
        # thread dies with the program
        self.__thread.daemon = True
        # state listener thread here
        self.__thread.start()


    def decode_desc(self, msg):
        """
        Decode the size and descriptor and return them.
        """
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
        return (desc, size)

    def decode_desc_api(self, msg):
        """
        Decode the size and descriptor and return them.
        """
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

        # Decode log event ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
        #print "ID: %d" % i
        return (desc, size, i)

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
