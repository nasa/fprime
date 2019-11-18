#!/bin/env python
#===============================================================================
# NAME: CommandWriter.py
#
# DESCRIPTION: This writer generates the command files in the COSMOS/config/targets
# /DEPLOYMENT_NAME/cmd_tlm/commands/ directory that contains configuration data for each
# command.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
import sys

from fprime_ac.utils.cosmos.writers import AbstractCosmosWriter

from fprime_ac.utils.cosmos.templates import Command

from fprime_ac.utils.cosmos.util import CosmosUtil
from fprime_ac.utils.cosmos.util import CheetahUtil

class CommandWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This class generates the command definition files in
    cosmos_directory/config/targets/deployment_name.upper()/cmd_tlm/commands/
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory):
        """
        @param cmd_tlm_data: Tuple containing lists channels [0], commands [1], and events [2]
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        """
        super(CommandWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        self.repeated_names = {}
    
        # Initialize writer-unique file destination location
        self.cosmos_directory = cosmos_directory
        self.destination = cosmos_directory + "/config/targets/" + deployment_name.upper() + "/cmd_tlm/commands/"
    
    def write(self):
        """
        Generates the file
        """
        if CosmosUtil.VERBOSE:
            print("Creating Command Files")
        command_templates = {}
        for cmd in self.cmd_tlm_data[1]:
            n = cmd.get_cmd_name()
            if n in self.repeated_names.keys():
                # Fix other name pair
                if n in command_templates.keys():
                    command_templates.get(n).cmd_name = self.repeated_names.get(n).get_comp_name() + "_" + n
                    command_templates.update({self.repeated_names.get(n).get_comp_name() + "_" + 
                                        n: command_templates.get(n)})
                    command_templates = self.removekey(command_templates, n)
                n = cmd.get_comp_name() + "_" + n
            self.repeated_names.update({n: cmd})

            # Initialize and fill Cheetah Template
            c = Command.Command()

            c.date = CheetahUtil.DATE
            c.user = CheetahUtil.USER
            c.source = cmd.get_source()
            c.component_string = cmd.get_component_string()
            c.cmd_name = n
            c.endianness = CosmosUtil.CMD_TLM_ENDIANNESS
            c.cmd_desc = cmd.get_cmd_desc()
            c.opcode = cmd.get_opcode()
            c.target_caps = self.deployment_name.upper()
            c.target_lower = self.deployment_name.lower()
            c.cmd_items = CheetahUtil.cmd_convert_items_to_cheetah_list(cmd.get_cmd_items())
            c.priority = cmd.get_priority()
            c.sync = cmd.get_sync()
            c.full = cmd.get_full()

            command_templates.update({n: c})

        # Write files
        if sys.version_info >= (3,):
            for name, c in command_templates.items():
                c.cmd_name = name
                fl = open(self.destination + name.lower() + ".txt", "w")
                if CosmosUtil.VERBOSE:
                    print("Command " + name + " Created")
                c.cmd_name = name.upper()
                msg = c.__str__()
                    
                fl.writelines(msg)
                fl.close()
        else:
            for name, c in command_templates.iteritems():
                c.cmd_name = name
                fl = open(self.destination + name.lower() + ".txt", "w")
                if CosmosUtil.VERBOSE:
                    print("Command " + name + " Created")
                c.cmd_name = name.upper()
                msg = c.__str__()

                fl.writelines(msg)
                fl.close()
