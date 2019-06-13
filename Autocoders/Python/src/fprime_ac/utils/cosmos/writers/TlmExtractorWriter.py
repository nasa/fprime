#!/bin/env python
#===============================================================================
# NAME: TlmExtractorWriter.py
#
# DESCRIPTION: This writer generates each of the files that the user may use within
#    the tlm extractor application.  They each contain only one tlm
#    packet item.  Users should pick multiple generated files within the app
#    if they want to create an extracted tlm text file with multiple
#    arguments.
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

from fprime_ac.utils.cosmos.templates import Tlm_Extractor

from fprime_ac.utils.cosmos.util import CheetahUtil
from fprime_ac.utils.cosmos.util import CosmosUtil

class TlmExtractorWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates each of the files that the user may use within
    the tlm extractor application.  They each contain only one tlm
    packet item.  Users should pick multiple generated files within the app
    if they want to create an extracted tlm text file with multiple
    arguments.
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(TlmExtractorWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        
        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/tools/tlm_extractor/"
    
    def remove_files(self):
        """
        Removes all the config files in the tlm_extractor directory that have the 
        deployment name prefix to be removed.
        """
        temp = []
        for fl in os.listdir(self.destination):
            if len(fl) > len(self.deployment_name) and fl[:len(self.deployment_name)] == self.deployment_name.lower():
                os.remove(self.destination + fl)
        print("Removed Tlm Extractor files")
                    
    def write(self):
        """
        Generates the file
        """
        
        # Generate channel files
        for ch in self.cmd_tlm_data[0]:
            fl_name = self.destination + self.deployment_name.lower() + '_ch_' + ch.get_ch_name().lower() + "_value.txt"
            
            if not os.path.isfile(fl_name):
                fl = open(fl_name, "w")
            
                tmpl = Tlm_Extractor.Tlm_Extractor()
            
                # Set all values
                tmpl.target_name = self.deployment_name.upper()
                tmpl.packet_name = ch.get_ch_name()
                tmpl.item_name = 'VALUE'
                
                tmpl.date = CheetahUtil.DATE
                tmpl.user = CheetahUtil.USER
                tmpl.source = ch.get_source()
                
                msg = tmpl.__str__()
                
                fl.writelines(msg)
                fl.close()
            
        # Generate event files    
        for evr in self.cmd_tlm_data[2]:
            for item in evr.get_evr_items():
                fl_name = self.destination + self.deployment_name.lower() + '_evr_' + evr.get_evr_name().lower() + "_value.txt"
            
                if not os.path.isfile(fl_name):
                    fl = open(fl_name, "w")
            
                    tmpl = Tlm_Extractor.Tlm_Extractor()
            
                    # Set all values
                    tmpl.target_name = self.deployment_name.upper()
                    tmpl.packet_name = evr.get_evr_name()
                    tmpl.item_name = item.name
                    
                    tmpl.date = CheetahUtil.DATE
                    tmpl.user = CheetahUtil.USER
                    tmpl.source = evr.get_source()
                
                    msg = tmpl.__str__()
                
                    fl.writelines(msg)
                    fl.close()
        
        if CosmosUtil.VERBOSE:
            print("Tlm Extractor config files created")
