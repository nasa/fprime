#!/bin/env python
#===============================================================================
# NAME: ServerWriter.py
#
# DESCRIPTION: This writer generates the cmd_tlm_server.txt file in COSMOS/config/targets
# /DEPLOYMENT_NAME/ directory that contains configuration data for the cosmos
# target interface.
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
from fprime_ac.utils.cosmos.util import CosmosConfigManager

from fprime_ac.utils.cosmos.templates import Server

class ServerWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates the server definition file in
    cosmos_directory/config/targets/deployment_name.upper()/
    """

    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(ServerWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)

        # Initialize writer-unique file destination location
        self.destination = cosmos_directory + "/config/targets/" + deployment_name.upper() + "/"


    def write(self):
        """
        Generates the file
        """
        # Open file
        fl = open(self.destination + "cmd_tlm_server.txt", "w")
        if CosmosUtil.VERBOSE:
            print("Server Interface File Created")

        # Initialize and fill cheetah template
        cs = Server.Server()

        conf = CosmosConfigManager.CosmosConfigManager.getInstance(self.deployment_name)

        cs.date = CheetahUtil.DATE
        cs.user = CheetahUtil.USER
        cs.target_name = self.deployment_name.upper()
        cs.write_port = conf.get('deployment' , 'write_port')
        cs.read_port = conf.get('deployment' , 'read_port')
        cs.read_timeout = conf.get('deployment' , 'read_timeout')
        cs.write_timeout = conf.get('deployment' , 'write_timeout')
        cs.protocol_name_w = conf.get('deployment' , 'protocol_name_w')
        cs.protocol_name_r = conf.get('deployment' , 'protocol_name_r')
        cs.len_bit_offset_w = conf.get('deployment' , 'len_bit_offset_w')
        cs.len_bit_offset_r = conf.get('deployment' , 'len_bit_offset_r')
        cs.len_bit_size_w = conf.get('deployment' , 'len_bit_size_w')
        cs.len_bit_size_r = conf.get('deployment' , 'len_bit_size_r')
        cs.len_val_offset_w = conf.get('deployment' , 'len_val_offset_w')
        cs.len_val_offset_r = conf.get('deployment' , 'len_val_offset_r')
        cs.bytes_per_count_w = conf.get('deployment' , 'bytes_per_count_w')
        cs.bytes_per_count_r = conf.get('deployment' , 'bytes_per_count_r')
        cs.endianness_w = conf.get('deployment' , 'endianness_w')
        cs.endianness_r = conf.get('deployment' , 'endianness_r')
        cs.discard_leading_w = conf.get('deployment' , 'discard_leading_w')
        cs.discard_leading_r = conf.get('deployment' , 'discard_leading_r')
        cs.sync_w = conf.get('deployment' , 'sync_w')
        cs.sync_r = conf.get('deployment' , 'sync_r')
        cs.has_max_length_w = conf.get('deployment' , 'has_max_length_w')
        cs.has_max_length_r = conf.get('deployment' , 'has_max_length_r')
        cs.fill_ls_w = conf.get('deployment' , 'fill_ls_w')
        cs.fill_ls_r = conf.get('deployment' , 'fill_ls_r')
        use_router = conf.get('deployment', 'use_router')
        if use_router == "True":
            cs.use_router = True
        else:
            cs.use_router = False 
        cs.router_read_port = conf.get('deployment', 'router_read_port')
        cs.router_write_port = conf.get('deployment', 'router_write_port')

        msg = cs.__str__()

        fl.writelines(msg)
        fl.close()
