import wx
import GDSChannelTelemetryPanelGUI
###########################################################################
## Class ChannelTelemetryImpl
###########################################################################

class ChannelTelemetryImpl (GDSChannelTelemetryPanelGUI.ChannelTelemetry):
	
	def __init__( self, parent ):
		GDSChannelTelemetryPanelGUI.ChannelTelemetry.__init__ ( self, parent)
		
	def __del__( self ):
		pass
	
	# Override these handlers to implement functionality for GUI elements
	def onChannelTelemSelectChannelsButtonClick( self, event ):
		event.Skip()
	
	def onChannelTelemShowHexCheckBoxClick( self, event ):
		event.Skip()
	

