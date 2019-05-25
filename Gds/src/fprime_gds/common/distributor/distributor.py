'''
@brief Class to parse raw messages and distribute them to registered objects

This class allows for other objects (namely Decoders) to register to receive
data messages of a given descriptor type. The data message with the length and
descriptor header will be passed on to the registered objects.

@date Created June 2018
@date Modified August 2, 2018

@author Josef Biberstein
@author Joseph Paetz

@bug No known bugs
'''
from __future__ import print_function

import sys

from fprime.common.models.serialize import u32_type
from fprime_gds.common.utils import data_desc_type
from fprime_gds.common.utils import config_manager


# NOTE decoder function to call is called data_callback(data)
class Distributor(object):
    """
    A distributor contains a socket client that connects to a ThreadedTCPServer.
    It then sends and recvs data from a FPrime deployment.
    Decoders can register with a distributor to recv packets of data of a certain description.
    """

    def __init__(self, config=None):
        """
        Sets up the dictionary of connected decoders and socket client object.

        Decoder dictionary is of the form:
        {data descriptor name: list of decoder objects registered for that data}

        Args:
            config (ConfigManager, default=None): Config manager with
                   information on what types the message fields are. If None,
                   defaults are used.
        """
        if config==None:
            # Retrieve defaults for the configs
            config = config_manager.ConfigManager()

        self.__decoders = {key.name: [] for key in list(data_desc_type.DataDescType)}

        # Internal buffer for un distributed data
        self.__buf = b""
        #Setup key framing
        self.key_frame = None
        try:
            tmp_frame = config.get("framing", "use_key")
        except:
            tmp_frame = "false"
        if (tmp_frame.lower() == "true"):
            self.key_frame = int(config.get("framing", "key_val"), 16)
        self.key_obj = config.get_type("key_val")
        self.len_obj = config.get_type("msg_len")


    # NOTE we could use either the type of the object or an enum as the type argument. It should indicate what the decoder decodes.
    # TODO implement as an ENUM name as key
    def register(self, typeof, obj):
        """
        Register a decoder with the distributor

        Arguments:
            typeof {string} -- The name of the data descriptor that the decoder will decode
            obj {decoder} -- The decoder object that will process the data
        """
        #TODO check that typeof is a valid DataDescType Enum value
        self.__decoders[typeof].append(obj)


    def parse_into_raw_msgs_api(self, data):
        """
        Parse the given data into raw messages.

        Raw messages include a length and descriptor header in front of the
        message data.

        Args:
            data (bytearray): Binary data to parse. First byte of data should be
                              the first byte of a valid raw message.

        Returns:
            (leftover_data, [raw_msg1, raw_msg2, ..., raw_msgN])
            Where leftover_data is a bytearray of anything at the end of data
            that could not be parsed (due to insufficient length).
        """
        data_left = data

        raw_msgs = []
        #Search data looking for key-frame
        if not self.key_frame is None:
            while True:
                # Check if we have enough data to parse a key
                # if not, bail on the function
                if (len(data_left) < self.key_obj.getSize()):
                    return (data_left, raw_msgs)
                #Check leading key size bytes to see if it is the key
                self.key_obj.deserialize(data_left, 0)
                if self.key_obj.val != self.key_frame:
                    data_left = data_left[1:]
                    continue
                #Key found break
                data_left = data_left[self.key_obj.getSize():]
                break

        # Keep parsing and then break when you can't parse no more
        while True:
            # Check if we have enough data to parse a length
            if (len(data_left) < self.len_obj.getSize()):
                break
            self.len_obj.deserialize(data_left, 0)

            expected_len = self.len_obj.val + self.len_obj.getSize()

            # Check if we have enough data to parse
            if (len(data_left) < expected_len):
                break

            raw_msgs.append(data_left[:expected_len])

            data_left = data_left[expected_len:]


        return (data_left, raw_msgs)


    def parse_raw_msg_api(self, raw_msg):
        """
        Parse the given raw message into its component parts

        Args:
            raw_msg (bytearray): Raw message to parse. Must be a valid raw msg

        Returns:
            Tuple: (length, descriptor, msg). length is type int. Descriptor is
            type int. Msg is a bytearray.
        """
        # Data Entry Structure
        #
        # +---------------------------+
        # | Length (n bytes)          |
        # +---------------------------+
        # | Descriptor Type (4 bytes) |      -
        # +---------------------------+      |
        # |                           |      |
        # | Message data...           |   Length
        # | ...                       |      |
        # | ..                        |      |
        #   .                                :

        offset = 0

        # Parse length
        self.len_obj.deserialize(raw_msg, offset)
        offset += self.len_obj.getSize()
        length = self.len_obj.val

        # Parse Descriptor type
        desc_obj = u32_type.U32Type()
        desc_obj.deserialize(raw_msg, offset)
        offset += desc_obj.getSize()
        desc = desc_obj.val

        # Retrieve message section
        msg = raw_msg[offset:]

        return (length, desc, msg)


    def on_recv(self, data):
        """
        Called by the internal socket client when data is recved from the socket
        client.

        Arguments:
            data {binary} -- the data recved from the socket client. May contain
                             more than one message.
        """
        # NOTE make data sizes selectable with a configuration later
        # TODO: Currently, the TCPServer sends the client just the raw bytes,
        #       without headers. This means that we don't have a good way to
        #       figure out where the head of the messages are when we decode
        #       them here. Ideally, the client would just be sending individual
        #       messages.


        # Add new data to end of buffer
        
        self.__buf = self.__buf + data

        (leftover_data, raw_msgs) = self.parse_into_raw_msgs_api(self.__buf)
        self.__buf = leftover_data

        for raw_msg in raw_msgs:
            (length, data_desc, msg) = self.parse_raw_msg_api(raw_msg)

            data_desc_key = data_desc_type.DataDescType(data_desc).name

            for d in self.__decoders[data_desc_key]:
                d.data_callback(msg)


