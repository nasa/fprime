"""
@brief Loader class for importing xml based packet dictionaries

@date Created July 12, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime_gds.common.data_types import exceptions

# Custom python modules
from fprime_gds.common.loaders.xml_loader import XmlLoader
from fprime_gds.common.templates.pkt_template import PktTemplate


class PktXmlLoader(XmlLoader):
    """Class to load xml packet dictionaries"""

    # Constants for use when parsing the xml
    PKT_LIST_TAG = "packet_list"  # tag on the root list of packets
    PKT_TAG = "packet"  # tag on each packet object
    CH_TAG = "channel"  # tag on each channel element of the packet
    NAME_FIELD = "name"  # key to retrieve the packet name
    ID_FIELD = "id"  # key to retrieve the packet id
    CH_NAME_FIELD = "name"  # key to retrieve the name from each ch elem

    def get_id_dict(self, path, ch_name_dict):
        """
        Returns the python dictionary keyed by ids for the given path

        This function will return the same dictionary originally computed for
        the given path or will construct new dictionaries if the path has never
        been passed to the get_id_dict or the get_name_dict functions.

        Args:
            path (string): Path to the xml pkt spec to convert to a python dict
            ch_name_dict (dict()): Channel dictionary with names as keys and
                                   ChTemplate objects as values.

        Returns:
            The id dictionary associated with the given path
        """
        if path in self.saved_dicts:
            (id_dict, name_dict) = self.saved_dicts[path]
        else:
            (id_dict, name_dict) = self.construct_dicts(path, ch_name_dict)
            self.saved_dicts[path] = (id_dict, name_dict)

        return id_dict

    def get_name_dict(self, path, ch_name_dict):
        """
        Returns the python dictionary keyed by names for the given path

        This function will return the same dictionary originally computed for
        the given path or will construct new dictionaries if the path has never
        been passed to the get_id_dict or the get_name_dict functions.

        Args:
            path (string): Path to the xml pkt spec to convert to a python dict
            ch_name_dict (dict()): Channel dictionary with names as keys and
                                   ChTemplate objects as values.

        Returns:
            The name dictionary associated with the given path
        """
        if path in self.saved_dicts:
            (id_dict, name_dict) = self.saved_dicts[path]
        else:
            (id_dict, name_dict) = self.construct_dicts(path, ch_name_dict)
            self.saved_dicts[path] = (id_dict, name_dict)

        return name_dict

    def construct_dicts(self, path, ch_name_dict):
        """
        Constructs and returns python dictionaries keyed on id and name

        This function should not be called directly, instead, use
        get_id_dict(path) and get_name_dict(path)

        Args:
            path (string): Path to the packet definition xml file to parse into
                           a dictionary.
            ch_name_dict (dict()): Channel dictionary with names as keys and
                                   ChTemplate objects as values.

        Returns:
            A tuple with two packet dictionaries (type==dict()):
            (id_dict, name_dict). The keys should be the packets' id and name
            fields respectively and the values should be PktTemplate objects.
        """
        packet_list = self.get_xml_tree(path)
        if packet_list.tag != self.PKT_LIST_TAG:
            raise exceptions.GseControllerParsingException(
                "expected packet list to have tag %s, but found %s"
                % (self.PKT_LIST_TAG, packet_list.tag)
            )

        id_dict = dict()
        name_dict = dict()

        for packet in packet_list:
            # check if this is actually a packet, and not something to ignore
            if packet.tag != self.PKT_TAG:
                continue

            pkt_name = packet.attrib[self.NAME_FIELD]
            pkt_id = int(packet.attrib[self.ID_FIELD])

            ch_list = []
            for ch in packet:
                ch_name = ch.attrib[self.CH_NAME_FIELD]

                if ch_name not in ch_name_dict:
                    raise exceptions.GseControllerParsingException(
                        "Channel %s in pkt %s, but cannot be found in channel dictionary"
                        % (ch_name, pkt_name)
                    )

                ch_list.append(ch_name_dict[ch_name])

            pkt_temp = PktTemplate(pkt_id, pkt_name, ch_list)

            id_dict[pkt_id] = pkt_temp
            name_dict[pkt_name] = pkt_temp

        return id_dict, name_dict
