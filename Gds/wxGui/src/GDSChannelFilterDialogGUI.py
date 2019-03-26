# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version May 29 2018)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc

###########################################################################
## Class ChannelFilterDialog
###########################################################################

class ChannelFilterDialog ( wx.Dialog ):
	
	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Choose Filter Members", pos = wx.DefaultPosition, size = wx.Size( 635,401 ), style = wx.DEFAULT_DIALOG_STYLE|wx.STAY_ON_TOP )
		
		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_panel1 = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer2 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer3 = wx.BoxSizer( wx.HORIZONTAL )
		
		bSizer5 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_staticText1 = wx.StaticText( self.m_panel1, wx.ID_ANY, u"Channels to Exclude", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText1.Wrap( -1 )
		bSizer5.Add( self.m_staticText1, 0, wx.ALL, 5 )
		
		ListBoxAllChannelsChoices = []
		self.ListBoxAllChannels = wx.ListBox( self.m_panel1, wx.ID_ANY, wx.DefaultPosition, wx.Size( 230,-1 ), ListBoxAllChannelsChoices, wx.LB_EXTENDED|wx.LB_HSCROLL|wx.LB_SORT )
		bSizer5.Add( self.ListBoxAllChannels, 1, wx.ALL, 5 )
		
		
		bSizer3.Add( bSizer5, 1, wx.EXPAND, 5 )
		
		bSizer8 = wx.BoxSizer( wx.VERTICAL )
		
		
		bSizer8.Add( ( 0, 0), 1, wx.EXPAND, 5 )
		
		self.ButtonAddChannel = wx.Button( self.m_panel1, wx.ID_ANY, u"Add >", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.ButtonAddChannel.SetFont( wx.Font( 10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, wx.EmptyString ) )
		
		bSizer8.Add( self.ButtonAddChannel, 1, wx.ALIGN_CENTER|wx.ALL, 5 )
		
		self.ButtonAddAllChannels = wx.Button( self.m_panel1, wx.ID_ANY, u"Add All >>", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.ButtonAddAllChannels.SetFont( wx.Font( 10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, wx.EmptyString ) )
		
		bSizer8.Add( self.ButtonAddAllChannels, 1, wx.ALIGN_CENTER|wx.ALL, 5 )
		
		self.ButtonRemoveChannel = wx.Button( self.m_panel1, wx.ID_ANY, u"< Rem", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.ButtonRemoveChannel.SetFont( wx.Font( 10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, wx.EmptyString ) )
		
		bSizer8.Add( self.ButtonRemoveChannel, 1, wx.ALIGN_CENTER|wx.ALL, 5 )
		
		self.ButtonRemoveAllChannels = wx.Button( self.m_panel1, wx.ID_ANY, u"<< Rem All", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.ButtonRemoveAllChannels.SetFont( wx.Font( 10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, False, wx.EmptyString ) )
		
		bSizer8.Add( self.ButtonRemoveAllChannels, 1, wx.ALIGN_CENTER|wx.ALL, 5 )
		
		
		bSizer8.Add( ( 0, 0), 1, wx.EXPAND, 5 )
		
		
		bSizer3.Add( bSizer8, 0, wx.EXPAND, 5 )
		
		bSizer9 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_staticText2 = wx.StaticText( self.m_panel1, wx.ID_ANY, u"Channels to Show", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText2.Wrap( -1 )
		bSizer9.Add( self.m_staticText2, 0, wx.ALIGN_RIGHT|wx.ALL, 5 )
		
		ListBoxShowChannelsChoices = []
		self.ListBoxShowChannels = wx.ListBox( self.m_panel1, wx.ID_ANY, wx.DefaultPosition, wx.Size( 230,-1 ), ListBoxShowChannelsChoices, wx.LB_EXTENDED|wx.LB_HSCROLL|wx.LB_SORT )
		bSizer9.Add( self.ListBoxShowChannels, 1, wx.ALIGN_RIGHT|wx.ALL, 5 )
		
		
		bSizer3.Add( bSizer9, 1, wx.EXPAND, 5 )
		
		
		bSizer2.Add( bSizer3, 8, wx.EXPAND, 5 )
		
		bSizer4 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_staticText4 = wx.StaticText( self.m_panel1, wx.ID_ANY, u"Current File", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText4.Wrap( -1 )
		bSizer4.Add( self.m_staticText4, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.TextCtrlFileName = wx.TextCtrl( self.m_panel1, wx.ID_ANY, u"None", wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
		bSizer4.Add( self.TextCtrlFileName, 1, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.ButtonSaveFilter = wx.Button( self.m_panel1, wx.ID_ANY, u"Save Filter", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.ButtonSaveFilter, 1, wx.ALL, 5 )
		
		self.ButtonLoadFilter = wx.Button( self.m_panel1, wx.ID_ANY, u"Load Filter", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.ButtonLoadFilter, 1, wx.ALL, 5 )
		
		self.ButtionApplyFilter = wx.Button( self.m_panel1, wx.ID_ANY, u"Apply Filter", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.ButtionApplyFilter, 1, wx.ALL, 5 )
		
		self.ButtonCancel = wx.Button( self.m_panel1, wx.ID_ANY, u"Close", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.ButtonCancel, 1, wx.ALL, 5 )
		
		
		bSizer2.Add( bSizer4, 1, wx.EXPAND, 5 )
		
		
		self.m_panel1.SetSizer( bSizer2 )
		self.m_panel1.Layout()
		bSizer2.Fit( self.m_panel1 )
		bSizer1.Add( self.m_panel1, 1, wx.EXPAND |wx.ALL, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.Bind( wx.EVT_CLOSE, self.onCloseChannelFilterDialog )
		self.ButtonAddChannel.Bind( wx.EVT_BUTTON, self.onClickAddChannel )
		self.ButtonAddAllChannels.Bind( wx.EVT_BUTTON, self.onClickAddAllChannels )
		self.ButtonRemoveChannel.Bind( wx.EVT_BUTTON, self.onClickRemoveChannel )
		self.ButtonRemoveAllChannels.Bind( wx.EVT_BUTTON, self.onClickRemoveAllChannels )
		self.ButtonSaveFilter.Bind( wx.EVT_BUTTON, self.onClickSaveFilter )
		self.ButtonLoadFilter.Bind( wx.EVT_BUTTON, self.onClickLoadFilter )
		self.ButtionApplyFilter.Bind( wx.EVT_BUTTON, self.onClickApplyFilter )
		self.ButtonCancel.Bind( wx.EVT_BUTTON, self.onClickClose )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def onCloseChannelFilterDialog( self, event ):
		event.Skip()
	
	def onClickAddChannel( self, event ):
		event.Skip()
	
	def onClickAddAllChannels( self, event ):
		event.Skip()
	
	def onClickRemoveChannel( self, event ):
		event.Skip()
	
	def onClickRemoveAllChannels( self, event ):
		event.Skip()
	
	def onClickSaveFilter( self, event ):
		event.Skip()
	
	def onClickLoadFilter( self, event ):
		event.Skip()
	
	def onClickApplyFilter( self, event ):
		event.Skip()
	
	def onClickClose( self, event ):
		event.Skip()
	
