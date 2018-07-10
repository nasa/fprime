import wx
import GDSCommandPanelGUI
import GDSArgItemTextCtl
import GDSArgItemComboBox
from models.serialize import u32_type
from models.serialize import bool_type, enum_type


###########################################################################
## Class CommandsImpl
###########################################################################

class CommandsImpl (GDSCommandPanelGUI.Commands):

	'''TODO remove client debugging''' 
	def __init__( self, parent, client, cname_dict ):
		GDSCommandPanelGUI.Commands.__init__ ( self, parent)

		self.client = client
		self.cname_dict = cname_dict
		self.CmdsComboBox.AppendItems(sorted(self.cname_dict.keys()))

		self.argname2arginput = dict()
	def __del__( self ):
		pass


	# Override these handlers to implement functionality for GUI elements
	def onCmdsComboBoxSelect( self, event ):
		self.argname2arginput = dict()
		self.CmdArgsScrolledWindow.GetSizer().Clear(True)

		s = self.CmdsComboBox.GetStringSelection()
		temp = self.cname_dict[s]
		width_total = 0
		for (arg_name, _, arg_type) in temp.arguments:
			if type(arg_type) == bool_type.BoolType:
				k = GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], arg_name)
				self.argname2arginput[arg_name] = k
				self.CmdArgsScrolledWindow.GetSizer().Add(k)
				w, _ = k.GetSizer().GetMinSize()
				width_total += w
			elif type(arg_type) == enum_type.EnumType:
				k = GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, arg_type.keys(), arg_name)
				self.argname2arginput[arg_name] = k
				self.CmdArgsScrolledWindow.GetSizer().Add(k)
				w, _ = k.GetSizer().GetMinSize()
				width_total += w
			else:
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), arg_name)
				self.argname2arginput[arg_name] = k
				self.CmdArgsScrolledWindow.GetSizer().Add(k)
				w, _ = k.GetSizer().GetMinSize()
				width_total += w
		
		
		self.CmdArgsScrolledWindow.Layout()
		self.CmdArgsScrolledWindow.SetVirtualSize((width_total, -1))
		#self.CmdArgsScrolledWindow.GetSizer().Fit(self.CmdArgsScrolledWindow)
		self.CmdArgsScrolledWindow.Refresh()

	def onCmdSendButtonClick( self, event ):
		# TODO remove debugging 
		op_code = u32_type.U32Type(0x79)
		cmd_data = op_code.serialize()
		desc = u32_type.U32Type(0x5A5A5A5A)
		desc_type = u32_type.U32Type(0)
		data_len = u32_type.U32Type(len(cmd_data) + desc_type.getSize())
		self.client.send("A5A5 FSW " + desc.serialize() + data_len.serialize() + desc_type.serialize() + cmd_data)

	def onCmdHistSearchButtonClick( self, event ):
		event.Skip()

	def onCmdHistClearButtonClick( self, event ):
		event.Skip()

	def onQuickCmdClearButtonClick( self, event ):
		event.Skip()

	def onQuickCmdSendButtonClick( self, event ):
		event.Skip()