if __name__ == "__main__":
    # Unit testing
    config = config_manager.ConfigManager()
    config.set('types', 'msg_len', 'U16')

    dist = Distributor(config)

    header_1 = "\x00\x0E\x00\x00\x00\x04"
    length_1 = 14
    desc_1 = 4
    data_1 = "\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A"

    header_2 = "\x00\x0A\x00\x00\x00\x01"
    length_2 = 10
    desc_2 = 1
    data_2 = "\x41\x42\x43\x44\x45\x46"

    leftover_data = "\x00\x0F\x00\x00\x00\x02\xFF"

    data = header_1 + data_1 + header_2 + data_2 + leftover_data

    (test_leftover, raw_msgs) = dist.parse_into_raw_msgs_api(data)

    if (test_leftover != leftover_data):
        print ("expected leftover data to be %s, but found %s"%
               (list(leftover_data), list(test_leftover)))
        sys.exit(-1)

    if (raw_msgs[0] != (header_1 + data_1)):
        print ("expected first raw_msg to be %s, but found %s"%
               (list(header_1 + data_1), list(raw_msgs[0])))
        sys.exit(-1)

    if (raw_msgs[1] != (header_2 + data_2)):
        print ("expected second raw_msg to be %s, but found %s"%
               (list(header_2 + data_2), list(raw_msgs[1])))
        sys.exit(-1)

    (test_len_1, test_desc_1, test_msg_1) = dist.parse_raw_msg_api(raw_msgs[0])
    (test_len_2, test_desc_2, test_msg_2) = dist.parse_raw_msg_api(raw_msgs[1])

    if (test_len_1 != length_1):
        print("expected 1st length to be %d but found %d"%(length_1, test_len_1))
        sys.exit(-1)

    if (test_len_2 != length_2):
        print("expected 2nd length to be %d but found %d"%(length_2, test_len_2))
        sys.exit(-1)

    if (test_desc_1 != desc_1):
        print("expected 1st desc to be %d but found %d"%(desc_1, test_desc_1))
        sys.exit(-1)

    if (test_desc_2 != desc_2):
        print("expected 2nd desc to be %d but found %d"%(desc_2, test_desc_2))
        sys.exit(-1)

    if (test_msg_1 != data_1):
        print("expected 1st msg to be %s but found %s"%
              (list(data_1), list(test_msg_1)))
        sys.exit(-1)

    if (test_msg_2 != data_2):
        print("expected 2nd msg to be %s but found %s"%
              (list(data_2), list(test_msg_2)))
        sys.exit(-1)

    print("ALL TESTS PASSED!")

