#!/bin/env python
#===============================================================================
# NAME: AbstractDictWriter.py
#
# DESCRIPTION: This is the abstract class inherited only by the writers who
# generate GDS Python dict modules within the dictgen tool.
#
# AUTHOR: Jordan Ishii
# EMAIL:  jordan.ishii@jpl.nasa.gov
# DATE CREATED: June 18, 2019
#
# Copyright 2018, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

class AbstractDictWriter(object):
    """
        This abstract class defines the commonality between all autocoded
        dictgen files.
        """
    
    def __init__(self, model):
        """
            @param cmd_tlm_data: Model containing all relevant data for template
            """
        self.model = model
    
    def write(self):
        """
            Method defined by inheriting class that writes dictionary data into a specific file
            """
        raise '# DictWriter.write() - Implementation Error: you must supply your own concrete implementation.'
