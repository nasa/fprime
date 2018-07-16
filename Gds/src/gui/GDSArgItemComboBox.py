import wx
class ArgItemComboBox(wx.Panel):
	'''Defines the GUI and funcitonality for the GUI element which accepts command arguments with a wx.ComboBox'''

	def __init__(self, parent, combo_options, label):
		"""ArgItemComboBox constructor
		
		Arguments:
			parent {wx.Window} -- The parent to this GUI element.
			combo_options {list} -- The options that the combobox will display
			label {string} -- The label to be displayed in the GUI element. Usually the name of the command argument.
		"""

		wx.Panel.__init__( self, parent, id = wx.ID_ANY )

		#self.m_panel = wx.Panel( parent, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer = wx.BoxSizer( wx.VERTICAL )
		
		self.m_argStaticText = wx.StaticText( self, wx.ID_ANY, label, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_argStaticText.Wrap( -1 )
		bSizer.Add( self.m_argStaticText, 0, wx.ALL, 5 )

		
		self.m_argWindow = wx.ComboBox( self, wx.ID_ANY, u"Combo!", wx.DefaultPosition, wx.DefaultSize, combo_options, wx.CB_READONLY )
		bSizer.Add( self.m_argWindow, 0, wx.ALL, 5 )

		self.SetSizer( bSizer )
		self.Layout()
		bSizer.Fit( self )

	def getSelection(self):
		"""Get the current selection of the combobox in this gui element
		
		Returns:
			string -- the current selection
		"""

		return self.m_argWindow.GetStringSelection()