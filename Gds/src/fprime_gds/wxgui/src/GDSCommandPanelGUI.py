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
## Class Commands
###########################################################################

class Commands ( wx.Panel ):

	def __init__( self, parent ):
		wx.Panel.__init__ ( self, parent, id = wx.ID_ANY, pos = wx.DefaultPosition, size = wx.Size( 800,600 ), style = wx.TAB_TRAVERSAL )

		CommandsMainBoxSizer = wx.BoxSizer( wx.VERTICAL )

		self.ImmediateCommandEntries = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		sbSizer1 = wx.StaticBoxSizer( wx.StaticBox( self.ImmediateCommandEntries, wx.ID_ANY, u"Immediate Command Entries" ), wx.HORIZONTAL )

		self.m_staticText1 = wx.StaticText( sbSizer1.GetStaticBox(), wx.ID_ANY, u"Cmds", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText1.Wrap( -1 )
		sbSizer1.Add( self.m_staticText1, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

		CmdsComboBoxChoices = []
		self.CmdsComboBox = wx.ComboBox( sbSizer1.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, CmdsComboBoxChoices, 0 )
		sbSizer1.Add( self.CmdsComboBox, 4, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

		CmdArgsBoxSizer = wx.StaticBoxSizer( wx.StaticBox( sbSizer1.GetStaticBox(), wx.ID_ANY, u"Command Args" ), wx.HORIZONTAL )

		self.CmdArgsScrolledWindow = wx.ScrolledWindow( CmdArgsBoxSizer.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.HSCROLL|wx.SUNKEN_BORDER )
		self.CmdArgsScrolledWindow.SetScrollRate( 5, 5 )
		CmdArgsScrolledWindowBoxSizer = wx.BoxSizer( wx.HORIZONTAL )

		CmdArgsScrolledWindowBoxSizer.SetMinSize( wx.Size( 300,-1 ) )

		self.CmdArgsScrolledWindow.SetSizer( CmdArgsScrolledWindowBoxSizer )
		self.CmdArgsScrolledWindow.Layout()
		CmdArgsScrolledWindowBoxSizer.Fit( self.CmdArgsScrolledWindow )
		CmdArgsBoxSizer.Add( self.CmdArgsScrolledWindow, 1, wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )


		sbSizer1.Add( CmdArgsBoxSizer, 4, wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )

		self.CmdSendButton = wx.Button( sbSizer1.GetStaticBox(), wx.ID_ANY, u"Send", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer1.Add( self.CmdSendButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )


		self.ImmediateCommandEntries.SetSizer( sbSizer1 )
		self.ImmediateCommandEntries.Layout()
		sbSizer1.Fit( self.ImmediateCommandEntries )
		CommandsMainBoxSizer.Add( self.ImmediateCommandEntries, 4, wx.EXPAND |wx.ALL, 5 )

		self.CmdHistPanel = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( self.CmdHistPanel, wx.ID_ANY, u"Command History" ), wx.HORIZONTAL )

		CmdHistListBoxChoices = []
		self.CmdHistListBox = wx.ListBox( sbSizer2.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, CmdHistListBoxChoices, 0|wx.NO_BORDER|wx.TAB_TRAVERSAL )
		sbSizer2.Add( self.CmdHistListBox, 1, wx.ALL|wx.EXPAND, 5 )


		self.CmdHistPanel.SetSizer( sbSizer2 )
		self.CmdHistPanel.Layout()
		sbSizer2.Fit( self.CmdHistPanel )
		CommandsMainBoxSizer.Add( self.CmdHistPanel, 4, wx.EXPAND |wx.ALL, 5 )

		self.CmdHistControlsPanel = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( self.CmdHistControlsPanel, wx.ID_ANY, u"Command History Controls" ), wx.HORIZONTAL )

		self.CmdHistSearchButton = wx.Button( sbSizer3.GetStaticBox(), wx.ID_ANY, u"Search", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer3.Add( self.CmdHistSearchButton, 1, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

		self.CmdHistSearchTextCtl = wx.TextCtrl( sbSizer3.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer3.Add( self.CmdHistSearchTextCtl, 3, wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

		self.CmdHistClearButton = wx.Button( sbSizer3.GetStaticBox(), wx.ID_ANY, u"Clear History", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer3.Add( self.CmdHistClearButton, 1, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )


		self.CmdHistControlsPanel.SetSizer( sbSizer3 )
		self.CmdHistControlsPanel.Layout()
		sbSizer3.Fit( self.CmdHistControlsPanel )
		CommandsMainBoxSizer.Add( self.CmdHistControlsPanel, 1, wx.EXPAND |wx.ALL, 5 )

		self.QuickCmdPanel = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		sbSizer4 = wx.StaticBoxSizer( wx.StaticBox( self.QuickCmdPanel, wx.ID_ANY, u"Quick Command" ), wx.HORIZONTAL )

		self.QuickCmdTextCtl = wx.TextCtrl( sbSizer4.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_PROCESS_ENTER )
		sbSizer4.Add( self.QuickCmdTextCtl, 2, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

		self.QuickCmdClearButton = wx.Button( sbSizer4.GetStaticBox(), wx.ID_ANY, u"Clear", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer4.Add( self.QuickCmdClearButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )

		self.QuickCmdSendButton = wx.Button( sbSizer4.GetStaticBox(), wx.ID_ANY, u"Send", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer4.Add( self.QuickCmdSendButton, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )


		self.QuickCmdPanel.SetSizer( sbSizer4 )
		self.QuickCmdPanel.Layout()
		sbSizer4.Fit( self.QuickCmdPanel )
		CommandsMainBoxSizer.Add( self.QuickCmdPanel, 1, wx.EXPAND |wx.ALL, 5 )


		self.SetSizer( CommandsMainBoxSizer )
		self.Layout()

		# Connect Events
		self.CmdsComboBox.Bind( wx.EVT_CHAR, self.onCharCmdComboBox )
		self.CmdsComboBox.Bind( wx.EVT_COMBOBOX, self.onCmdsComboBoxSelect )
		self.CmdsComboBox.Bind( wx.EVT_KEY_DOWN, self.onKeyDownCmdComboBox )
		self.CmdsComboBox.Bind( wx.EVT_TEXT, self.onTextCmdComboBox )
		self.CmdsComboBox.Bind( wx.EVT_TEXT_ENTER, self.onTextEnterCmdComboBox )
		self.CmdSendButton.Bind( wx.EVT_BUTTON, self.onCmdSendButtonClick )
		self.CmdHistListBox.Bind( wx.EVT_LISTBOX, self.onListBoxItemSelect )
		self.CmdHistSearchButton.Bind( wx.EVT_BUTTON, self.onCmdHistSearchButtonClick )
		self.CmdHistClearButton.Bind( wx.EVT_BUTTON, self.onCmdHistClearButtonClick )
		self.QuickCmdTextCtl.Bind( wx.EVT_CHAR, self.onCharQuickCmd )
		self.QuickCmdTextCtl.Bind( wx.EVT_KEY_UP, self.onKeyUpQuickCmd )
		self.QuickCmdTextCtl.Bind( wx.EVT_TEXT, self.onTextQuickCmd )
		self.QuickCmdTextCtl.Bind( wx.EVT_TEXT_ENTER, self.onQuickCmdTextCtrlEnterPressed )
		self.QuickCmdClearButton.Bind( wx.EVT_BUTTON, self.onQuickCmdClearButtonClick )
		self.QuickCmdSendButton.Bind( wx.EVT_BUTTON, self.onQuickCmdSendButtonClick )

	def __del__( self ):
		pass


	# Virtual event handlers, overide them in your derived class
	def onCharCmdComboBox( self, event ):
		event.Skip()

	def onCmdsComboBoxSelect( self, event ):
		event.Skip()

	def onKeyDownCmdComboBox( self, event ):
		event.Skip()

	def onTextCmdComboBox( self, event ):
		event.Skip()

	def onTextEnterCmdComboBox( self, event ):
		event.Skip()

	def onCmdSendButtonClick( self, event ):
		event.Skip()

	def onListBoxItemSelect( self, event ):
		event.Skip()

	def onCmdHistSearchButtonClick( self, event ):
		event.Skip()

	def onCmdHistClearButtonClick( self, event ):
		event.Skip()

	def onCharQuickCmd( self, event ):
		event.Skip()

	def onKeyUpQuickCmd( self, event ):
		event.Skip()

	def onTextQuickCmd( self, event ):
		event.Skip()

	def onQuickCmdTextCtrlEnterPressed( self, event ):
		event.Skip()

	def onQuickCmdClearButtonClick( self, event ):
		event.Skip()

	def onQuickCmdSendButtonClick( self, event ):
		event.Skip()

