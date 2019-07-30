#!/bin/env python
#===============================================================================
# NAME: BaseConfigWriter.py
#
# DESCRIPTION: This is the abstract class inherited by only the writers who generate
# the files that reside outside of the COSMOS/config/targets/DEPLOYMENT_NAME/
# as they require the ability to append rather than just creating / deleting.
# These classes all begin with the prefix "Config"
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 6, 2018
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import re

from fprime_ac.utils.cosmos.writers import AbstractCosmosWriter

class BaseConfigWriter(AbstractCosmosWriter.AbstractCosmosWriter):
    """
    This abstract class defines the commonality between all files outside
    of the COSMOS/config/targets/TARGET_NAME/ directories as they must be
    able to append / remove target definitions as opposed to just rewriting
    the entire file.
    """
    
    def __init__(self, cmd_tlm_data, deployment_name, cosmos_directory, old_definition):
        """
        @param cmd_tlm_data: Tuple containing lists channels, commands, and events
        @param deployment_name: name of the COSMOS target
        @param cosmos_directory: Directory of COSMOS
        @param old_definition: COSMOS target name that you want to remove
        """
        super(BaseConfigWriter, self).__init__(cmd_tlm_data, deployment_name, cosmos_directory)
        self.old_definition = old_definition
        
    def read_for_token(self, fl_loc, token, ignored_lines):
        """
        Reads a file and locates all lines that begin with a token (COSMOS keyword) so that
        the deployment names following the token can be parsed and then re-coded back into the file
        alongside the new information
        @param fl_loc: The file directory to be read
        @param token: Token (COSMOS keyword) to look for at the beginning of every line
        @param ignored_lines: Words that should not be printed out because they are already a part of tmpl
        @return: List of deployment names (plus possibly inline comments) to be re-coded
        """
        fl = open(fl_loc, "r")
        names = []
        lines = re.findall(".*" + token + " .*", fl.read())
        bad_lines = []
        for line in lines:
            line = line.strip()
            if line[0][0] == '#' or not line[:len(token)] == token or " ".join(line.split(" ")[0:2]) in ignored_lines:
                bad_lines.append(line)
                    
        for line in bad_lines:
            lines.remove(line)
                    
        for line in lines:
            line = line.split(" ")
            if not self.old_definition or not line[1] == self.old_definition:
                names.append(" ".join(line[1:]))
                            
        fl.close()
        
        return names
