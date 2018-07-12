'''
@brief Class to store a specific packet and associated readings

@date Created July 12, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

from sys_data import SysData
from models.serialize.time_type import TimeType

class PktData(SysData):
    '''Stores the data from a specific packet receive'''

    def __init__(self, pkt_chs, pkt_time, pkt_temp):
        '''
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
        '''
        self.chs = pkt_chs
        self.time = pkt_time
        self.template = pkt_temp


    def get_chs(self):
        return self.chs

    def get_time(self):
        return self.time

    def get_template(self):
        return self.template


    def __str__(self):
        '''
        Convert the pkt data to a human readable string

        Returns:
            String version of the packet data
        '''
        pkt_str = "%s: %s (%d) {\n"%(self.time.to_readable(),
                                     self.template.get_name(),
                                     self.template.get_id())

        for ch in self.chs:
            pkt_str = pkt_str + "\t" + str(ch) + "\n"

        pkt_str = pkt_str + "}"

        return pkt_str


