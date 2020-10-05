"""
@brief Class to store a specific channel telemetry reading

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime.common.models.serialize import time_type
from fprime_gds.common.data_types import sys_data


class ChData(sys_data.SysData):
    """
    The ChData class stores a specific channel telemetry reading.
    """

    def __init__(self, ch_val_obj, ch_time, ch_temp):
        """
        Constructor.

        Args:
            ch_val_obj: The channel's value at the given time. Should be an
                        instance of a class derived from the BaseType class
                        (with a deserialized data value) or None
            ch_temp: Channel template instance for this channel
            ch_time: Time the reading was made

        Returns:
            An initialized ChData object
        """
        super().__init__()
        self.id = ch_temp.get_id()
        self.val_obj = ch_val_obj
        self.time = ch_time
        self.template = ch_temp
        self.pkt = None

    @staticmethod
    def get_empty_obj(ch_temp):
        """
        Obtains a channel object that is empty (has a value of None)

        Args:
            ch_temp: (ChTemplate object) Template describing the channel

        Returns:
            A ChData Object with ch value of None
        """
        return ChData(None, time_type.TimeType(), ch_temp)

    def set_pkt(self, pkt):
        """
        Set the packet object to which this channel belongs (can be None)

        Args:
            pkt: The packet object to which these channels were transmitted in
        """
        self.pkt = pkt

    def get_pkt(self):
        """
        Return the packet object to which this channel belongs (could be None)

        Returns:
            The channel's packet
        """
        return self.pkt

    def get_val(self):
        """
        Return the channel value

        Returns:
            The channel reading
        """
        if self.val_obj is None:
            return None
        else:
            return self.val_obj.val

    def get_val_obj(self):
        """
        Return the channel's value object

        Returns:
            The channel's value object containing the value (obj of a type
            inherited from TypeBase
        """
        return self.val_obj

    @staticmethod
    def get_csv_header(verbose=False):
        """
        Get the header for a csv file containing channel data

        Args:
            verbose: (boolean, default=False) Indicates if header should be for
                                              regular or verbose output

        Returns:
            Header for a csv file containing channel data
        """
        if verbose:
            return "Time,Raw Time,Name,ID,Value\n"
        else:
            return "Time,Name,Value\n"

    def get_str(self, time_zone=None, verbose=False, csv=False):
        """
        Convert the channel data to a string

        Args:
            time_zone: (tzinfo, default=None) Timezone to print time in. If
                      time_zone=None, use local time.
            verbose: (boolean, default=False) Prints extra fields if True
            csv: (boolean, default=False) Prints each field with commas between
                                          if true

        Returns:
            String version of the channel data
        """
        time_str_nice = self.time.to_readable(time_zone)
        raw_time_str = str(self.time)
        ch_name = self.template.get_full_name()
        fmt_str = self.template.get_format_str()
        if self.val_obj is None:
            ch_val = "EMPTY CH OBJ"
        elif fmt_str:
            ch_val = fmt_str % (self.val_obj.val)
        else:
            ch_val = str(self.val_obj.val)

        if verbose and csv:
            return "%s,%s,%s,%d,%s" % (
                time_str_nice,
                raw_time_str,
                ch_name,
                self.id,
                ch_val,
            )
        elif verbose and not csv:
            return "%s: %s (%d) %s %s" % (
                time_str_nice,
                ch_name,
                self.id,
                raw_time_str,
                ch_val,
            )
        elif not verbose and csv:
            return "{},{},{}".format(time_str_nice, ch_name, ch_val)
        else:
            return "{}: {} = {}".format(time_str_nice, ch_name, ch_val)

    def get_val_str(self):

        """
        Convert the value to a string, using the format specifier if provided
        """
        fmt_str = self.template.get_format_str()
        if self.val_obj is None:
            return ""
        elif fmt_str:
            return fmt_str % (self.val_obj.val)
        else:
            return str(self.val_obj.val)

    def __str__(self):
        """
        Convert the ch data to a string

        Returns:
            String version of the channel data
        """
        return self.get_str()
