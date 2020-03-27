import wx
import GDSMainFrameGUI
import GDSChannelTelemetryPanelImpl
import GDSStatusPanelImpl
import GDSCommandPanelImpl
import GDSLogEventPanelImpl

###########################################################################
## Class MainFrameImpl
###########################################################################

class MainFrameImpl ( GDSMainFrameGUI.MainFrame ):
	
	def __init__( self, parent ):
		GDSMainFrameGUI.MainFrame.__init__ ( self, parent)
		
		self.cmd_pnl = GDSCommandPanelImpl.CommandsImpl(self.TabNotebook)
		self.status_pnl = GDSStatusPanelImpl.StatusImpl(self.TabNotebook)
		self.event_pnl = GDSLogEventPanelImpl.LogEventsImpl(self.TabNotebook)
		self.telem_pnl = GDSChannelTelemetryPanelImpl.ChannelTelemetryImpl(self.TabNotebook)

		self.TabNotebook.AddPage( self.cmd_pnl, u"Commands", False )
		self.TabNotebook.AddPage( self.status_pnl, u"Status", False )
		self.TabNotebook.AddPage( self.event_pnl, u"Log Events", False )
		self.TabNotebook.AddPage( self.telem_pnl, u"Channel Telemetry", False )

		self.child_main_instances = []
		
	def __del__( self ):
		pass
	
	# Override these handlers to implement functionality for GUI elements
	def onNewMenuItemClick( self, event ):
		frame = MainFrameImpl(self)
		self.child_main_instances.append(frame)
		frame.Show(True)
	
	def onSaveMenuItemClick( self, event ):
		event.Skip()
	
	def onLoadMenuItemClick( self, event ):
		event.Skip()
	
	def onAboutMenuItemClick( self, event ):
		event.Skip()
	
	def onSaveWinCfgMenuItemClick( self, event ):
		event.Skip()
	
	def onRestoreWinMenuItemClick( self, event ):
		event.Skip()
	
	def onExitMenuItemClick( self, event ):
		event.Skip()
	

