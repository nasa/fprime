'''
@brief Class to log raw binary input and output as well as telemetry and events
'''

import datetime
import os

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.data_types.pkt_data import PktData

class DataLogger(object):

    def __init__(self, logdir, verbose=False, csv=False, prefix=""):

        self.logdir = logdir

        self.recv_file = prefix + "recv.bin"
        self.send_file = prefix + "sent.bin"
        self.telem_file = prefix + "channel.log"
        self.event_file = prefix + "event.log"
        self.command_file = prefix + "command.log"

        self.verbose = verbose
        self.csv = csv
        self.f_r = open(self.logdir + os.sep + self.recv_file, "wb+")
        self.f_s = open(self.logdir + os.sep + self.send_file, "wb+")
        self.f_telem = open(self.logdir + os.sep + self.telem_file, "w+")
        self.f_event = open(self.logdir + os.sep + self.event_file, "w+")


    def __del__(self):
        self.f_r.close()
        self.f_s.close()
        self.f_telem.close()
        self.f_event.close()

    def data_callback(self, data):
        # TODO Ideally, each data object would have an identifier for its type,
        # or you would have a separate logger object for each
        if (isinstance(data, ChData) or isinstance(data, PktData)):
            self.f_telem.write(data.get_str(verbose=self.verbose, csv=self.csv)+
                               '\n')

        if (isinstance(data, EventData)):
            self.f_event.write(data.get_str(verbose=self.verbose, csv=self.csv)+
                               '\n')


    def send(self, data, dest):
        """Send callback for the encoder

        Arguments:
            data {bin} -- binary data packet
            dest {string} -- where the data will be sent by the server
        """

        self.f_s.write(data)

    # Some data was recvd
    def on_recv(self, data):
        """Data was recved on the socket server

        Arguments:
            data {bin} --binnary data string that was recved
        """

        self.f_r.write(data)
