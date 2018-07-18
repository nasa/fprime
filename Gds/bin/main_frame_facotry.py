'''
@brief Main frame factory class

This class demonstrated how to set up a pipeline with the GDS API and provides
an interface for creating additional GDS windows that use this pipeline

@data Created July 18, 2018
@author Josef Biberstein

@bug No known bugs
'''


from loaders import cmd_py_loader, event_py_loader, ch_py_loader
from encoders import cmd_encoder
from decoders import ch_decoder, event_decoder, pkt_decoder
from distributor import distributor
from client_socket import client_socket

from gui import GDSMainFrameImpl

import os


class MainFrameFactory(object):
    
    def __init__(self, opts):
        self.opts = opts

        self.client_socket = None
        self.dist = None

        self.evnt_ldr = None
        self.cmd_ldr = None
        self.ch_ldr = None

        self.cmd_enc = None
        self.event_dec = None
        self.ch_dec = None

        self.main_frame_instances = []


    def create_new_window(self):
        if len(self.main_frame_instances) > 0:
            frame = GDSMainFrameImpl.MainFrameImpl(None, self, \
                evnt_pnl_state=self.main_frame_instances[0].event_pnl.getEventLogState(), \
                tlm_pnl_state=self.main_frame_instances[0].telem_pnl.getChannelTelemDataViewState())
            self.event_dec.register(frame.event_pnl)
            self.ch_dec.register(frame.telem_pnl)
            frame.cmd_pnl.register_encoder(self.cmd_enc)
            self.main_frame_instances.append(frame)
            frame.Show(True)
        else:
            raise Exception("Please run setup_pipline() before using this method to create another window")

        
    def setup_pipeline(self):
        self.dist = distributor.Distributor()
        self.client_socket = client_socket.ThreadedTCPSocketClient()

        self.evnt_ldr = event_py_loader.EventPyLoader()
        eid_dict = self.evnt_ldr.get_id_dict(self.opts.generated_path + os.sep + "events")

        self.cmd_ldr = cmd_py_loader.CmdPyLoader()
        cname_dict = self.cmd_ldr.get_name_dict(self.opts.generated_path + os.sep + "commands")

        self.ch_ldr = ch_py_loader.ChPyLoader()
        ch_dict = self.ch_ldr.get_id_dict(self.opts.generated_path + os.sep + "channels")

        self.cmd_enc = cmd_encoder.CmdEncoder(cname_dict)
        self.event_dec = event_decoder.EventDecoder(eid_dict)
        self.ch_dec = ch_decoder.ChDecoder(ch_dict)

        self.client_socket.register_distributor(self.dist)
        
        self.dist.register("FW_PACKET_LOG", self.event_dec)
        self.dist.register("FW_PACKET_TELEM", self.ch_dec)

        frame = GDSMainFrameImpl.MainFrameImpl(None, self)
        
        self.event_dec.register(frame.event_pnl)
        self.ch_dec.register(frame.telem_pnl)
        frame.cmd_pnl.register_encoder(self.cmd_enc)
        self.cmd_enc.register(self.client_socket)

        frame.Show(True)
        self.main_frame_instances.append(frame)