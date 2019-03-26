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
## Class MainFrame
###########################################################################

class MainFrame ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"FPrime Ground Station", pos = wx.DefaultPosition, size = wx.Size( 800,600 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )
		
		MainSizer = wx.BoxSizer( wx.VERTICAL )
		
		self.TabNotebook = wx.Notebook( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, 0 )
		
		
		MainSizer.Add( self.TabNotebook, 1, wx.EXPAND |wx.ALL, 5 )
		
		
		self.SetSizer( MainSizer )
		self.Layout()
		#self.GroundStationStatusBar = self.CreateStatusBar( 3, wx.STB_SIZEGRIP, wx.ID_ANY )
		self.m_menubar1 = wx.MenuBar( 0 )
		self.FileMenu = wx.Menu()
		self.NewMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"New...", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.NewMenuItem )
		
		self.SaveMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"Save...", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.SaveMenuItem )
		
		self.LoadMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"Load...", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.LoadMenuItem )
		
		self.AboutMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"About...", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.AboutMenuItem )
		
		self.SaveWinCfgMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"Save Window Configuration", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.SaveWinCfgMenuItem )
		
		self.RestoreWinMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"Restore Windows", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.RestoreWinMenuItem )
		
		self.ExitMenuItem = wx.MenuItem( self.FileMenu, wx.ID_ANY, u"Exit", wx.EmptyString, wx.ITEM_NORMAL )
		self.FileMenu.Append( self.ExitMenuItem )
		
		self.m_menubar1.Append( self.FileMenu, u"File" ) 
		
		self.SetMenuBar( self.m_menubar1 )
		
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.Bind( wx.EVT_CLOSE, self.onMainFrameClose )
		self.Bind( wx.EVT_MENU, self.onNewMenuItemClick, id = self.NewMenuItem.GetId() )
		self.Bind( wx.EVT_MENU, self.onSaveMenuItemClick, id = self.SaveMenuItem.GetId() )
		self.Bind( wx.EVT_MENU, self.onLoadMenuItemClick, id = self.LoadMenuItem.GetId() )
		self.Bind( wx.EVT_MENU, self.onAboutMenuItemClick, id = self.AboutMenuItem.GetId() )
		self.Bind( wx.EVT_MENU, self.onSaveWinCfgMenuItemClick, id = self.SaveWinCfgMenuItem.GetId() )
		self.Bind( wx.EVT_MENU, self.onRestoreWinMenuItemClick, id = self.RestoreWinMenuItem.GetId() )
		self.Bind( wx.EVT_MENU, self.onExitMenuItemClick, id = self.ExitMenuItem.GetId() )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def onMainFrameClose(self, event ):
		event.Skip()

	def onNewMenuItemClick( self, event ):
		event.Skip()
	
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
		event.Skip()
	

