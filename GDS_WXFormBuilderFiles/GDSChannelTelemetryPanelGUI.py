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
## Class ChannelTelemetry
###########################################################################

class ChannelTelemetry ( wx.Panel ):
	
	def __init__( self, parent ):
		wx.Panel.__init__ ( self, parent, id = wx.ID_ANY, pos = wx.DefaultPosition, size = wx.Size( 800,600 ), style = wx.TAB_TRAVERSAL )
		
		bSizer6 = wx.BoxSizer( wx.VERTICAL )
		
		self.ChannelTelemDataViewCtl = wx.dataview.DataViewCtrl( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.dataview.DV_MULTIPLE|wx.dataview.DV_VERT_RULES )
		bSizer6.Add( self.ChannelTelemDataViewCtl, 10, wx.ALL|wx.EXPAND, 5 )
		
		sbSizer8 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Channel Telemetry Filtering" ), wx.HORIZONTAL )
		
		self.ChannelTelemSelectChannelsButton = wx.Button( sbSizer8.GetStaticBox(), wx.ID_ANY, u"Select Channels", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer8.Add( self.ChannelTelemSelectChannelsButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.ChannelTelemResetFilterButton = wx.Button( sbSizer8.GetStaticBox(), wx.ID_ANY, u"Reset Filter", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer8.Add( self.ChannelTelemResetFilterButton, 0, wx.ALL, 5 )
		
		self.m_staticText3 = wx.StaticText( sbSizer8.GetStaticBox(), wx.ID_ANY, u"Active Filter Selected", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText3.Wrap( -1 )
		sbSizer8.Add( self.m_staticText3, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.ChannelTelemFilterSelectedTextCtl = wx.TextCtrl( sbSizer8.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
		sbSizer8.Add( self.ChannelTelemFilterSelectedTextCtl, 3, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.ChannelTelemShowHexCheckBox = wx.CheckBox( sbSizer8.GetStaticBox(), wx.ID_ANY, u"Show Hex", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer8.Add( self.ChannelTelemShowHexCheckBox, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		
		bSizer6.Add( sbSizer8, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer6 )
		self.Layout()
		
		# Connect Events
		self.ChannelTelemDataViewCtl.Bind( wx.dataview.EVT_DATAVIEW_ITEM_CONTEXT_MENU, self.onChannelTelemContextMenu, id = wx.ID_ANY )
		self.ChannelTelemSelectChannelsButton.Bind( wx.EVT_BUTTON, self.onChannelTelemSelectChannelsButtonClick )
		self.ChannelTelemResetFilterButton.Bind( wx.EVT_BUTTON, self.onClickResetFilter )
		self.ChannelTelemShowHexCheckBox.Bind( wx.EVT_CHECKBOX, self.onChannelTelemShowHexCheckBoxClick )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def onChannelTelemContextMenu( self, event ):
		event.Skip()
	
	def onChannelTelemSelectChannelsButtonClick( self, event ):
		event.Skip()
	
	def onClickResetFilter( self, event ):
		event.Skip()
	
	def onChannelTelemShowHexCheckBoxClick( self, event ):
		event.Skip()
	

