import wx
import GDSStatusPanelGUI
import binascii
###########################################################################
## Class StatusImpl
###########################################################################

class StatusImpl ( GDSStatusPanelGUI.Status ):
	
	def __init__( self, parent ):
		GDSStatusPanelGUI.Status.__init__ ( self, parent)
		self.msg_buffer = []

		# Start text control updating service
		self.update_text_ctrl()

	def __del__( self ):
		pass

	def update_text_ctrl(self):

		for m in self.msg_buffer:
			self.StatusTabTextCtl.AppendText(m)
		self.msg_buffer = []
		wx.CallLater(500, self.update_text_ctrl)
	
	# Some data was sent
	def send(self, data, dest):
		self.msg_buffer.append("Data sent to " + dest + '\n' + binascii.hexlify(data) + '\n\n')

	# Some data was recvd
	def on_recv(self, data):
		self.msg_buffer.append("Data was recieved\n" + binascii.hexlify(data) + '\n\n')

