#!/bin/env python
#===============================================================================
# NAME: ConfigDataViewerWriter.py
#
# DESCRIPTION: This writer generates the data_viewer.txt in COSMOS/config/
# tools/data_viewer/ directory that defines the existing targets for the data_viewer
# application
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import os

from fprime_ac.utils.cosmos.writers import BaseConfigWriter

from fprime_ac.utils.cosmos.templates import Config_Data_Viewer

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

class ConfigDataViewerWriter(BaseConfigWriter.BaseConfigWriter):
    """
    This class generates the data viewer config file in
    cosmos_directory/config/tools/data_viewer/
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory, old_definition=None):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        @param old_definition: COSMOS target name that you want to remove
        """
        super(ConfigDataViewerWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory, old_definition)
        self.repeated_names = {}
        self.token = "TARGET_COMPONENT"
        self.argument = ""
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/tools/data_viewer/"
        
                    
    def write(self):
        """
        Generates the file
        """
        # Add target to list of lines that will always be written
        ignored_lines = []
        if self.deployment_name and not self.deployment_name == "":
            ignored_lines.append(self.token + " " + self.deployment_name.upper())
        
        # Open file for reading if exists already and parse all old targets
        names = []
        fl_loc = self.destination + 'data_viewer.txt'
        if os.path.isfile(fl_loc):
            names = self.read_for_token(fl_loc, self.token, ignored_lines)
            if CosmosUtil.VERBOSE:
                print("Data Viewer Tool Config Altered")
        else:
            if CosmosUtil.VERBOSE:
                print("Data Viewer Tool Config Created")
            
        for line in ignored_lines:
            names.append(line.split(" ")[1] + self.argument)
        
        # Open file
        fl = open(fl_loc, "w")
         
        # Initialize and fill Cheetah template
        dv = Config_Data_Viewer.Config_Data_Viewer()
         
        dv.date = CheetahUtil.DATE
        dv.user = CheetahUtil.USER
        dv.names = names
                     
        msg = dv.__str__()
                     
        fl.writelines(msg)
        fl.close()
