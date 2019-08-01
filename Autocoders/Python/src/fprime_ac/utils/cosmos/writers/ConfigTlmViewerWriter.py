#!/bin/env python
#===============================================================================
# NAME: ConfigTlmViewerWriter.py
#
# DESCRIPTION: This writer generates the tlm_viewer.txt in COSMOS/config/
# tools/tlm_viewer/ directory that defines the existing targets for the 
# tlm_viewer application
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

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

from fprime_ac.utils.cosmos.templates import Config_Tlm_Viewer

class ConfigTlmViewerWriter(BaseConfigWriter.BaseConfigWriter):
    """
    This class generates the tlm viewer config file in
    cosmos_directory/config/tools/tlm_viewer/
    """
    
    def __init__(self, parser, deployment_name, cosmos_directory, old_definition=None):
        """
        @param cmd_tlm_data: Tuple containing lists channels, commands, and events
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        @param old_definition: COSMOS target name that you want to remove
        """
        super(ConfigTlmViewerWriter, self).__init__(parser, deployment_name, cosmos_directory, old_definition)
        self.token = "AUTO_TARGET"
        self.argument = ""
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/tools/tlm_viewer/"
        
                    
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
        fl_loc = self.destination + 'tlm_viewer.txt'
        if os.path.isfile(fl_loc):
            names = self.read_for_token(fl_loc, self.token, ignored_lines)
            if CosmosUtil.VERBOSE:
                print("Tlm Viewer Tool Config Altered")
        else:
            if CosmosUtil.VERBOSE:
                print("Tlm Viewer Tool Config Created")
                
        for line in ignored_lines:
            names.append(line.split(" ")[1] + "")
        
        # Open file
        fl = open(fl_loc, "w")
        
        # Initialize and fill Cheetah template 
        tv = Config_Tlm_Viewer.Config_Tlm_Viewer()
         
        tv.date = CheetahUtil.DATE
        tv.user = CheetahUtil.USER
        tv.names = names
                     
        msg = tv.__str__()
                     
        fl.writelines(msg)
        fl.close()
