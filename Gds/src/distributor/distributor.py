import binascii

from models.serialize import u32_type
from utils import data_desc_type


# NOTE decoder function to call is called data_callback(data)
# TODO close the socket when you're done
class Distributor(object):
    """
    A distributor contains a socket client that connects to a ThreadedTCPServer.
    It then sends and recvs data from a FPrime deployment.
    Decoders can register with a distributor to recv packets of data of a certain description.
    """

    def __init__(self):
        """
        Sets up the dictionary of connected encoders and socket client object. Encoder dictionary is of the form
        {data descriptor name: list of decoder objects registered for that data}
        """
        self.__decoders = {key.name: [] for key in list(data_desc_type.DataDescType)}

        # Internal buffer for un distributed data
        self.__buf = ""

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

        # TODO remove
        print("Decoders=%s"%self.__decoders)


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

        # TODO remove
        print("data=%s"%list(data))


        # Add new data to end of buffer
        self.__buf = self.__buf + data

        # Parse messages until we run low on data and then break
        while True:
            # Start of data message should always be at start of buffer
            offset = 0

            # TODO remove
            print("self.__buf=%s"%list(self.__buf))


            # 4 byte length header
            msg_len = u32_type.U32Type()
            # Check if we have enough data to parse a length
            if (len(self.__buf) < msg_len.getSize()):
                break
            msg_len.deserialize(self.__buf, offset)
            offset += msg_len.getSize()

            expected_len = msg_len.val + msg_len.getSize()

            # Check if we have enough data to parse
            if (len(self.__buf) < expected_len):
                break

            data_desc = u32_type.U32Type()
            data_desc.deserialize(self.__buf, offset)
            offset += data_desc.getSize()

            # This data will be passed on to decoders
            data_pass_thru = self.__buf[offset:expected_len]
            # The rest of the data will be parsed later
            self.__buf = self.__buf[expected_len:]

            # Pass on data to decoders
            data_desc_key = data_desc_type.DataDescType(data_desc.val).name

            for d in self.__decoders[data_desc_key]:
                d.data_callback(data_pass_thru)

if __name__ == "__main__":
    pass
