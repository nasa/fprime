'''
@brief Class to store data from a specific event

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

import sys_data
from models.serialize import time_type

class EventData(sys_data.SysData):
    '''
    The EventData class stores a specific event message.
    '''

    def __init__(self, event_args, event_time, event_template):
        '''
        Constructor.

        Args:
            event_args: The arguments of the event being stored. Should match
                        the types of the arguments in the event_temp object.
                        Should be a tuple.
            event_time: The time the event occured (TimeType)
            event_template: Event template instance for this event

        Returns:
            An initialized EventData object
        '''
        self.id = ch_temp.get_id()
        self.args = event_args
        self.time = event_time
        self.template = event_template


    def get_args(self):
        return self.args


    def __str__(self):
        time_str = self.time.to_readable()
        name = self.template.get_name()
        format_str = self.template.get_format_str()
        arg_str = format_str%self.args

        return ("%s: %s (%d) Severity.%s : %s"%(time_str, name, self.id,
                                                self.template.get_severity(),
                                                arg_str))

