import wx
import GDSCommandPanelGUI
import GDSArgItemTextCtl
import GDSArgItemComboBox

###########################################################################
## Class CommandsImpl
###########################################################################

class CommandsImpl (GDSCommandPanelGUI.Commands):
	
	def __init__( self, parent ):
		GDSCommandPanelGUI.Commands.__init__ ( self, parent)

		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), "hi"))
		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), "hi"))
		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], "combo breaker"))
		self.CmdArgsScrolledWindow.GetSizer().Add(GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], "combo breaker"))		
		
	def __del__( self ):
		pass
	
	
	# Override these handlers to implement functionality for GUI elements
	def onCmdsComboBoxSelect( self, event ):
		event.Skip()
	
	def onCmdSendButtonClick( self, event ):
		event.Skip()
	
	def onCmdHistSearchButtonClick( self, event ):
		event.Skip()
	
	def onCmdHistClearButtonClick( self, event ):
		event.Skip()
	
	def onQuickCmdClearButtonClick( self, event ):
		event.Skip()
	
	def onQuickCmdSendButtonClick( self, event ):
		event.Skip()
	

