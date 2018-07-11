'''
@brief Class to store a specific channel telemetry reading

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

import sys_data
from serialize import time_type

class ChData(sys_data.SysData):
    '''
    The ChData class stores a specific channel telemetry reading.
    '''

    def __init__(self, ch_val_obj, ch_time, ch_temp):
        '''
        Constructor.

        Args:
            ch_val_obj: The channel's value at the given time. Should be an
                        instance of a class derived from the BaseType class
                        (with a deserialized data value)
            ch_temp: Channel template instance for this channel
            ch_time: Time the reading was made

        Returns:
            An initialized ChData object
        '''
        self.id = ch_temp.get_id()
        self.val_obj = ch_val_obj
        self.time = ch_time
        self.template = ch_temp

    def get_val(self):
        '''
        Return the channel value

        Returns:
            The channel reading
        '''
        return self.val


    def __str__(self):
        '''
        Convert the ch data to a string

        Returns:
            String version of the tlm data
        '''
        time_str_nice = self.time.to_readable()
        time_str = str(self.time)
        ch_name = self.template.get_name()
        fmt_str = self.__dict_entry.get_format_str()
        if (fmt_str):
            ch_val = fmt_str%(self.val_obj.val)
        else:
            ch_val = str(self.val_obj.val)

        return ("%s: %s %d %s %s"%(time_str_nice, ch_name, self.id, time_str,
                                   ch_val))

