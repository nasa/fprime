'''
@brief Class to store data from a specific event

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

from fprime_gds.common.data_types import sys_data
from fprime.common.models.serialize import time_type

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
                        derived from the BaseType class with a filled in value.
                        Each element's class should match the class of the
                        corresponding argument type object in the event_temp
                        object. This can be None.
            event_time: The time the event occurred (TimeType)
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
    
    def get_severity(self):
        return self.template.get_severity()

    @staticmethod
    def get_empty_obj(event_temp):
        '''
        Obtains an event object that is empty (arguments = None)

        Args:
            event_temp: (EventTemplate obj) Template describing event

        Returns:
            An EventData object with argument value of None
        '''
        return EventData(None, time_type.TimeType(), event_temp)

    @staticmethod
    def get_csv_header(verbose=False):
        '''
        Get the header for a csv file containing event data

        Args:
            verbose: (boolean, default=False) Indicates if header should be for
                                              regular or verbose output

        Returns:
            String version of the channel data
        '''
        if verbose:
            return "Time,Raw Time,Name,ID,Severity,Args\n"
        else:
            return "Time,Name,Severity,Args\n"


    def get_str(self, time_zone=None, verbose=False, csv=False):
        '''
        Convert the event data to a string

        Args:
            time_zone: (tzinfo, default=None) Timezone to print time in. If
                      time_zone=None, use local time.
            verbose: (boolean, default=False) Prints extra fields if True
            csv: (boolean, default=False) Prints each field with commas between
                                          if true

        Returns:
            String version of the event data
        '''
        time_str = self.time.to_readable(time_zone)
        raw_time_str = str(self.time)
        name = self.template.get_name()
        severity = self.template.get_severity()
        format_str = self.template.get_format_str()

        if self.args == None:
            arg_str = "EMPTY EVENT OBJ"
        else:
            # The arguments are currently serializable objects which cannot be
            # used to fill in a format string. Convert them to values that can be
            arg_val_list = [arg_obj.val for arg_obj in self.args]

            arg_str = format_str%tuple(arg_val_list)

        if verbose and csv:
            return ("%s,%s,%s,%d,%s,%s"%(time_str, raw_time_str, name, self.id,
                                         severity, arg_str))
        elif verbose and not csv:
            return ("%s: %s (%d) %s %s : %s"%(time_str, name, self.id,
                                              raw_time_str, severity, arg_str))
        elif not verbose and csv:
            return ("%s,%s,%s,%s"%(time_str, name, severity, arg_str))
        else:
            return ("%s: %s %s : %s"%(time_str, name, severity, arg_str))


    def __str__(self):
        '''
        Convert the event data to a string

        Returns:
            String version of the channel data
        '''
        return self.get_str()

