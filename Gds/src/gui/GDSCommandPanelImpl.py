import wx
import GDSCommandPanelGUI
import GDSArgItemTextCtl
import GDSArgItemComboBox
from models.serialize import u32_type

###########################################################################
## Class CommandsImpl
###########################################################################

class CommandsImpl (GDSCommandPanelGUI.Commands):

	'''TODO remove client debugging''' 
	def __init__( self, parent, client ):
		GDSCommandPanelGUI.Commands.__init__ ( self, parent)

		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), "hi"))
		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), "hi"))
		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], "combo breaker"))
		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], "combo breaker"))
		self.client = client
	def __del__( self ):
		pass


	# Override these handlers to implement functionality for GUI elements
	def onCmdsComboBoxSelect( self, event ):
		event.Skip()

	def onCmdSendButtonClick( self, event ):
		# TODO remove debugging 
		op_code = u32_type.U32Type(0x79)
		cmd_data = op_code.serialize()
		desc = u32_type.U32Type(0x5A5A5A5A)
		desc_type = u32_type.U32Type(0)
		data_len = u32_type.U32Type(len(cmd_data) + desc_type.getSize())
		self.client.send("A5A5 FSW " + desc.serialize() + data_len.serialize() + desc_type.serialize() + cmd_data)

	def onCmdHistSearchButtonClick( self, event ):
		event.Skip()

	def onCmdHistClearButtonClick( self, event ):
		event.Skip()

	def onQuickCmdClearButtonClick( self, event ):
		event.Skip()

	def onQuickCmdSendButtonClick( self, event ):
		event.Skip()


