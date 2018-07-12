import wx
import GDSLogEventPanelGUI
from data_types import event_data
from pprint import pprint

###########################################################################
## Class LogEventsImpl
###########################################################################

class LogEventsImpl (GDSLogEventPanelGUI.LogEvents):

	def __init__( self, parent ):
		GDSLogEventPanelGUI.LogEvents.__init__ ( self, parent)

		self.scrollEventLogToBottom()

	def __del__( self ):
		pass

	def data_callback(self, data):

		#TODO find out in how the message portion of the event should be displayed.
		if type(data) == event_data.EventData:
                        arg_vals = tuple([arg.val for arg in data.args])
			l = [data.time.to_readable(),
                             data.template.name,
                             str(data.template.id),
                             data.template.severity,
                             data.template.format_str%arg_vals]
			self.EventLogDataListCtl.AppendItem(l)

	def scrollEventLogToBottom(self):
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


