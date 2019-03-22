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
from __future__ import print_function
import collections
import sys
import os
import struct
import logging
import time
import Queue

from fprime_gds.tkgui.controllers import consumer
from fprime_gds.tkgui.controllers import channel_listener
from fprime_gds.tkgui.controllers import packetize_channel_loader

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize import u16_type
from fprime.common.models.serialize import u8_type

from fprime_gds.tkgui.utils import Logger

class PacketizeChannelListener(consumer.Consumer):
    """
    An listener class that reads incomming packetized channel telemetry,
    converts to individual channels and passes them to the channel listener
    """
    __instance = None


    def __init__(self):
        """
        Constructor.
        WARNING: After the first instantiation setupLogging must be executed.
        """
        super(PacketizeChannelListener, self).__init__()

        # Get dictionary
        self.__channel_listener = channel_listener.ChannelListener.getInstance()
        ch_loader = packetize_channel_loader.PacketizeChannelLoader.getInstance()
        self.__packet_dict = ch_loader.get_packet_dict()


    def getInstance():
        """
        Return instance of singleton.
        """
        if(PacketizeChannelListener.__instance is None):
            PacketizeChannelListener.__instance = PacketizeChannelListener()
        return PacketizeChannelListener.__instance

    #define static method
    getInstance = staticmethod(getInstance)

    def process_data(self, data):
      self.decode_packet(data)

    def decode_packet(self, msg):
        """
        Decode packetized telemetry using packet specification.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0

        # Decode packet ID here...
        u16_obj = u16_type.U16Type()
        u16_obj.deserialize(msg, ptr)
        ptr += u16_obj.getSize()
        packet_id = u16_obj.val
#        print "ID: %d" % packet_id
        
        time_tag = msg[ptr:ptr+11]
        
        # get packet from dictionary
        if not self.__packet_dict.has_key(packet_id):
            print("Packetized telemetry packet id %d not found!" % packet_id)
            return
        
        (packet_name,packet_size,channel_list) = self.__packet_dict[packet_id]
        
        # check size
        if (packet_size != len(msg)):
            print("Packet %s, id %d, size mismatch. Expected: %d Actual: %d" % (packet_name,packet_id,packet_size,len(msg)))
            return
        
        # extract channels from packet
        # print("Decoding packet %s"%packet_name)
        
        packet_offset = 11 + 2 # start at data part of packet
        
        for (channel_id,channel_size,channel_name) in channel_list:
            #print("Decoding channel %s, ID %d, size %d"%(channel_name,channel_id,channel_size))
            # build a new channel object for each channel and pass it to the channel listener
            ch_id_obj = u32_type.U32Type(channel_id)
            #ch_id_obj.val(channel_id)
            # build message for channel decode 
            channel_msg = ch_id_obj.serialize() + time_tag + msg[packet_offset:packet_offset+channel_size]
            # send the channel
            self.__channel_listener.put_data(channel_msg)
            packet_offset += channel_size

    def get_packet(self):
        '''
        Gets a packet, returns decoded packet or None
        @return: None when no packet, decoded packet if exists
        Note: currently, this *always* returns None
        '''
        # Get item off queue
        try:
            msg = self._queue.get_nowait()
        except Queue.Empty:
            # No Events to get
            return None
        return self.decode_packet(msg)
        

if __name__ == "__main__":
    pass
