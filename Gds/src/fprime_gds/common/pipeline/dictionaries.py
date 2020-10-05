"""
dictionaries.py:

Helps the standard pipeline wrangle dictionaries by encapsulating the functionality of dictionary loading into a single
class called "Dictionaries".

@author mstarch
"""
import os

import fprime_gds.common.loaders.ch_py_loader
import fprime_gds.common.loaders.ch_xml_loader

# Py Loaders
import fprime_gds.common.loaders.cmd_py_loader

# XML Loaders
import fprime_gds.common.loaders.cmd_xml_loader
import fprime_gds.common.loaders.event_py_loader
import fprime_gds.common.loaders.event_xml_loader


class Dictionaries:
    """
    Dictionaries class to encapsulate the many different dictionaries used in the system. This includes the following
    dictionary types:

    1. Command IDs to Commands
    2. Command names to Commands
    3. Channel IDs to Channels
    4. Channel names to Channels
    5. Event IDs to Events
    6. Event names to Events
    7. Packet IDs to Packets
    """

    def __init__(self):
        """ Constructor of the dictionaries object """
        self._command_id_dict = None
        self._event_id_dict = None
        self._channel_id_dict = None
        self._command_name_dict = None
        self._event_name_dict = None
        self._channel_name_dict = None
        self._packet_dict = None

    def load_dictionaries(self, dictionary, packet_spec):
        """
        Loads the dictionaries based on the dictionary path supplied. Optional packet_spec is allowed to specify the
        definitions of packets.

        :param dictionary: dictionary path used for loading dictionaries
        :param packet_spec: specification for packets, or None, for packetized telemetry
        """
        # Loading the dictionaries from a directory. A directory indicates heritage python dicts.
        if os.path.isdir(dictionary):
            # Events
            event_loader = fprime_gds.common.loaders.event_py_loader.EventPyLoader()
            self._event_id_dict = event_loader.get_id_dict(
                os.path.join(dictionary, "events")
            )
            self._event_name_dict = event_loader.get_name_dict(
                os.path.join(dictionary, "events")
            )
            # Commands
            command_loader = fprime_gds.common.loaders.cmd_py_loader.CmdPyLoader()
            self._command_id_dict = command_loader.get_id_dict(
                os.path.join(dictionary, "commands")
            )
            self._command_name_dict = command_loader.get_name_dict(
                os.path.join(dictionary, "commands")
            )
            # Channels
            channel_loader = fprime_gds.common.loaders.ch_py_loader.ChPyLoader()
            self._channel_id_dict = channel_loader.get_id_dict(
                os.path.join(dictionary, "channels")
            )
            self._channel_name_dict = channel_loader.get_name_dict(
                os.path.join(dictionary, "channels")
            )
        # XML dictionaries
        elif os.path.isfile(dictionary):
            # Events
            event_loader = fprime_gds.common.loaders.event_xml_loader.EventXmlLoader()
            self._event_id_dict = event_loader.get_id_dict(dictionary)
            self._event_name_dict = event_loader.get_name_dict(dictionary)
            # Commands
            command_loader = fprime_gds.common.loaders.cmd_xml_loader.CmdXmlLoader()
            self._command_id_dict = command_loader.get_id_dict(dictionary)
            self._command_name_dict = command_loader.get_name_dict(dictionary)
            # Channels
            channel_loader = fprime_gds.common.loaders.ch_xml_loader.ChXmlLoader()
            self._channel_id_dict = channel_loader.get_id_dict(dictionary)
            self._channel_name_dict = channel_loader.get_name_dict(dictionary)
        else:
            raise Exception(
                "[ERROR] Dictionary '{}' does not exist.".format(dictionary)
            )
        # Check for packet specification
        if packet_spec is not None:
            packet_loader = fprime_gds.common.loaders.pkt_xml_loader.PktXmlLoader()
            self._packet_dict = packet_loader.get_id_dict(
                packet_spec, self._channel_name_dict
            )
        else:
            self._packet_dict = None

    @property
    def command_id(self):
        """ Command dictionary by ID """
        return self._command_id_dict

    @property
    def event_id(self):
        """ Event dictionary by ID """
        return self._event_id_dict

    @property
    def channel_id(self):
        """ Channel dictionary by ID """
        return self._channel_id_dict

    @property
    def command_name(self):
        """ Command dictionary by name """
        return self._command_name_dict

    @property
    def event_name(self):
        """ Event dictionary by name """
        return self._event_name_dict

    @property
    def channel_name(self):
        """ Channel dictionary by name """
        return self._channel_name_dict

    @property
    def packet(self):
        """ Packet dictionary """
        return self._packet_dict
