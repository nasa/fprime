import datetime

from data_types.ch_data import ChData
from data_types.event_data import EventData
from data_types.pkt_data import PktData
class DataLogger(object):

    def __init__(self, logdir, verbose=False, csv=False):

        self.logdir = logdir

        n = str(datetime.datetime.time(datetime.datetime.now())).replace(":", "_").replace(".", "_")

        self.recv_file = "recv_" + n + ".bin"
        self.send_file = "sent" + n + ".bin"
        self.plain_file = "humman_readable" + n + ".txt"

        self.verbose = verbose
        self.csv = csv
        self.f_r = open(self.logdir + self.recv_file, "w+")
        self.f_s = open(self.logdir + self.send_file, "w+")
        self.f_h = open(self.logdir + self.plain_file, "w+")
        

    def __del__(self):
        self.f_r.close()
        self.f_s.close()
        self.f_h.close()

    def data_callback(self, data):

        self.f_h.write(data.get_str(verbose=self.verbose, csv=self.csv) + '\n')
            

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
