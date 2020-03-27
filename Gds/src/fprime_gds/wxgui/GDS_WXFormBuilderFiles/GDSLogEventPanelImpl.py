# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version May 29 2018)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import GDSLogEventPanelGUI

###########################################################################
## Class LogEventsImpl
###########################################################################

class LogEventsImpl (GDSLogEventPanelGUI.LogEvents):
	
	def __init__( self, parent ):
		GDSLogEventPanelGUI.LogEvents.__init__ ( self, parent)
		
	def __del__( self ):
		pass
	
	
	# Override these handlers to implement functionality for GUI elements
	def onEventLogScrollCheckBoxClick( self, event ):
		event.Skip()
	
	def onEventLogClearButtonClick( self, event ):
		event.Skip()
	
	def onEventLogApplyFilterButtonClick( self, event ):
		event.Skip()
	
	def onEventLogResetFilterButtonClick( self, event ):
		event.Skip()
	

