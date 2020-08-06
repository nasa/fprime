"""
@brief Packet Template class

Instances of this class describe a telemetry packet. For example an instance may
describe the packet with ID 5 and channels A, B, and C in that order.

@date Created July 2, 2018
@date Modified July 12, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime.common.models.serialize.type_exceptions import TypeMismatchException

from . import ch_template, data_template


class PktTemplate(data_template.DataTemplate):
    """Class to create packet templates to describe specific packet types"""

    def __init__(self, pkt_id, pkt_name, ch_temp_list):
        """
        Constructor

        Args:
            pkt_id (int): The ID of the telemetry packet being described
            pkt_name (str): Packet name
            ch_temp_list (ch_template list): List of ch_template objects
                         describing the channels included in the packer. The
                         order of the list is the order of the channels in the
                         packet.
        """
        super().__init__()
        if not isinstance(pkt_id, int):
            raise TypeMismatchException(int, type(pkt_id))

        if not isinstance(pkt_name, str):
            raise TypeMismatchException(str, type(pkt_name))

        if not isinstance(ch_temp_list, list):
            raise TypeMismatchException(list, type(ch_temp_list))

        for ch in ch_temp_list:
            if not isinstance(ch, ch_template.ChTemplate):
                raise TypeMismatchException(type(ch_template.ChTemplate), type(ch))

        self.id = pkt_id
        self.name = pkt_name
        self.ch_list = ch_temp_list

    def get_id(self):
        return self.id

    def get_name(self):
        return self.name

    def get_ch_list(self):
        return self.ch_list
