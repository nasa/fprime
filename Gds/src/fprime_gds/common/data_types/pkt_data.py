"""
@brief Class to store a specific packet and associated readings

@date Created July 12, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.sys_data import SysData


class PktData(SysData):
    """Stores the data from a specific packet receive"""

    def __init__(self, pkt_chs, pkt_time, pkt_temp):
        """
        Constructor.

        Args:
            pkt_chs:  A list of ChData objects. The ChData objects and their
                     order should match the channel order in the pkt_temp
                     object. Each ChData object has the reading for that channel
                     in the packet.
            pkt_time: The time the packet was received (as a TimeType object)
            pkt_temp: PktTemplate object that describes this packet

        Returns:
            An initialized PktData object
        """
        super().__init__()
        self.id = pkt_temp.get_id()
        self.chs = pkt_chs
        self.time = pkt_time
        self.template = pkt_temp

        # Set the packet of all the channels
        for ch in self.chs:
            ch.set_pkt(self)

    def get_chs(self):
        return self.chs

    def get_time(self):
        return self.time

    def get_template(self):
        return self.template

    @staticmethod
    def get_csv_header(verbose=False):
        """
        Get the header for a csv file containing packet data

        Args:
            verbose: (boolean, default=False) Indicates if header should be for
                                              regular or verbose output

        Returns:
            String version of the channel data
        """
        # For csv output, all channels are just printed without regards to
        # packet information
        return ChData.get_csv_header(verbose)

    def get_str(self, time_zone=None, verbose=False, csv=False):
        """
        Convert the packet data to a string

        Args:
            time_zone: (tzinfo, default=None) Timezone to print time in. If
                      time_zone=None, use local time.
            verbose: (boolean, default=False) Prints extra fields if True
            csv: (boolean, default=False) Prints each field with commas between
                                          if true

        Returns:
            String version of the packet data
        """
        pkt_str = ""

        if not csv and verbose:
            pkt_str += "%s: %s (%d) %s{\n" % (
                self.time.to_readable(time_zone),
                self.template.get_name(),
                self.template.get_id(),
                str(self.time),
            )
        elif not csv and not verbose:
            pkt_str += "{}: {} {{\n".format(
                self.time.to_readable(time_zone),
                self.template.get_name(),
            )

        for i in range(len(self.chs)):
            ch = self.chs[i]

            if not csv:
                pkt_str += "\t"

            pkt_str += ch.get_str(time_zone, verbose, csv)

            # Only print newline if we have not just printed the last line
            if i < (len(self.chs) - 1):
                pkt_str += "\n"

        if not csv:
            pkt_str += "\n}"

        return pkt_str

    def __str__(self):
        """
        Convert the pkt data to a human readable string

        Returns:
            String version of the packet data
        """
        return self.get_str()
