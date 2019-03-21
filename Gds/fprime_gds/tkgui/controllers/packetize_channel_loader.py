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
from lxml import etree
from fprime_gds.tkgui.controllers import channel_loader


# Add the loadModules method using this decorator
@module_loader.ModuleLoader
class PacketizeChannelLoader(object):
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
        self.__packetize_dict           = dict()

    def getInstance():
        """
        Return instance of singleton.
        """
        if(PacketizeChannelLoader.__instance is None):
            PacketizeChannelLoader.__instance = PacketizeChannelLoader()
        return PacketizeChannelLoader.__instance

    #define static method
    getInstance = staticmethod(getInstance)

    def create(self, packet_specification_file):
        if packet_specification_file == None:
            return
        # read packet specification file
        xml_parser = etree.XMLParser(remove_comments=True)
        fd = open(packet_specification_file,'r')
        element_tree = etree.parse(fd,parser=xml_parser)
        
        channels = channel_loader.ChannelLoader.getInstance()        
    
        # Traverse tree
        packet_list = element_tree.getroot()
        packetNum = 0
        
        for packet in packet_list:
            if packet.tag == "packet":
                packet_size = 0
                packet_name = packet.attrib['name']
                packet_id = int(packet.attrib['id'])
                #print("loading packet %s (%d)"%(packet_name,packet_id))
                channel_list = list()
                for channel in packet:
                    channel_name = channel.attrib['name']
                    try:
                        channel_id = channels.getChDictByName()[channel_name].getId()
                        channel_size = channels.getChDictByName()[channel_name].getType().getSize()
                    except LookupError:
                        raise Exception("Unable to find packetized channel %s, file %s"%(channel_name,packet_specification_file))
                    packet_size += channel_size
                    channel_list.append((channel_id,channel_size,channel_name))
                packet_size += (11 + 2) # raw packet size + time tag + packet id
                self.__packetize_dict[packet_id] = (packet_name,packet_size,channel_list)
        
    def get_packet_dict(self):
        return self.__packetize_dict

            
            
            
            
            

        
