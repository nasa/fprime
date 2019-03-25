from __future__ import absolute_import
import wx
from . import GDSStatusPanelGUI
import binascii

###########################################################################
## Class StatusImpl
###########################################################################

class StatusImpl ( GDSStatusPanelGUI.Status ):
    """Implementation of the status panel tab
    """

    
    def __init__( self, parent, config=None ):
        GDSStatusPanelGUI.Status.__init__ ( self, parent)
        self._send_msg_buffer = []
        self._recv_msg_buffer = []

        # Start text control updating service
        self.update_text_ctrl()

    def __del__( self ):
        pass

    def update_text_ctrl(self):
        """Called to update the status panel with new raw output. Called every 500ms on the GUI thread.
        """

        for m in self._recv_msg_buffer:
            self.StatusTabRecvTextCtl.AppendText(m)
        for m in self._send_msg_buffer:
            self.StatusTabSendTextCtl.AppendText(m)
        self._send_msg_buffer = []
        self._recv_msg_buffer = []
        wx.CallLater(500, self.update_text_ctrl)
    
    # [00 12 34 ...]
    # Some data was sent
    def send(self, data, dest):
        """Send callback for the encoder
        
        Arguments:
            data {bin} -- binary data packet
            dest {string} -- where the data will be sent by the server
        """
        str_data = "[" +" ".join(["{0:2x}".format(byte if type(byte) != str else ord(byte)) for byte in data]) + "]\n\n"
        self._send_msg_buffer.append(str_data)

    # Some data was recvd
    def on_recv(self, data):
        """Data was recved on the socket server
        
        Arguments:
            data {bin} --binnary data string that was recved
        """
        str_data = "[" +" ".join(["{0:2x}".format(byte if type(byte) != str else ord(byte)) for byte in data]) + "]\n\n"

        self._recv_msg_buffer.append(str_data)

