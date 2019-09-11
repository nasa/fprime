#!/bin/env python
#===============================================================================
# NAME: ConfigConfig_SystemWriter.py
#
# DESCRIPTION: This writer generates the system.txt in COSMOS/config/
# system/ folder that defines the existing targets for the entire cosmos
# system
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import os
import re

from fprime_ac.utils.cosmos.writers import BaseConfigWriter

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

from fprime_ac.utils.cosmos.templates import Config_System

class ConfigSystemWriter(BaseConfigWriter.BaseConfigWriter):
    """
    This class generates the system config file in
    cosmos_directory/config/system/
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory, old_definition=None):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        @param old_definition: COSMOS target name that you want to remove
        """
        super(ConfigSystemWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory, old_definition)
        self.token = "DECLARE_TARGET"
        self.argument= ""
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/system/"
        self.fl_loc = self.destination + 'system.txt'
                    
    def write(self):
        """
        Generates the file
        """
        # Add target to list of lines that will always be written
        ignored_lines = []
        ignored_lines.append((self.token + " SYSTEM"))
        if self.deployment_name and not self.deployment_name == "":
            ignored_lines.append(self.token + " " + self.deployment_name.upper())        
        
        # Open file for reading if exists already and parse all old targets
        names = []
        if os.path.isfile(self.fl_loc):
            names = self.read_for_token(self.fl_loc, self.token, ignored_lines)
            if CosmosUtil.VERBOSE:
                print("Config_System.txt Altered")
        else:
            if CosmosUtil.VERBOSE:
                print("Config_System.txt Created")
            
        for line in ignored_lines:
            names.append(line.split(" ")[1] + self.argument)
        
        # Open file
        fl = open(self.fl_loc, "w")
        
        # Initialize and fill Cheetah template 
        s = Config_System.Config_System()
         
        s.date = CheetahUtil.DATE
        s.user = CheetahUtil.USER
        s.names = names
                     
        msg = s.__str__()
                     
        fl.writelines(msg)
        fl.close()

    def find_subtargets(self):
        """
        Finds subtarget definitions of self.old_definition i.e. DECLARE_TARGET INST INST2 where INST2 is a subtarget
        and INST is the self.old_definition
        @return: List of subtargets
        """
        
        fl = open(self.fl_loc, "r")
        subtargets = []
        lines = re.findall(".*" + self.token + " .*", fl.read())
        for line in lines:
            line = line.strip()
            potential_subtarget = ""
            if len(line.split(" ")) > 2:
                potential_subtarget = line.split(" ")[2] # First word after DECLARE_TARGET TARGET_NAME
            if not potential_subtarget == "" and not line[0][0] == '#' and line[:len(self.token)] == self.token and not potential_subtarget[0] == '#':
                if line.split(" ")[1] == self.old_definition:
                    subtargets.append(potential_subtarget)
                    print("Subtarget " + potential_subtarget + " of " + self.old_definition + " Found")
        
        fl.close()        
        return subtargets
