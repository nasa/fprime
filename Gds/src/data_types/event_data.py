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

    def __init__(self, event_args, event_time, event_temp):
        '''
        Constructor.

        Args:
            event_args: The arguments of the event being stored. This should
                        be a tuple where each element is an object of a class
                        derived from the BaseType class. Each element's class
                        should match the class of the corresponding argument
                        type object in the event_temp object.
            event_time: The time the event occured (TimeType)
            event_temp: Event template instance for this event

        Returns:
            An initialized EventData object
        '''
        # TODO refactor so that arguments are actually of BaseType objects
        self.id = event_temp.get_id()
        self.args = event_args
        self.time = event_time
        self.template = event_temp


    def get_args(self):
        return self.args


    def __str__(self):
        time_str = self.time.to_readable()
        name = self.template.get_name()
        format_str = self.template.get_format_str()

        # The arguments are currently serializable objects which cannot be
        # used to fill in a format string. Convert them to values that can be
        arg_val_list = [arg_obj.val for arg_obj in self.args]

        arg_str = format_str%tuple(arg_val_list)

        return ("%s: %s (%d) Severity.%s : %s"%(time_str, name, self.id,
                                                self.template.get_severity(),
                                                arg_str))

