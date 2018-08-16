import wx
import GDSCommandPanelGUI

###########################################################################
## Class CommandsImpl
###########################################################################

class CommandsImpl (GDSCommandPanelGUI.Commands):
	
	def __init__( self, parent ):
		GDSCommandPanelGUI.Commands.__init__ ( self, parent)
		
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
	

