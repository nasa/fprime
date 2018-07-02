import wx
class ArgItemTextCtl(wx.Panel):
	def __init__(self, parent, validator, label):
		wx.Panel.__init__( self, parent, id = wx.ID_ANY )

		#self.m_panel = wx.Panel( parent, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer = wx.BoxSizer( wx.VERTICAL )
		
		self.m_argStaticText = wx.StaticText( self, wx.ID_ANY, label, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_argStaticText.Wrap( -1 )
		bSizer.Add( self.m_argStaticText, 0, wx.ALL, 5 )
		
		self.m_argWindow = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0, validator)
		bSizer.Add( self.m_argWindow, 0, wx.ALL, 5 )
		
		
		self.SetSizer( bSizer )
		self.Layout()
		bSizer.Fit( self )

		self.m_argWindow.Bind(wx.EVT_KILL_FOCUS, self.onLeaveTextCtlInput)

	def onLeaveTextCtlInput(self, event):
		self.Validate()
		
class HexIntegerValidator(wx.Validator):

	def __init__(self):
		wx.Validator.__init__(self)

	def Clone(self):
		return HexIntegerValidator()

	def Validate(self):
		textCtrl = self.GetWindow()
		text = textCtrl.GetValue()

		try:
			int(text)
			textCtrl.SetBackgroundColour("white")
			return True
		except ValueError:
			try:
				int(text, 16)
				return True
			except ValueError:
				wx.MessageBox("Argument: " + text + " is not decimal or hexidecimal. This argument must be a valid integer", "Error")
				textCtrl.SetBackgroundColour("pink")
				textCtrl.Refresh()
				return False


	def TransferToWindow(self):
		return True

	def TransferFromWindow(self):
		return True

class RealValidator(wx.Validator):

	def __init__(self):
		wx.Validator.__init__(self)

	def Clone(self):
		return RealValidator()

	def Validate(self, win):
		textCtrl = self.GetWindow()
		text = textCtrl.GetValue()

		try:
			float(text)
			textCtrl.SetBackgroundColour("white")
			return True
		except ValueError:
			wx.MessageBox("Argument: " + text + " is not a real value. This argument must have a real value", "Error")
			textCtrl.SetBackgroundColour("pink")
			textCtrl.Refresh()
			return False


	def TransferToWindow(self):
		return True

	def TransferFromWindow(self):
		return True

