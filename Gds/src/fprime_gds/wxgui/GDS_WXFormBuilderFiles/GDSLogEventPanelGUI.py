# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version May 29 2018)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc
import wx.dataview

###########################################################################
## Class LogEvents
###########################################################################

class LogEvents ( wx.Panel ):
	
	def __init__( self, parent ):
		wx.Panel.__init__ ( self, parent, id = wx.ID_ANY, pos = wx.DefaultPosition, size = wx.Size( 800,600 ), style = wx.TAB_TRAVERSAL )
		
		bSizer41 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer61 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Event Log Messages" ), wx.VERTICAL )
		
		self.EventLogDataListCtl = wx.dataview.DataViewListCtrl( sbSizer61.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.dataview.DV_MULTIPLE|wx.dataview.DV_VERT_RULES )
		sbSizer61.Add( self.EventLogDataListCtl, 1, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer41.Add( sbSizer61, 5, wx.EXPAND, 5 )
		
		bSizer5 = wx.BoxSizer( wx.HORIZONTAL )
		
		
		bSizer5.Add( ( 0, 0), 1, wx.EXPAND, 5 )
		
		self.EventLogScrollCheckBox = wx.CheckBox( self, wx.ID_ANY, u"Scroll", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer5.Add( self.EventLogScrollCheckBox, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT|wx.ALL, 5 )
		
		self.EventLogClearButton = wx.Button( self, wx.ID_ANY, u"Clear", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer5.Add( self.EventLogClearButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT|wx.ALL, 5 )
		
		
		bSizer41.Add( bSizer5, 1, wx.ALIGN_RIGHT|wx.EXPAND, 5 )
		
		sbSizer7 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Display Only Log Event Messages Containing Keywords" ), wx.HORIZONTAL )
		
		self.EventLogSeachKeywordTextCtl = wx.TextCtrl( sbSizer7.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer7.Add( self.EventLogSeachKeywordTextCtl, 3, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.m_staticText2 = wx.StaticText( sbSizer7.GetStaticBox(), wx.ID_ANY, u"Severity", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText2.Wrap( -1 )
		sbSizer7.Add( self.m_staticText2, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		EventLogSeverityComboBoxChoices = []
		self.EventLogSeverityComboBox = wx.ComboBox( sbSizer7.GetStaticBox(), wx.ID_ANY, u"Combo!", wx.DefaultPosition, wx.DefaultSize, EventLogSeverityComboBoxChoices, 0 )
		sbSizer7.Add( self.EventLogSeverityComboBox, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.EventLogApplyFilterButton = wx.Button( sbSizer7.GetStaticBox(), wx.ID_ANY, u"Apply Filter", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer7.Add( self.EventLogApplyFilterButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.EventLogResetFilterButton = wx.Button( sbSizer7.GetStaticBox(), wx.ID_ANY, u"Reset", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer7.Add( self.EventLogResetFilterButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		
		bSizer41.Add( sbSizer7, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer41 )
		self.Layout()
		
		# Connect Events
		self.EventLogDataListCtl.Bind( wx.dataview.EVT_DATAVIEW_ITEM_CONTEXT_MENU, self.onLogEventDataViewContextMenu, id = wx.ID_ANY )
		self.EventLogScrollCheckBox.Bind( wx.EVT_CHECKBOX, self.onEventLogScrollCheckBoxClick )
		self.EventLogClearButton.Bind( wx.EVT_BUTTON, self.onEventLogClearButtonClick )
		self.EventLogApplyFilterButton.Bind( wx.EVT_BUTTON, self.onEventLogApplyFilterButtonClick )
		self.EventLogResetFilterButton.Bind( wx.EVT_BUTTON, self.onEventLogResetFilterButtonClick )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def onLogEventDataViewContextMenu( self, event ):
		event.Skip()
	
	def onEventLogScrollCheckBoxClick( self, event ):
		event.Skip()
	
	def onEventLogClearButtonClick( self, event ):
		event.Skip()
	
	def onEventLogApplyFilterButtonClick( self, event ):
		event.Skip()
	
	def onEventLogResetFilterButtonClick( self, event ):
		event.Skip()
	

