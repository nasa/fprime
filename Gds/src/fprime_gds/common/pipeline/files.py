"""
files.py:

Module to encapsulate the handling of file uplink and downlink. This includes setting up and registering both the file
uplinker and the file downlinker. These will handle the transferring of file data to/from the host into the
communications layer.

@author mstarch
"""
import fprime_gds.common.files.uplinker

class Filing(object):
    """
    A composition object that holds references to a FileUplinker and file downlinker.
    """
    def __init__(self):
        """
        The filing composition consists of an uplinker and a downlinker
        """
        self.__uplinker = None

    def setup_file_handling(self, file_encoder, file_decoder, distributor):
        """
        Sets up the file handling (uplink and downlink) from a pair of encoders and decoders
        :param file_encoder: file encoder for uplink
        :param file_decoder: file decoder for downlink
        """
        self.__uplinker = fprime_gds.common.files.uplinker.FileUplinker(file_encoder)
        distributor.register("FW_PACKET_HAND", self.__uplinker)

    @property
    def uplinker(self):
        """
        Property to return the uplinker
        :return: uplinker object
        """
        return self.__uplinker
