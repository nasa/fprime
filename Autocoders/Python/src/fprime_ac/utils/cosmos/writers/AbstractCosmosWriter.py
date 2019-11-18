#!/bin/env python
#===============================================================================
# NAME: AbstractConfigWriter.py
#
# DESCRIPTION: This is the abstract class inherited by only the writers who generate
# the files that reside inside of the COSMOS/config/targets/DEPLOYMENT_NAME/
# directory, and it contains the abstract write method and some shared fields
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

class AbstractCosmosWriter(object):
    """
    This abstract class defines the commonality between all autocoded
    COSMOS files as they require a write method and parser / target
    data.
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels, commands, and events
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        self.cmd_tlm_data = cmd_tlm_data
        self.deployment_name = deployment_name
        self.cosmos_directory = cosmos_directory
        
    def removekey(self, ls, key):
        """
        Helper method that simply returns
        @param ls: Dictionary to have a key removed
        @param key: Key to remove
        @return: Dict with argument removed
        """
        temp = dict(ls)
        del temp[key]
        return temp
    
    def write(self):
        """
        Method defined by inheriting class that writes COSMOS data into a specific file 
        in the fprime/COSMOS/ directory tree
        """
        raise '# CosmosWriter.write() - Implementation Error: you must supply your own concrete implementation.'