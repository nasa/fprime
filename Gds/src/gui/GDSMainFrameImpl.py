import wx
import GDSMainFrameGUI
import GDSChannelTelemetryPanelImpl
import GDSStatusPanelImpl
import GDSCommandPanelImpl
import GDSLogEventPanelImpl

import os

from pprint import pprint

###########################################################################
## Class MainFrameImpl
###########################################################################

class MainFrameImpl ( GDSMainFrameGUI.MainFrame ):

	def __init__( self, parent, factory, evnt_pnl_state = None, tlm_pnl_state = None):
		GDSMainFrameGUI.MainFrame.__init__ (self, parent)

		self.cmd_pnl = GDSCommandPanelImpl.CommandsImpl(self.TabNotebook, factory.cmd_name_dict)
		self.status_pnl = GDSStatusPanelImpl.StatusImpl(self.TabNotebook)
		self.event_pnl = GDSLogEventPanelImpl.LogEventsImpl(self.TabNotebook)
		self.telem_pnl = GDSChannelTelemetryPanelImpl.ChannelTelemetryImpl(self.TabNotebook)

		if evnt_pnl_state:
			self.event_pnl.setEventLogState(evnt_pnl_state)

		if tlm_pnl_state:
			self.telem_pnl.setChannelTelemDataViewState(tlm_pnl_state)

		self.TabNotebook.AddPage( self.cmd_pnl, u"Commands", False )
		self.TabNotebook.AddPage( self.status_pnl, u"Status", False )
		self.TabNotebook.AddPage( self.event_pnl, u"Log Events", False )
		self.TabNotebook.AddPage( self.telem_pnl, u"Channel Telemetry", False )

		self.main_frame_factory = factory

	def __del__( self ):
		pass

	# Override these handlers to implement functionality for GUI elements
	def onMainFrameClose(self, event ):
		self.main_frame_factory.main_frame_instances.remove(self)
		self.Destroy()

	def onNewMenuItemClick( self, event ):
		self.main_frame_factory.create_new_window()

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
		self.onMainFrameClose(event)


