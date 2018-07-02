import wx
class ArgItemComboBox(wx.Panel):
	def __init__(self, parent, combo_options, label):
		wx.Panel.__init__( self, parent, id = wx.ID_ANY )

		#self.m_panel = wx.Panel( parent, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer = wx.BoxSizer( wx.VERTICAL )
		
		self.m_argStaticText = wx.StaticText( self, wx.ID_ANY, label, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_argStaticText.Wrap( -1 )
		bSizer.Add( self.m_argStaticText, 0, wx.ALL, 5 )

		
		self.m_argWindow = wx.ComboBox( self, wx.ID_ANY, u"Combo!", wx.DefaultPosition, wx.DefaultSize, combo_options, 0 )
		bSizer.Add( self.m_argWindow, 0, wx.ALL, 5 )

		self.SetSizer( bSizer )
		self.Layout()
		bSizer.Fit( self )