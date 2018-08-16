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
## Class Status
###########################################################################

class Status ( wx.Panel ):
	
	def __init__( self, parent ):
		wx.Panel.__init__ ( self, parent, id = wx.ID_ANY, pos = wx.DefaultPosition, size = wx.Size( 800,600 ), style = wx.TAB_TRAVERSAL )
		
		bSizer7 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer3 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_staticText1 = wx.StaticText( self, wx.ID_ANY, u"Data Recieved", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText1.Wrap( -1 )
		bSizer3.Add( self.m_staticText1, 1, wx.ALL, 5 )
		
		self.m_staticText2 = wx.StaticText( self, wx.ID_ANY, u"Data Sent", wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_RIGHT )
		self.m_staticText2.Wrap( -1 )
		bSizer3.Add( self.m_staticText2, 1, wx.ALL, 5 )
		
		
		bSizer7.Add( bSizer3, 0, wx.EXPAND, 5 )
		
		bSizer2 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.StatusTabRecvTextCtl = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_MULTILINE|wx.TE_READONLY )
		bSizer2.Add( self.StatusTabRecvTextCtl, 1, wx.ALL|wx.EXPAND, 5 )
		
		self.StatusTabSendTextCtl = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_MULTILINE|wx.TE_READONLY )
		bSizer2.Add( self.StatusTabSendTextCtl, 1, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer7.Add( bSizer2, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer7 )
		self.Layout()
	
	def __del__( self ):
		pass
	

