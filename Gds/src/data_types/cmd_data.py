'''
@brief Command Data class

Instances of this class define a specific instance of a command with specific
argument values.

@data Created July 3, 2018
@author Josef Biberstein

@bug No known bugs
'''

import sys_data
from enum import Enum
from copy import deepcopy


class CmdData(sys_data.SysData):
    '''The CmdData class stores a specific command'''

    def __init__(self, cmd_args, cmd_temp, cmd_time=None):
        '''
        Constructor.

        Args:
            cmd_args: The arguments for the event. Should match the types of the
                      arguments in the cmd_temp object. Should be a tuple.
            cmd_temp: Command Template instance for this command (this provides
                      the opcode and argument types are stored)
            cmd_time: The time the event should occur. This is for sequences.
                      Should be a TimeType object with time base=TB_DONT_CARE

        Returns:
            An initialized CmdData object
        '''
        self.template = cmd_template
        self.args = cmd_args

        self.arg_types = [deepcopy(c) for c in self.template.arguments]

        if (cmd_time):
            self.time = cmd_time
        else:
            self.time = time_type.TimeType(time_type.TimeBase["TB_DONT_CARE"])

    def get_args(self):
        return self.command_args

    def serialize_args(self):
        data = b''
        for val, typ in zip(self.args, self.arg_types):
            typ.val(val)
            data += typ.serialize()
        return data

    def __str__(self):
        arg_str = ''
        for (arg, _, arg_value) in self.args:
            arg_str += ('   Arg %s with value %s\n')% arg, str(arg_value.val)

        arg_info = 'Command mneumonic %s\n'% self.template.mneumonic

        return arg_info + arg_str


