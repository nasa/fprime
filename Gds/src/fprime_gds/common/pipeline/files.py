"""
files.py:

Module to encapsulate the handling of file uplink and downlink. This includes setting up and registering both the file
uplinker and the file downlinker. These will handle the transferring of file data to/from the host into the
communications layer.

@author mstarch
"""
import fprime_gds.common.files.downlinker
import fprime_gds.common.files.uplinker


class Filing:
    """
    A composition object that holds references to a FileUplinker and file downlinker.
    """

    def __init__(self):
        """
        The filing composition consists of an uplinker and a downlinker
        """
        self.__uplinker = None
        self.__downlinker = None

    def setup_file_handling(
        self, down_store, file_encoder, file_decoder, distributor, log_dir
    ):
        """
        Sets up the file handling (uplink and downlink) from a pair of encoders and decoders

        :param down_store: downlink storage directory
        :param file_encoder: file encoder for uplink
        :param file_decoder: file decoder for downlink
        :param distributor: data distributor to register handshaking to
        :param log_dir: log directory to output downlink logs
        """
        self.__uplinker = fprime_gds.common.files.uplinker.FileUplinker(file_encoder)
        self.__downlinker = fprime_gds.common.files.downlinker.FileDownlinker(
            down_store, log_dir=log_dir
        )
        file_decoder.register(self.__downlinker)
        distributor.register("FW_PACKET_HAND", self.__uplinker)

    @property
    def uplinker(self):
        """
        Property to return the uplinker

        :return: uplinker object
        """
        return self.__uplinker

    @property
    def downlinker(self):
        """
        Property to return the downlinker

        :return: downlinker object
        """
        return self.__downlinker
