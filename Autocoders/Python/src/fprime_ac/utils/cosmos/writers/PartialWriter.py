#!/bin/env python
#===============================================================================
# NAME: PartialWriter.py
#
# DESCRIPTION: This writer generates the partial files (start with _) in 
# the COSMOS/config/targets/DEPLOYMENT_NAME/cmd_tlm/* directories that contain 
# configuration data for each channel, command, and event as well as the partial
# file in the COSMOS/config/targets/DEPLOYMENT_NAME/tools/data_viewer/ directory.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import os

from fprime_ac.utils.cosmos.writers import AbstractCosmosWriter

from fprime_ac.utils.cosmos.templates import Partial_Channel
from fprime_ac.utils.cosmos.templates import Partial_Command
from fprime_ac.utils.cosmos.templates import Partial_Data_Viewer
from fprime_ac.utils.cosmos.templates import Partial_Event

from fprime_ac.utils.cosmos.util import CosmosUtil

class PartialWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates each of the files that the user must input their own data into:
    _tlm_chn_hdr.txt: Channel header file that contains all shared channel definition fields
    _cmds_hdr.txt: Command header file that contains all shared command definition fields
    _tlm_evr_hdr.txt: Event header file that contains all shared event definition fields
    _user_dataviewers.txt: Contains all user/inputted data viewer fields
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(PartialWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        
        # Initialize writer-unique file destination location
        self.overwrite_destinations = {
            cosmos_directory + "/config/targets/" + deployment_name.upper() + "/cmd_tlm/channels/_" + deployment_name.lower() + "_tlm_chn_hdr.txt": Partial_Channel.Partial_Channel(),
            cosmos_directory + "/config/targets/" + deployment_name.upper() + "/cmd_tlm/commands/_" + deployment_name.lower() + "_cmds_hdr.txt": Partial_Command.Partial_Command(),
            cosmos_directory + "/config/targets/" + deployment_name.upper() + "/cmd_tlm/events/_" + deployment_name.lower() + "_tlm_evr_hdr.txt": Partial_Event.Partial_Event()
            }
        self.leave_alone_destinations = {
            cosmos_directory + "/config/targets/" + deployment_name.upper() + "/tools/data_viewer/_user_dataviewers.txt": Partial_Data_Viewer.Partial_Data_Viewer()
            }
                    
    def write(self):
        """
        Generates the file
        """
        for destination in self.overwrite_destinations.keys():
            fl = open(destination, "w")
                
            # All partial Cheetah template files do not contain variables, so can just be spit back out
            tmpl = self.overwrite_destinations[destination]
                
            msg = tmpl.__str__()
                
            fl.writelines(msg)
            fl.close()
                
            if CosmosUtil.VERBOSE:
                print(destination + " Created")
                
        for destination in self.leave_alone_destinations.keys():
            if not os.path.exists(destination):
                fl = open(destination, "w")
                
                # All partial Cheetah template files do not contain variables, so can just be spit back out
                tmpl = self.leave_alone_destinations[destination]
                
                msg = tmpl.__str__()
                
                fl.writelines(msg)
                fl.close()
                
                if CosmosUtil.VERBOSE:
                    print(destination + " Created")
            else:
                if CosmosUtil.VERBOSE:
                    print(destination + " Already exists")
