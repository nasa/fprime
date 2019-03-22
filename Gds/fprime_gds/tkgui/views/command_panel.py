'''
Created on Dec 9, 2014

@author: reder
'''
import Tkinter
import Pmw
import time

from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import status_bar_updater
from fprime_gds.tkgui.controllers import command_loader
from fprime_gds.tkgui.controllers import commander

from fprime_gds.common.models.common import command
from fprime_gds.tkgui.utils import tkGui_misc
from fprime_gds.tkgui.utils import command_args

import exceptions
import command_args_frame
from fprime.common.models.serialize.i32_type import *


@commander.mediated
class CommandPanel(object):
    '''
    A class that instances and communicates with a command
    panel that maintains history of commands.
    '''
    def __init__(self, parent, top, opt):
        '''
        Constructor
        '''
        # The parent notebook widget
        self.__parent = parent
        # The main top widget with the menues and status text
        self.__top = top
        #
        self.__start_time = None
        # Setup command dictionary of command objects here.
        self.__cmds = command_loader.CommandLoader.getInstance()
        self.__cmds_dict = self.__cmds.getCommandDict()
        #
        # This keeps a history list of command objects
        #
        self.__cmds_history_list = []
        #
        # Create an args frame here to generate the entries for each command.
        #
        self.__args_frame = command_args_frame.CommandArgsFrame()
        #
        # Status updater and status bar updater singletons
        #
        self.__status_update     = status_updater.StatusUpdater.getInstance()
        self.__status_bar_update = status_bar_updater.StatusBarUpdater.getInstance()

        #
        # Container Frame
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)
        f.config(bg="lightblue")

        f4 = Tkinter.LabelFrame(f, text="Quick Command", padx=10, pady=10)
        f4.pack(side=Tkinter.BOTTOM,fill=Tkinter.BOTH, expand=0, padx=5,pady=5)

        b11=Tkinter.Button(f4, text="Send", width=4, command=self._parse_entry)
        b11.pack(side=Tkinter.RIGHT, anchor=Tkinter.N, padx=10,pady=10)

        b12=Tkinter.Button(f4, text="Clear", width=4, command=self.clear_entry_button)
        b12.pack(side=Tkinter.RIGHT, anchor=Tkinter.N, padx=10,pady=10)

        self.quick_entry = Tkinter.Entry(f4, width=70)
        self.quick_entry.bind("<Button-1>", self.clear_entry)
        self.quick_entry.bind("<Return>", self._parse_entry_key)
        self.quick_entry.bind("<Escape>", self.clear_entry)
        self.quick_entry.pack(side=Tkinter.LEFT, anchor=Tkinter.N, padx=10,pady=10)
        self.quick_entry.insert(0, "Command Mneumonic, arg_1, arg_2, arg_3, ..., arg_n")
        self.quick_entry['fg']='gray'

        self.to_clear = True

        #
        # Build command history controls here
        #
        f3 = Tkinter.LabelFrame(f, text="Command History Controls", padx=5, pady=5)
        f3.pack(side=Tkinter.BOTTOM, anchor=Tkinter.N, fill=Tkinter.X, expand=0, padx=5, pady=5)
        #
        b2=Tkinter.Button(f3, text="Search", command=self.searchHistory)
        b2.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)
        self.__search_entry = Pmw.EntryField(f3, validate=None)
        self.__search_entry.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1, padx=10, pady=10)
        b3=Tkinter.Button(f3, text='Clear History', command=self.clearHistory)
        b3.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)
        self.__search_index = 0

        #
        # Build immediate command entry area here
        #
        f1 = Tkinter.LabelFrame(f, text="Immediate Command Entries", relief=Tkinter.GROOVE, borderwidth=2, padx=5, pady=5)
        f1.pack(side=Tkinter.TOP, anchor=Tkinter.NW, fill=Tkinter.BOTH, expand=0,padx=10,pady=10)

        #
        # Send immediate command here and add to history
        #
        b1=Tkinter.Button(f1, text="Send", width=4, command=self.send)
        b1.pack(side=Tkinter.RIGHT, anchor=Tkinter.N, padx=10,pady=10)
        # Help text
        self.__status_bar_update.register_help(b1,"Send command to target application.")

        #
        # Command combobox
        #
        self.__cmd_variable = Tkinter.StringVar()
        self.__cmds_list = ["NOOP"]
        self.__combobox = Pmw.ComboBox(f1, label_text='Cmds:', labelpos='wn',
                                entry_width=4, dropdown=1,
                                selectioncommand=self._choseEntry,
                                scrolledlist_items=self.__cmds_list, entry_textvariable=self.__cmd_variable)
        self.__combobox.pack(side=Tkinter.LEFT, anchor=Tkinter.N, padx=8, pady=8)
        self.__combobox.selectitem(self.__cmds_list[0])
        self.__selected_cmd_mnemonic = self.__cmds_list[0]

        # Rebind Up and Down key events to prevent exception
        tkGui_misc.rebind_comboBox(self.__combobox)
        # Rebind for auto complete
        self.__combobox._entryWidget.bind("<Key>", self.__combobox_entry)
        self.__combobox._entryWidget.bind("<Return>", self.__combobox_return)
        self.__combobox._arrowBtn.bind("<Button-1>", self.__combobox_press)

        #
        # Command arguments frame here
        #
        self.__args_pane = Tkinter.LabelFrame(f1, text="Command Args:")
        self.__args_pane.pack(side=Tkinter.LEFT, fill=Tkinter.X, expand=1, anchor=Tkinter.N)



        #
        # Build command history list here
        #
        f2 = Tkinter.LabelFrame(f, text="Command History", padx=5, pady=5)
        f2.pack(side=Tkinter.BOTTOM,fill=Tkinter.BOTH, expand=1, padx=10,pady=10, anchor=Tkinter.N)
        #
        self.__list_box = Pmw.ScrolledListBox(f2, listbox_selectmode=Tkinter.SINGLE,
                                  hull_height=120,
                                  selectioncommand=self._singleClick_handler, dblclickcommand=self._doubleClick_handler,
                                  usehullsize=1)
        self.__list_box.pack(side=Tkinter.TOP, anchor=Tkinter.NW,
                             fill=Tkinter.BOTH, expand=1, padx=5, pady=5)
        self.__opt = opt

    def __combobox_entry(self, event):
        """
        Search for entry
        """
        display_list = []
        current = self.__cmd_variable.get()
        for cmd in self.__cmds_list:
            if current in cmd:
                display_list.append(cmd)
        self.__combobox._list.setlist(display_list)

        self.__top.root().update_idletasks()

    def __combobox_return(self, event):
        """
        Display selection when return is pressed.
        """
        self.__combobox.invoke()

    def __combobox_press(self, event):
        """
        Reset list when arrow button in pressed.
        """
        self.__combobox._list.setlist(self.__cmds_list)
        self.__combobox.invoke()



    def _choseEntry(self, entry):
        self.__selected_cmd_mnemonic = entry
        #print 'You chose command "%s"' % entry
        self._updateArgUI(self.__selected_cmd_mnemonic)


    def _updateArgUI(self, mnemonic):
        """
        Update the arguments UI entries for a command mnemonic.
        """
        self.__args_frame.destroy()
        #self.__parent.update()
        self.__args_ui_panel_dict = self.__args_frame.create(mnemonic, self.__args_pane)
        self.__parent.update()

    def _singleClick_handler(self):
        """
        Single click restores the command entry
        """
        self.__list_box._listbox.after(175, self._do_selection)

    def _doubleClick_handler(self):
        """
        Double click restores the command entry
        and sends the command. Only send command
        if something is selected.
        """
        self._do_selection()
        if self._has_selection():
            self.send()

    def setCmdList(self, cmds):
        """
        Set up a new list of commands.
        """
        max_len = 0
        for cmd in cmds:
            if len(cmd) > max_len:
                max_len = len(cmd)
        self.__cmds_list = cmds
        self.__cmds_list.sort()
        self.__combobox.configure(entry_width = max_len)
        self.__combobox.setlist(self.__cmds_list)
        self.__combobox.selectitem(self.__cmds_list[0], setentry=True)
        self.__selected_cmd_mnemonic = self.__cmds_list[0]
        # Select first item in list so arguments are displayed in ArgUI
        self.__combobox._selectCmd()

    def appendCmdList(self, cmd):
        """
        Adds a new command to the list.
        """
        self.__cmds_list.append(cmd)
        self.__cmds_list.sort()
        max_len = 0
        for cmd in self.__cmds_list:
            if len(cmd) > max_len:
                max_len = len(cmd)
        self.__combobox.configure(entry_width = max_len)
        self.__combobox.setlist(self.__cmds_list)
    #@todo: Will need to add series of command list interface.
    #@todo: How do commands get associated with arguments.



    def _has_selection(self):
        """
        Check if ScrolledListbox has a selection.
        """
        sels = self.__list_box.getcurselection()
        if len(sels) == 0:
            return False

        return True


    def _do_selection(self):
        """
        Get selection from listbox and update widget values.
        But only if there is something selected.
        """
        if self._has_selection() is False:
            pass
            #print 'No selection'
        else:
            sels = self.__list_box.getcurselection()
            #print 'Selection:', sels[0]
            # get the new stored command object
            i = self.__list_box.component('listbox').index(Tkinter.ACTIVE)
            mnemonic = self.__cmds_history_list[i].getMnemonic()
            cmd_obj = self.__cmds_history_list[i]
            #print mnemonic, cmd_obj.getMnemonic(), i
            # change command mnemonic pull down to select command
            self.__combobox.selectitem(self.__cmds_list[self.__cmds_list.index(mnemonic)])
            self.__selected_cmd_mnemonic = mnemonic
            # change the entry types to match mnemonic
            self._updateArgUI(mnemonic)
            # update values in entries and in singleton dictionary
            for arg in cmd_obj.getArgs():
                arg_name, arg_desc, arg_type = arg
                e = self.__args_ui_panel_dict[arg_name]
                # EntryField
                if 'extravalidators' in e.configure().keys():
                    e.setvalue( "%s" % arg_type.val )
                # Combo
                elif 'selectioncommand' in e.configure().keys():
                    v = arg_type.val
                    if v == True:
                        v = "True"
                    elif v == False:
                        v = "False"
                    e.selectitem(v)
                else:
                    raise exceptions.ValueError
            #
            self.__cmds.setCommandObj(mnemonic, cmd_obj)
            #


    def appendHistoryItem(self, cmd_obj):
        """
        Append a command into history list.
        """
        # Added command object to history
        new_cmd_obj = command.Command(cmd_obj.getComponent(),
                                             cmd_obj.getMnemonic(),
                                             cmd_obj.getOpCode(),
                                             cmd_obj.getDescription(),
                                             cmd_obj.getArgs())
        self.__cmds_history_list.append(new_cmd_obj)
        mnemonic = cmd_obj.getMnemonic()
        opcode = cmd_obj.getOpCode()
        args = cmd_obj.getArgs()
        #
        cmd = "%s (0x%x)" % (mnemonic,int(opcode))
        for arg in args:
            cmd += ", %s" % arg[2].val

        self.__list_box.insert(Tkinter.END, cmd)
        self.__list_box.see(Tkinter.END)


    def _create_args(self, cmd_obj):
        """
        Command interfact to command_args_frame to instance
        entries for commands.
        """
        arg_num = 1
        for arg in cmd_obj.getArgs():
            arg_name, arg_desc, arg_type = arg
            e = self.__args_ui_panel_dict[arg_name]
            v = e.component('entry').get()
            # Handle hex value
            if "0x" in v:
                v = int(v, 16)

            try:
               new_obj = command_args.create_arg_type(arg_name, arg_type, v)
               if isinstance(new_obj.val, str) and len(new_obj.val) == 0:
                   #self.__status_update.clear()
                   self.__status_update.update('No input provided for command argument %d'%arg_num, 'red')
                   return None

            except ValueError:
                   #self.__status_update.clear()
                   self.__status_update.update('No input provided for command argument %d'%arg_num, 'red')
                   return None
            cmd_obj.setArg(arg_name, new_obj)
            arg_num += 1
        #
        #self.__cmds_dict[self.__selected_cmd_mnemonic] = cmd_objself
        #self.__cmds.setCommandDict(self.__cmds_dict)
        self.__cmds.setCommandObj(self.__selected_cmd_mnemonic, cmd_obj)
        return cmd_obj


    def send(self):
        """
        Command send button callback.
        """
        #cmd_objself = self.__cmds_dict[self.__selected_cmd_mnemonic]
        cmd_objself = self.__cmds.getCommandObj(self.__selected_cmd_mnemonic)
        #print cmd_objself.getMnemonic(), self.__selected_cmd_mnemonic
        #
        cmd_objself = self._create_args(cmd_objself)
        if cmd_objself != None:
           # Added by decorator - sends command object
           self.on_change(cmd_objself)
           #
           self.appendHistoryItem(cmd_objself)
           #self.__status_update.clear()


    def clearHistory(self):
        """
        Clear the command history list here...
        """
        del self.__cmds_history_list
        self.__cmds_history_list = []
        self.__list_box.clear()


    def searchHistory(self):
        """
        Search the current history list here...
        """
        mnemonic_target = self.__search_entry.get()
        mnemonic_list = [obj_cmd.getMnemonic() for obj_cmd in self.__cmds_history_list]
        if mnemonic_target in mnemonic_list:
            #i = mnemonic_list.index(mnemonic_target)
            search_ids = [i for i,j in enumerate(mnemonic_list) if j == mnemonic_target]
            if self.__search_index >= len(search_ids):
                self.__search_index = 0
            i = search_ids[self.__search_index]
            #
            self.__args_frame.destroy()
            self.__combobox.selectitem(mnemonic_target)
            self.__list_box.component('listbox').selection_clear(0, Tkinter.END)
            #
            self.__list_box.component('listbox').activate(i)
            self.__list_box.component('listbox').selection_set(i)
            self.__list_box.component('listbox').see(i)
            self.selectionCommand(False)
            self.__search_index += 1
            if self.__search_index >= len(search_ids):
                self.__search_index = 0
        else:
            self.__search_index = 0
            self.__status_update.update("Cannot find search mnemonic: %s" % mnemonic_target)
            print "Cannot find search mnemonic: %s" % mnemonic_target

    def _parse_entry(self):
        text = self.quick_entry.get()

        items = text.split(",")
        items = [a.strip(" ") for a in items]

        opcode = items[0]
        args = items[1:]
        
        cmd_objself = None

        try:
            cmd_objself = self.__cmds.getCommandObj(opcode)
            cmd_objself = self._parse_args(cmd_objself, opcode, args)
        except KeyError:
            self.__status_update.update('Invalid command mneumonic provided: %s'%opcode, 'red')

        if cmd_objself != None:
            self.quick_entry['bg'] = 'white'
            self.on_change(cmd_objself)
           
            self.appendHistoryItem(cmd_objself)

            self.__status_update.update('')
            self.clear_entry(None)
        else:
            self.quick_entry['bg'] = 'red'

    def _parse_entry_key(self, event):
        self._parse_entry()
            

    def _parse_args(self, cmd_obj, opcode, args):
        arg_num = 1
        args_to_parse = cmd_obj.getArgs()

        if(len(args_to_parse)!=len(args)):
            self.__status_update.update('Incorrect number of arguments provided: %d required'%len(args_to_parse), 'red')
            return None

        for arg in args_to_parse:
            arg_name, arg_desc, arg_type = arg
            
            v = args[arg_num-1]
            # Handle hex value
            if "0x" in v:
                v = int(v, 16)

            try:
               new_obj = command_args.create_arg_type(arg_name, arg_type, v)
               if isinstance(new_obj.val, str) and len(new_obj.val) == 0:
                   #self.__status_update.clear()
                   self.__status_update.update('No input provided for command argument %d'%arg_num, 'red')
                   return None

            except ValueError:
                   #self.__status_update.clear()
                   self.__status_update.update('No input provided for command argument %d'%arg_num, 'red')
                   return None
            cmd_obj.setArg(arg_name, new_obj)
            arg_num += 1
        #
        #self.__cmds_dict[self.__selected_cmd_mnemonic] = cmd_objself
        #self.__cmds.setCommandDict(self.__cmds_dict)
        self.__cmds.setCommandObj(opcode, cmd_obj)
        return cmd_obj

    def clear_entry(self, event):
        if(self.to_clear):
            self.quick_entry['fg'] = 'black'
            self.quick_entry.delete(0,Tkinter.END)
            self.to_clear = False

    def clear_entry_button(self):
        self.to_clear = True
        self.clear_entry(None)
        
def main_window_start():
    """
    """
    root = Tkinter.Tk()
    #root.option_readfile('optionDB')
    root.title('F Prime Command Panel')
    Pmw.initialise()
    c=CommandPanel(root)
    c.setCmdList(["NOOP"])
    for i in range(30):
        c.appendCmdList("CMD%2d" % i)
    for i in range(60):
        c.appendHistoryItem("CMD%4d" % i)
    root.update()
    return root


if __name__ == "__main__":
    root = main_window_start()

    root.mainloop()
