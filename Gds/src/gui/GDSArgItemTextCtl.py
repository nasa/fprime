import wx

class ArgItemTextCtl(wx.Panel):
	'''Defines the GUI and funcitonality for the GUI element which accepts command arguments with a wx.TextCtrl'''

	def __init__(self, parent, validator, label):
		"""ArgItemTextCtl constructor
		
		Arguments:
			parent {wx.Window} -- The parent window for this UI element
			validator {wx.Validator} -- Validator object that will check if the TextCtrl entry is formated correctly
			label {string} -- Label for this GUI element. Usually the name of the argument.
		"""

		wx.Panel.__init__( self, parent, id = wx.ID_ANY )

		#self.m_panel = wx.Panel( parent, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer = wx.BoxSizer( wx.VERTICAL )
		
		self.m_argStaticText = wx.StaticText( self, wx.ID_ANY, label, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_argStaticText.Wrap( -1 )
		bSizer.Add( self.m_argStaticText, 0, wx.ALL, 5 )
		
		if validator is not None:
			self.m_argWindow = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0, validator)
		else:
			self.m_argWindow = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0)

		bSizer.Add( self.m_argWindow, 0, wx.ALL, 5 )
		self.SetSizer( bSizer )
		self.Layout()
		bSizer.Fit( self )

		self.m_argWindow.Bind(wx.EVT_KILL_FOCUS, self.onLeaveTextCtlInput)

	def setText(self, text):
		"""Set the contents of this gui's text entry value
		
		Arguments:
			text {string} -- the string to set
		"""

		self.m_argWindow.SetValue(str(text))

	def getText(self):
		"""Get the contents of this gui's text entry value
		
		Returns:
			string --the contents of the text control in this gui element
		"""

		return self.m_argWindow.GetLineText(0)

	def onLeaveTextCtlInput(self, event):
		"""Called when the user leaves the TextCtrl. Calls Validator to check input.
		
		Arguments:
			event {wx.Event} -- wx object passed by callback
		"""
		# NOTE This might be annoying - add it back if you want
		#self.Validate()
		
class HexIntegerValidator(wx.Validator):
	'''Validator which checks for hex or integer formating of entry.'''

	def __init__(self):
		wx.Validator.__init__(self)

	def Clone(self):
		return HexIntegerValidator()

	def Validate(self, win):
		"""Validates the text control contents as a hex or integer number
		
		Arguments:
			win {wx.Window} -- Parent window. Passed in automoatically
		
		Returns:
			bool -- True if correct format, False otherwise
		"""
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
	'''Validator that checks for real number formating of input'''

	def __init__(self):
		wx.Validator.__init__(self)

	def Clone(self):
		return RealValidator()

	def Validate(self, win):
		"""Validates the text control contents as a real number
		
		Arguments:
			win {wx.Window} -- Parent window. Passed in automoatically
		
		Returns:
			bool -- True if correct format, False otherwise
		"""

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

class StringValidator(wx.Validator):
	'''Validator that checks for string formating of input'''

	def __init__(self):
		wx.Validator.__init__(self)

	def Clone(self):
		return StringValidator()

	def Validate(self, win):
		"""Validates the text control contents as a string
		
		Arguments:
			win {wx.Window} -- Parent window. Passed in automoatically
		
		Returns:
			bool -- True if correct format, False otherwise
		"""

		textCtrl = self.GetWindow()
		text = textCtrl.GetValue()


		if text is not u'':
			textCtrl.SetBackgroundColour("white")
			return True
		else:
			wx.MessageBox("Need an value for string arg", "Error")
			textCtrl.SetBackgroundColour("pink")
			textCtrl.Refresh()
			return False

	def TransferToWindow(self):
		return True

	def TransferFromWindow(self):
		return True
