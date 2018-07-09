import wx
import GDSLogEventPanelGUI
from data_types import event_data

###########################################################################
## Class LogEventsImpl
###########################################################################

class LogEventsImpl (GDSLogEventPanelGUI.LogEvents):
	
	def __init__( self, parent ):
		GDSLogEventPanelGUI.LogEvents.__init__ ( self, parent)
		
	def __del__( self ):
		pass
	
	def data_callback(self, data):
		#TODO find out in how the message portion of the event should be displayed.
		if type(data) == event_data.EventData:
			l = [data.time, data.template.name, data.template.id, data.template.severity, " "]
			self.EventLogDataListCtl.Append(l)

	# Override these handlers to implement functionality for GUI elements
	def onEventLogScrollCheckBoxClick( self, event ):
		event.Skip()
	
	def onEventLogClearButtonClick( self, event ):
		event.Skip()
	
	def onEventLogApplyFilterButtonClick( self, event ):
		event.Skip()
	
	def onEventLogResetFilterButtonClick( self, event ):
		event.Skip()
	

