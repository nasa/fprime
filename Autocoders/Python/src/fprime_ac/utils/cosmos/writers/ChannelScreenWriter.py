#!/bin/env python
#===============================================================================
# NAME: ChannelScreenWriter.py
#
# DESCRIPTION: This writer generates the channels.txt file in COSMOS/config/targets
# /DEPLOYMENT_NAME/screens/ directory that contains configuration data for the cosmos
# tlm_viewer application.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

from fprime_ac.utils.cosmos.writers import AbstractCosmosWriter

from fprime_ac.utils.cosmos.templates import Channel_Screen

class ChannelScreenWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates the channel screen definition file in
    cosmos_directory/config/targets/deployment_name.upper()/screens/
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels, commands, and events
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(ChannelScreenWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        self.repeated_names = {}
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/targets/" + deployment_name.upper() + "/screens/"
        
                    
    def write(self):
        """
        Generates the file
        """
        channel_list= []
        for ch in self.cmd_tlm_data[0]:
            n = ch.get_ch_name()
            if n in self.repeated_names.keys():
                # Fix other name pair
                if n in channel_list:
                    channel_list.remove(n)
                    channel_list.append(self.repeated_names.get(n).get_comp_name() + "_" + n)
                n = ch.get_comp_name() + "_" + n
            self.repeated_names.update({n: ch})
            channel_list.append(n)
        channel_list = sorted(channel_list)
        
        # Open file
        fl = open(self.destination + "channels.txt", "w")
        if CosmosUtil.VERBOSE:
            print("Channel Screen Created")
        
        # Initialize and fill cheetah template
        c = Channel_Screen.Channel_Screen()
        
        c.date = CheetahUtil.DATE
        c.user = CheetahUtil.USER
        c.target_name = self.deployment_name.upper()
        c.channels = channel_list
                    
        msg = c.__str__()
                    
        fl.writelines(msg)
        fl.close()
