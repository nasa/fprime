from __future__ import absolute_import
import wx
from . import GDSCommandPanelGUI
from . import GDSArgItemTextCtl
from . import GDSArgItemComboBox

from fprime.common.models.serialize.bool_type import *
from fprime.common.models.serialize.enum_type import *
from fprime.common.models.serialize.f32_type import *
from fprime.common.models.serialize.f64_type import *

from fprime.common.models.serialize.u8_type import *
from fprime.common.models.serialize.u16_type import *
from fprime.common.models.serialize.u32_type import *
from fprime.common.models.serialize.u64_type import *

from fprime.common.models.serialize.i8_type import *
from fprime.common.models.serialize.i16_type import *
from fprime.common.models.serialize.i32_type import *
from fprime.common.models.serialize.i64_type import *

from fprime.common.models.serialize.string_type import *
from fprime.common.models.serialize.serializable_type import *

from fprime_gds.common.data_types import cmd_data



from itertools import cycle

###########################################################################
## Class CommandsImpl
###########################################################################

class CommandsImpl (GDSCommandPanelGUI.Commands):
    '''Implementation file for the Command Panel GUI element'''

    def __init__( self, parent, cname_dict, config=None ):
        """Constructor for the Command Panel implementation

        Arguments:
            parent {wx.Window} -- Parent to this GUI element
            cname_dict {dictionary} -- A dictionary mapping command mneumonic names to the corresponding CmdTemplate object.
        """

        GDSCommandPanelGUI.Commands.__init__ ( self, parent)

        self.cname_dict = cname_dict
        
        for n in sorted(self.cname_dict.keys()):
            self.CmdsComboBox.Append(n)
        
        self.QuickCmdTextCtl.SetHint("cmd,arg1,arg2,\"string arg with spaces\"")

        self.arginputs = list()

        self._encoders = list()

        self._previous_search_term = None

        self._search_index_pool = None

        self._cmd_complete_search_pool = None


    def __del__( self ):
        pass

    def register_encoder(self, enc ):
        """Register an encoder object to this object. Encoder must implement data_callback(data)

        Arguments:
            enc {Encoder} -- The encoder to register
        """

        self._encoders.append(enc)

    def updateCmdSearchPool(self):
        """Updates the list of commands we are searching for in the command history
        """

        if self._previous_search_term is not None:
            itms = self.CmdHistListBox.Items
            idxs = [i for i, v in enumerate(itms) if self._previous_search_term in v]
            self._search_index_pool = cycle(idxs)

    def setupCommandArguments(self, temp):
        """Render the command argument gui elements to the screen based on selected command
        
        Arguments:
            temp {CmdTemplate} -- template object for the given command
        """

        self.arginputs = list()
        self.CmdArgsScrolledWindow.GetSizer().Clear(True)

        width_total = 0

        for (arg_name, _, arg_type) in temp.arguments:
            if type(arg_type) == BoolType:
                k = GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, ["True", "False"], arg_name, validator=GDSArgItemComboBox.ComboEnumValidator())
            elif type(arg_type) == EnumType:
                k = GDSArgItemComboBox.ArgItemComboBox(self.CmdArgsScrolledWindow, arg_type.keys(), arg_name, validator= GDSArgItemComboBox.ComboEnumValidator())
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
                k = GDSArgItemTextCtl.ArgItemTextCtl(self.CmdArgsScrolledWindow, GDSArgItemTextCtl.StringValidator(), arg_name)
            elif type(arg_type) == type(SerializableType()):
                pass

            self.arginputs.append(k)
            self.CmdArgsScrolledWindow.GetSizer().Add(k)
            w, _ = k.GetSizer().GetMinSize()
            width_total += w


        self.CmdArgsScrolledWindow.Layout()
        self.CmdArgsScrolledWindow.SetVirtualSize((width_total, -1))
        self.CmdArgsScrolledWindow.Refresh()

    # Override these handlers to implement functionality for GUI elements
    def onCmdsComboBoxSelect( self, event ):
        '''Set up the argument GUI elements for the command with the selected mneumonic'''
        s = self.CmdsComboBox.GetStringSelection()
        temp = self.cname_dict[s]
        self.setupCommandArguments(temp)

    def onCmdSendButtonClick( self, event ):
        '''Gathers entered command arguments and sends them to all encoders'''
        arglist = list()
        for i in self.arginputs:
            if i.Validate() == False:
                return False
            if type(i) == GDSArgItemTextCtl.ArgItemTextCtl:
                arglist.append(str(i.getText()))
            elif type (i) == GDSArgItemComboBox.ArgItemComboBox:
                arglist.append(i.getSelection())


        s = self.CmdsComboBox.GetStringSelection()
        if s is not u'':
            temp = self.cname_dict[s]
            data_obj = cmd_data.CmdData(tuple(arglist), temp)

            for i in self._encoders:
                i.data_callback(data_obj)

            self.CmdHistListBox.Append(str(data_obj), data_obj)
            self.CmdHistListBox.EnsureVisible(self.CmdHistListBox.Count - 1)
            self.updateCmdSearchPool()

        return True

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
        self.setupCommandArguments(itm_obj.template)
        self.CmdsComboBox.SetSelection(self.CmdsComboBox.Items.index(itm_obj.template.mnemonic))

        for k,v in zip(self.arginputs, itm_obj.get_args()):
            if type(k) == GDSArgItemTextCtl.ArgItemTextCtl:
                k.setText(v.val)
            else:
                k.setSelection(v.val)

    def onQuickCmdTextCtrlEnterPressed( self, event ):
        self.onQuickCmdSendButtonClick(event)

    def onQuickCmdSendButtonClick( self, event ):
        cmds = self.QuickCmdTextCtl.GetLineText(0)
        if sys.version_info < (3,0):
            cmds = cmds.encode('ascii', 'ignore')
        cmds = cmds.split(",")
        try:

            temp = self.cname_dict[cmds[0].strip()]
            for a in cmds[1:]:
                a.strip()
                a.replace('\"\"', '')

            data_obj = cmd_data.CmdData(tuple(cmds[1:]), temp)

            for i in self._encoders:
                i.data_callback(data_obj)

            self.CmdHistListBox.Append(str(data_obj), data_obj)
            self.CmdHistListBox.EnsureVisible(self.CmdHistListBox.Count - 1)
            self.updateCmdSearchPool()
        except KeyError:
            raise Exception("Command mneumonic is not valid")
        except IndexError:
            raise Exception("Malformed command string or some arguments not specified")

    def onCharQuickCmd( self, event ):
        event.Skip()

    def onKeyUpQuickCmd( self, event ):
        event.Skip()
    
    def onTextQuickCmd( self, event ):
        event.Skip()

    def onKeyDownCmdComboBox( self, event ):
        event.Skip()
    
    def onTextCmdComboBox( self, event ):
        ''' This the start of the autocomplete for the cmd combo box - finish if you want that feature and reach out to me if you want explanation (jxb@mit.edu)
        # Get current text in cbox
        text = self.CmdsComboBox.Value
        print text

        # Get current cursor position in cbox
        cpos = self.CmdsComboBox.GetInsertionPoint() + 1
        print cpos

        # Generate search term by ignoring everything after the insertion point
        search_term = text[:cpos]
        print search_term

        # Generate new cycler
        idxs = [i for i, v in enumerate(self.cname_dict.keys()) if v.startswith(search_term)]
        print(idxs)
        self._cmd_complete_search_pool = cycle(idxs)

        # Get first member in cycle
        idx = next(self._cmd_complete_search_pool)

        # Get the entry from cmd cbox that corresponds to this idx
        new_txt = self.CmdsComboBox.GetString(idx)

        # Set the cbox text to the new string
        self.CmdsComboBox.Value = new_txt

        # Set the insertion point and highlighting
        self.CmdsComboBox.SetInsertionPoint(cpos)
        self.CmdsComboBox.SetTextSelection(cpos, len(new_txt))
        '''
        event.Skip()
    
    def onTextEnterCmdComboBox( self, event ):
        event.Skip()

    def onCharCmdComboBox( self, event ):
        '''
        keycode = event.GetKeyCode()
        print keycode

        # Backspace pressed
        if keycode != 8:
            print self.CmdsComboBox.Value
        '''
        event.Skip()
