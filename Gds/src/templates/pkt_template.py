'''
@brief Packet Template class

Instances of this class describe a telemetry packet. For example an instance may
describe the packet with ID 5 and channels A, B, and C in that order.

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

import data_template
import ch_template


class PacketTemplate(data_template.DataTemplate):
    '''Class to create packet templates to describe specific packet types'''

    def __init__(self, pkt_id, name, ch_temp_list):
        '''
        Constructor

        Args:
            pkt_id: The ID of the telemetry packet being described
            name: Packet name as a string
            ch_temp_list: List of ch_template objects describing the channels
                          included in the packer. The order of the list is the
                          order of the channels in the packet.
        '''
        # TODO is this check necessary
        if not type(ch_temp_list) == type(list()):
            raise TypeMismatchException(type(list()), type(ch_temp_list))

        for ch in ch_list:
            if not type(ch) == type(ch_template.ChTemplate):
                raise TypeMismatchException(type(ch_template.ChTemplate),
                                            type(ch))

        self.id      = pkt_id
        self.name    = name
        self.ch_list = ch_temp_list


    def get_ch_list(self):
        return self.ch_list

