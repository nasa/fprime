import wx
import GDSLogEventPanelGUI
from data_types import event_data
from pprint import pprint

###########################################################################
## Class LogEventsImpl
###########################################################################

class LogEventsImpl (GDSLogEventPanelGUI.LogEvents):
	'''Implementation class for LogEvents panel. Defines funcitonality.'''
	

	def __init__( self, parent ):
		"""LogEventsImple constructor
		
		Arguments:
			parent {wx.Window} -- The parrent for this GUI element
		"""

		
		GDSLogEventPanelGUI.LogEvents.__init__ ( self, parent)
		self.parent = parent
		self.scrollEventLogToBottom()

	def __del__( self ):
		pass

	def data_callback(self, data):
		"""Recieves data from decoders to which this consumer is registered
		
		Arguments:
			data {Data Object} -- A Data Object containing the data passed from the decoder (e.g., an EventData object)
		"""
		
		if type(data) == event_data.EventData:
			pprint(data)
			l = [data.time.to_readable(), data.template.name, str(data.template.id), data.template.severity, data.template.format_str%tuple([a.val for a in data.args])]
			self.EventLogDataListCtl.AppendItem(l)

	def scrollEventLogToBottom(self):
		"""Move the event log scroll bar so that the last entry is visible. Called repeatedly when the "scroll" box is checked"
		"""

		if self.EventLogScrollCheckBox.GetValue() == True:
			i = self.EventLogDataListCtl.RowToItem(int(self.EventLogDataListCtl.ItemCount - 1))
			self.EventLogDataListCtl.EnsureVisible(i)
			self.EventLogDataListCtl.Refresh()
		wx.CallLater(10, self.scrollEventLogToBottom)
		
	# Override these handlers to implement functionality for GUI elements
	def onEventLogClearButtonClick( self, event ):
		self.EventLogDataListCtl.DeleteAllItems()

	def onEventLogApplyFilterButtonClick( self, event ):
		event.Skip()

	def onEventLogResetFilterButtonClick( self, event ):
		event.Skip()


