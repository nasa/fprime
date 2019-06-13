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

from fprime_ac.utils.cosmos.templates import Ruby_Evr_Dump_Component
from fprime_ac.utils.cosmos.templates import Ruby_Multi_String_Tlm_Item_Conversion
from fprime_ac.utils.cosmos.templates import Ruby_FPrime_Protocol

from fprime_ac.utils.cosmos.util import CosmosUtil

class RubyWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates each of the Ruby scripts that the user must have within the lib directory:
    evr_dump_component.rb: Plain text writing protocol for data_viewer application EVR view
    multi_string_tlm_item_conversion.rb: Parses raw hex data for packets with multiple variable-
        length strings because COSMOS does not have this feature
    ref_protocol.rb: Helper script for Fprime to bypass a warning message in cmd_tlm_server
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(RubyWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        
        # Initialize writer-unique file destination location
        self.destinations = {
            cosmos_directory + "/lib/evr_dump_component.rb": Ruby_Evr_Dump_Component.Ruby_Evr_Dump_Component(),
            cosmos_directory + "/lib/multi_string_tlm_item_conversion.rb": Ruby_Multi_String_Tlm_Item_Conversion.Ruby_Multi_String_Tlm_Item_Conversion(),
            cosmos_directory + "/lib/fprime_protocol.rb": Ruby_FPrime_Protocol.Ruby_FPrime_Protocol()
            }
        
                    
    def write(self):
        """
        Generates the file
        """
        for destination in self.destinations.keys():
            fl = open(destination, "w")
                
            # All Ruby Cheetah template files do not contain variables, so can just be spit back out
            tmpl = self.destinations[destination]
                
            msg = tmpl.__str__()
                
            fl.writelines(msg)
            fl.close()
                
            if CosmosUtil.VERBOSE:
                print("Ruby script " + destination + " Created")
