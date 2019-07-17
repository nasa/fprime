#!/bin/env python
#===============================================================================
# NAME: DataViewerWriter.py
#
# DESCRIPTION: This writer generates the data_viewer.txt file in COSMOS/config/targets
# /DEPLOYMENT_NAME/tools/data_viewer directory that contains configuration data for
# the EVR screen in the data_viewer application.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

from fprime_ac.utils.cosmos.writers import AbstractCosmosWriter

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

from fprime_ac.utils.cosmos.templates import Data_Viewer

class DataViewerWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates the data viewer definition file in
    cosmos_directory/config/targets/deployment_name.upper()/tools/data_viewer/
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(DataViewerWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        self.repeated_names = {}
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/targets/" + deployment_name.upper() + "/tools/data_viewer/"
        
                    
    def write(self):
        """
        Generates the file
        """
        event_list= []
        for evr in self.cmd_tlm_data[2]:
            n = evr.get_evr_name()
            if n in self.repeated_names.keys():
                # Fix other name pair
                if n in event_list:
                    event_list.remove(n)
                    event_list.append(self.repeated_names.get(n).get_comp_name() + "_" + n)
                n = evr.get_comp_name() + "_" + n
            self.repeated_names.update({n: evr})
            event_list.append(n)
        event_list = sorted(event_list)
        
        # Open file
        fl = open(self.destination + "data_viewer.txt", "w")
        if CosmosUtil.VERBOSE:
            print("Data Viewer Created")
        
        # Initialize and fill cheetah template
        dv = Data_Viewer.Data_Viewer()
        
        dv.date = CheetahUtil.DATE
        dv.user = CheetahUtil.USER
        dv.target_upper = self.deployment_name.upper()
        dv.target_name = self.deployment_name.upper()
        dv.evr_names = event_list
                    
        msg = dv.__str__()
                    
        fl.writelines(msg)
        fl.close()
