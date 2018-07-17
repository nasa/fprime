import wx
import GDSCommandPanelGUI
import GDSArgItemTextCtl
import GDSArgItemComboBox

from models.serialize.bool_type import *
from models.serialize.enum_type import *
from models.serialize.f32_type import *
from models.serialize.f64_type import *

from models.serialize.u8_type import *
from models.serialize.u16_type import *
from models.serialize.u32_type import *
from models.serialize.u64_type import *

from models.serialize.i8_type import *
from models.serialize.i16_type import *
from models.serialize.i32_type import *
from models.serialize.i64_type import *

from models.serialize.string_type import *
from models.serialize.serializable_type import *

from data_types import cmd_data

from pprint import pprint

from itertools import cycle

###########################################################################
## Class CommandsImpl
###########################################################################

class CommandsImpl (GDSCommandPanelGUI.Commands):
	'''Implementation file for the Command Panel GUI element'''

	def __init__( self, parent, cname_dict ):
		"""Constructor for the Command Panel implementation
		
		Arguments:
			parent {wx.Window} -- Parent to this GUI element
			cname_dict {dictionary} -- A dictionary mapping command mneumonic names to the corresponding CommandTemplate object.
		"""

		GDSCommandPanelGUI.Commands.__init__ ( self, parent)

		self.cname_dict = cname_dict
		self.CmdsComboBox.AppendItems(sorted(self.cname_dict.keys()))

		self.arginputs = list()

		self._encoders = list()

		self._previous_search_term = None

		self._search_index_pool = None

	def __del__( self ):
		pass

	def register_encoder(self, enc ):
		"""Register an encoder object to this object. Encoder must implement data_callback(data)
		
		Arguments:
			enc {Encoder} -- The encoder to register
		"""

		self._encoders.append(enc)

	# Cancel scroll button check if you get a scroll event on the window
	def updateCmdSearchPool(self):
		if self._previous_search_term is not None:
			itms = self.CmdHistListBox.Items
			idxs = [i for i, v in enumerate(itms) if self._previous_search_term in v]
			self._search_index_pool = cycle(idxs)

	# Override these handlers to implement functionality for GUI elements
	def onCmdsComboBoxSelect( self, event ):
		'''Set up the argument GUI elements for the command with the selected mneumonic'''
		self.arginputs = list()
		self.CmdArgsScrolledWindow.GetSizer().Clear(True)

		s = self.CmdsComboBox.GetStringSelection()
		temp = self.cname_dict[s]
		width_total = 0
		
		for (arg_name, _, arg_type) in temp.arguments:
			if type(arg_type) == BoolType:
				k = GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], arg_name)
			elif type(arg_type) == EnumType:
				k = GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, arg_type.keys(), arg_name)
			elif type(arg_type) == type(F64Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), arg_name)
			elif type(arg_type) == type(F32Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.RealValidator(), arg_name)
			elif type(arg_type) == type(I64Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(I32Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(I16Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(I8Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(U64Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(U32Type()):
			   	k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(U16Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(U8Type()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.HexIntegerValidator(), arg_name)
			elif type(arg_type) == type(StringType()):
				k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, None, arg_name)
			elif type(arg_type) == type(SerializableType()):
				pass
			
			self.arginputs.append(k)
			self.CmdArgsScrolledWindow.GetSizer().Add(k)
			w, _ = k.GetSizer().GetMinSize()
			width_total += w
		
		
		self.CmdArgsScrolledWindow.Layout()
		self.CmdArgsScrolledWindow.SetVirtualSize((width_total, -1))
		self.CmdArgsScrolledWindow.Refresh()

	def onCmdSendButtonClick( self, event ):
		'''Gathers entered command arguments and sends them to all encoders'''
		arglist = list()
		for i in self.arginputs:
			if type(i) == GDSArgItemTextCtl.ArgItemTextCtl:
				arglist.append(i.getText())
			elif type (i) == GDSArgItemComboBox.ArgItemComboBox:
				arglist.append(i.getSelection())

		s = self.CmdsComboBox.GetStringSelection()
		temp = self.cname_dict[s]
		data_obj = cmd_data.CmdData(tuple(arglist), temp)
		
		for i in self._encoders:
			i.data_callback(data_obj)

		self.CmdHistListBox.Append(str(data_obj), data_obj)
		self.CmdHistListBox.EnsureVisible(self.CmdHistListBox.Count - 1)
		self.updateCmdSearchPool()

	
	def onCmdHistSearchButtonClick( self, event ):

		if self.CmdHistSearchTextCtl.GetLineText(0) is not u'':
			if self.CmdHistSearchTextCtl.GetLineText(0) != self._previous_search_term:
				self._previous_search_term = self.CmdHistSearchTextCtl.GetLineText(0)
				self.updateCmdSearchPool()
				cidx = next(self._search_index_pool)
				self.CmdHistListBox.SetSelection(cidx)
			else:
				cidx = next(self._search_index_pool)
				self.CmdHistListBox.SetSelection(cidx)

	def onCmdHistClearButtonClick( self, event ):
		self.CmdHistListBox.Clear()

	def onQuickCmdClearButtonClick( self, event ):
		self.QuickCmdTextCtl.Clear()

	def onListBoxItemSelect( self, event ):
		itm_obj = self.CmdHistListBox.GetClientData(self.CmdHistListBox.GetSelection())
		pprint(itm_obj)

	def onQuickCmdTextCtrlEnterPressed( self, event ):
		self.onQuickCmdSendButtonClick(event)

	def onQuickCmdSendButtonClick( self, event ):
		cmds = self.QuickCmdTextCtl.GetLineText(0).split(",")

		try:

			temp = self.cname_dict[cmds[0]]
			data_obj = cmd_data.CmdData(tuple(cmds[1:]), temp)

			for i in self._encoders:
				i.data_callback(data_obj)

			self.CmdHistListBox.Append(str(data_obj), data_obj)
			self.CmdHistListBox.EnsureVisible(self.CmdHistListBox.Count - 1)
			self.updateCmdSearchPool()
		except KeyError:
			raise Exception("Command mneumonic is not valid")




