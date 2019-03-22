from __future__ import absolute_import
import wx
from . import GDSMainFrameGUI
from . import GDSChannelTelemetryPanelImpl
from . import GDSStatusPanelImpl
from . import GDSCommandPanelImpl
from . import GDSLogEventPanelImpl

import os


###########################################################################
## Class MainFrameImpl
###########################################################################

class MainFrameImpl ( GDSMainFrameGUI.MainFrame ):
	"""Implementation of the main frame of the GDS which holds all of the different tabs
	"""

	def __init__( self, parent, factory, evnt_pnl_state = None, tlm_pnl_state = None, status_bar_state = None, ch_dict = None, config=None):
		GDSMainFrameGUI.MainFrame.__init__ (self, parent)

		self.status_bar = GDSStatusBar(self)
		self.SetStatusBar(self.status_bar)

		# The number of recvd bytes the last time we checked. Use to update the status light
		self.last_num_byte_recv = 0

		self.cmd_pnl = GDSCommandPanelImpl.CommandsImpl(self.TabNotebook, factory.cmd_name_dict, config=config)
		self.event_pnl = GDSLogEventPanelImpl.LogEventsImpl(self.TabNotebook, config=config)
		self.telem_pnl = GDSChannelTelemetryPanelImpl.ChannelTelemetryImpl(self.TabNotebook, ch_dict=ch_dict, config=config)
		self.status_pnl = GDSStatusPanelImpl.StatusImpl(self.TabNotebook, config=config)

		if evnt_pnl_state:
			self.event_pnl.setEventLogState(evnt_pnl_state)

		if tlm_pnl_state:
			self.telem_pnl.setChannelTelemDataViewState(tlm_pnl_state)

		if status_bar_state:
			self.status_bar.set_state(status_bar_state)

		self.TabNotebook.AddPage( self.cmd_pnl, u"Commands", False )
		self.TabNotebook.AddPage( self.event_pnl, u"Log Events", False )
		self.TabNotebook.AddPage( self.telem_pnl, u"Channel Telemetry", False )
		self.TabNotebook.AddPage( self.status_pnl, u"Status", False )

		self.main_frame_factory = factory

		# Start updating light
		self.updateStatusBar()
		
	def __del__( self ):
		pass

	def updateStatusBar(self):
		"""Called every second to update teh values shown on the status bar at the bottom of the window
		"""

		self.status_bar.SetStatusText('Bytes Sent %d | Bytes Recv %d'%(self.status_bar.bytes_sent, self.status_bar.bytes_recv), 1)

		if self.status_bar.bytes_recv > self.last_num_byte_recv:
			self.last_num_byte_recv = self.status_bar.bytes_recv
			self.status_bar.light_color = wx.Colour('green')
		else:
			self.status_bar.light_color = wx.Colour('red')

		self.status_bar.light_panel.Refresh()

		wx.CallLater(1000, self.updateStatusBar)

	def updateBytesRecv(self, num):
		"""Add to the number of bytes recieved
		
		Arguments:
			num {int} -- number of bytes to add to the count
		"""

		self.status_bar.bytes_recv += num

	def updateBytesSent(self, num):
		"""Add to the number of bytes sent
		
		Arguments:
			num {int} -- number of bytes to add to the count
		"""

		self.status_bar.bytes_sent += num

	def on_recv(self, data):
		"""Called when data is recieved over the TCP server. This callback is only here to get the length of the recvd data.
		
		Arguments:
			data {bin} -- binary data list recieved
		"""

		self.updateBytesRecv(len(data))

	def send(self, data, dest):
		"""Called when data is sent from the command encoder to the TCP client. Only here so we can get the length of what is sent.
		
		Arguments:
			data {bin} -- binary data list recieved
			dest {string} -- destination string needed to creat the full binary string sent
		"""

		self.updateBytesSent(len("A5A5 %s %s"%(dest, data)))

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


class GDSStatusBar(wx.StatusBar):
	"""Custom wxStatusBar implementation that allows us to draw the status light/circle on the bar
	"""
	
	def __init__(self, parent):
		wx.StatusBar.__init__(self, parent, -1)

		# Tracked quantities for the status bar
		self.bytes_recv = 0
		self.bytes_sent = 0

		self.light_color = wx.Colour('red')

		self.SetFieldsCount(2)
		self.SetStatusWidths([40, -4])

		self.SetStatusText("Bytes <>", 1)

		self.light_panel = wx.Panel(self, size=wx.Size(self.GetStatusWidth(0), 100))

		self.light_panel.Bind(wx.EVT_PAINT, self.onPaint)

	def onPaint(self, event):
		"""Called when the status bar is redrawn (Refresh() is called on it)
		"""

		dc = wx.PaintDC(self.light_panel)
		dc.Clear()
		dc.SetBrush(wx.Brush(self.light_color))
		dc.DrawCircle(16, 8, 8)
	
	def get_state(self):
		"""Get the current state of the status bar
		
		Returns:
			tuple -- (number of bytes recved, number of bytes sent)
		"""

		return self.bytes_recv, self.bytes_sent
	
	def set_state(self, state):
		"""Set the state of the status bar
		
		Arguments:
			state {tuple} -- (number of bytes recved, number of bytes sent)
		"""

		self.bytes_recv = state[0]
		self.bytes_sent = state[1]


	
