#!/bin/env python
#===============================================================================
# NAME: TargetWriter.py
#
# DESCRIPTION: This writer generates the target.txt file in COSMOS/config/targets
# /DEPLOYMENT_NAME/ directory that contains configuration data for the cosmos
# target.
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

from fprime_ac.utils.cosmos.templates import Target

class TargetWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates the target definition file in
    cosmos_directory/config/targets/deployment_name.upper()/
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(TargetWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/targets/" + deployment_name.upper() + "/"
        
                    
    def write(self):
        """
        Generates the file
        """
        # Open file
        fl = open(self.destination + "target.txt", "w")
        if CosmosUtil.VERBOSE:
            print("target.txt Created")
        
        # Initialize and fill cheetah template 
        t = Target.Target()
         
        t.date = CheetahUtil.DATE
        t.user = CheetahUtil.USER
                     
        msg = t.__str__()
                     
        fl.writelines(msg)
        fl.close()
