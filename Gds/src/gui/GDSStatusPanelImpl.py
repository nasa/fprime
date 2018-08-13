import wx
import GDSStatusPanelGUI
import binascii
###########################################################################
## Class StatusImpl
###########################################################################

class StatusImpl ( GDSStatusPanelGUI.Status ):
	"""Implementation of the status panel tab
	"""

	
	def __init__( self, parent, config=None ):
		GDSStatusPanelGUI.Status.__init__ ( self, parent)
		self.msg_buffer = []

		# Start text control updating service
		self.update_text_ctrl()

	def __del__( self ):
		pass

	def update_text_ctrl(self):
		"""Called to update the status panel with new raw output. Called every 500ms on the GUI thread.
		"""

		for m in self.msg_buffer:
			self.StatusTabTextCtl.AppendText(m)
		self.msg_buffer = []
		wx.CallLater(500, self.update_text_ctrl)
	
	# Some data was sent
	def send(self, data, dest):
		"""Send callback for the encoder
		
		Arguments:
			data {bin} -- binary data packet
			dest {string} -- where the data will be sent by the server
		"""

		self.msg_buffer.append("Data sent to " + dest + '\n' + binascii.hexlify(data) + '\n\n')

	# Some data was recvd
	def on_recv(self, data):
		"""Data was recved on the socket server
		
		Arguments:
			data {bin} --binnary data string that was recved
		"""

		self.msg_buffer.append("Data was recieved\n" + binascii.hexlify(data) + '\n\n')

