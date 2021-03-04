"""
@brief Class to log raw binary input and output as well as telemetry and events
"""

import os

import fprime_gds.common.handlers
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.cmd_data import CmdData
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.data_types.pkt_data import PktData


class DataLogger(fprime_gds.common.handlers.DataHandler):
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
        self.f_command = open(self.logdir + os.sep + self.command_file, "w+")

    def __del__(self):
        self.f_r.close()
        self.f_s.close()
        self.f_telem.close()
        self.f_event.close()

    def data_callback(self, data, sender=None):
        if isinstance(data, ChData) or isinstance(data, PktData):
            self.f_telem.write(data.get_str(verbose=self.verbose, csv=self.csv) + "\n")
            self.f_telem.flush()

        if isinstance(data, EventData):
            self.f_event.write(data.get_str(verbose=self.verbose, csv=self.csv) + "\n")
            self.f_event.flush()

        if isinstance(data, CmdData):
            self.f_command.write(
                data.get_str(verbose=self.verbose, csv=self.csv) + "\n"
            )
            self.f_command.flush()

    def send(self, data, dest):
        """Send callback for the encoder

        Arguments:
            data {bin} -- binary data packet
            dest {string} -- where the data will be sent by the server
        """

        self.f_s.write(data)
        self.f_s.flush()

    # Some data was recvd
    def on_recv(self, data):
        """Data was received on the socket server

        Arguments:
            data {bin} --binary data string that was received
        """

        self.f_r.write(data)
        self.f_r.flush()
