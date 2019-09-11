#!/bin/env python
#===============================================================================
# NAME: ConfigServerWriter.py
#
# DESCRIPTION: This writer generates the cmd_tlm_server.txt in COSMOS/config/
# tools/cmd_tlm_server directory that defines the existing targets for the cmd_tlm_server
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

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

from fprime_ac.utils.cosmos.templates import Config_Server

class ConfigServerWriter(BaseConfigWriter.BaseConfigWriter):
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
        super(ConfigServerWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory, old_definition)
        self.repeated_names = {}
        self.token = "INTERFACE_TARGET"
        self.argument = " cmd_tlm_server.txt"
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/tools/cmd_tlm_server/"
        
                    
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
        fl_loc = self.destination + 'cmd_tlm_server.txt'
        if os.path.isfile(fl_loc):
            names = self.read_for_token(fl_loc, self.token, ignored_lines)
            if CosmosUtil.VERBOSE:
                print("Cmd Tlm Server Config Altered")
        else:
            if CosmosUtil.VERBOSE:
                print("Cmd Tlm Server Config Created")
                
        for line in ignored_lines:
            names.append(line.split(" ")[1])
        
        # Open file
        fl = open(fl_loc, "w")
         
        # Initialize and fill Cheetah template
        sc = Config_Server.Config_Server()
         
        sc.date = CheetahUtil.DATE
        sc.user = CheetahUtil.USER
        sc.names = names
                     
        msg = sc.__str__()
                     
        fl.writelines(msg)
        fl.close()
