'''
Created on March 18, 2015

@author: reder
'''
import Tkinter
import Pmw
import time

from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import command_loader
from fprime_gds.common.models.common import command
import exceptions

import command_args_frame

from fprime.common.models.serialize.i32_type import *
from fprime.common.models.serialize.u64_type import *
from fprime.common.models.serialize.u32_type import *
from fprime.common.models.serialize.u16_type import *
from fprime.common.models.serialize.u8_type import *
from fprime.common.models.serialize.time_type import *

from fprime_gds.tkgui.utils import tkGui_misc
from fprime_gds.tkgui.utils import command_args

import zlib


class SeqPanel(object):
    '''
    A class that instances and communicates with a sequencer
    panel that is used to create sequences of commands for
    execution by various types of sequencers.
    '''
    def __init__(self, parent, top):
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
        # This keeps a list of command objects
        #
        self.__seq_cmds_list = []
        #
        # Create an args frame here to generate the entries for each command.
        #
        self.__args_frame = command_args_frame.CommandArgsFrame()
        #
        # Status updater singleton
        #
        self.__status_update = status_updater.StatusUpdater.getInstance()
        #
        # Container Frame
        #
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)
        f.config(bg="lightblue")

        #
        # Build command history controls here
        f3 = Tkinter.LabelFrame(f, text="Sequence Editing Controls", padx=5, pady=5)
        f3.pack(side=Tkinter.BOTTOM, anchor=Tkinter.N, fill=Tkinter.X, expand=0, padx=5, pady=5)
        #
        b2=Tkinter.Button(f3, text="Search", command=self.searchHistory)
        b2.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)
        #
        self.__search_entry = Pmw.EntryField(f3, validate=None)
        self.__search_entry.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1, padx=10, pady=10)
        self.__search_entry.component('entry').bind("<Return>", self.searchHistory)
        #
        b3=Tkinter.Button(f3, text='Clear', command=self.clearHistory)
        b3.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)

        b4=Tkinter.Button(f3, text='Cut', command=self.cut)
        b4.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)

        b5=Tkinter.Button(f3, text='Copy', command=self.copy)
        b5.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)

        b6=Tkinter.Button(f3, text='Paste', command=self.paste)
        b6.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)


        #
        # Build immediate command entry area here
        #
        f1 = Tkinter.LabelFrame(f, text="Immediate Command Entries", relief=Tkinter.GROOVE, borderwidth=2, padx=5, pady=5)
        f1.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)
        #
        # Command combobox
        #
        self.__cmd_variable = Tkinter.StringVar()
        self.__cmds_list = ["NOOP"]
        self.__combobox = Pmw.ComboBox(f1, label_text='Cmds:', labelpos='wn',
                                listbox_width=24, dropdown=1,
                                selectioncommand=self._choseEntry,
                                scrolledlist_items=self.__cmds_list, entry_textvariable=self.__cmd_variable)
        self.__combobox.pack(side=Tkinter.LEFT, padx=8, pady=8, anchor=Tkinter.N)
        self.__combobox.selectitem(self.__cmds_list[0])
        self.__selected_cmd_mnemonic = self.__cmds_list[0]
        # Rebind combo box key events
        tkGui_misc.rebind_comboBox(self.__combobox)
        # Rebind for auto complete
        self.__combobox._entryWidget.bind("<Key>", self.__combobox_entry)
        self.__combobox._entryWidget.bind("<Return>", self.__combobox_return)
        self.__combobox._arrowBtn.bind("<Button-1>", self.__combobox_press)

        #
        # Command arguments frame here
        #
        self.__args_pane = Tkinter.LabelFrame(f1, text="Command Args:")
        self.__args_pane.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1, anchor=Tkinter.N)
        #
        # Add immediate command to sequence here
        #
        b1=Tkinter.Button(f1, text="Add", width=4, command=self.add)
        b1.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE, padx=10,pady=10)

        #
        # Build command history list here
        #
        f2 = Tkinter.LabelFrame(f, text="Sequence Under Construction", padx=5, pady=5)
        f2.pack(side=Tkinter.BOTTOM, fill=Tkinter.BOTH, anchor=Tkinter.N, expand=1, padx=10,pady=10)
        #
        self.__list_box = Pmw.ScrolledListBox(f2, listbox_selectmode=Tkinter.SINGLE,
                                  hull_height=120,
                                  selectioncommand=self.selectionCommand,
                                  usehullsize=1)
        self.__list_box.pack(side=Tkinter.TOP, anchor=Tkinter.N,
                             fill=Tkinter.BOTH, expand=1, padx=5, pady=5)


        #
        self.__search_index = 0

        self.__top.root().update_idletasks()


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
        self.__parent.update()
        self.__args_ui_panel_dict = self.__args_frame.create(mnemonic, self.__args_pane)
        self.__parent.update()


    def setCmdList(self, cmds):
        """
        Set up a new list of commands.
        """
        self.__cmds_list = cmds
        self.__cmds_list.sort()
        self.__combobox.setlist(self.__cmds_list)
        self.__combobox.selectitem(self.__cmds_list[0])
        self.__selected_cmd_mnemonic = self.__cmds_list[0]
        self._updateArgUI(self.__selected_cmd_mnemonic)


    def appendCmdList(self, cmd):
        """
        Adds a new command to the list.
        """
        self.__cmds_list.append(cmd)
        self.__combobox.setlist(self.__cmds_list)
    #@todo: Will need to add series of command list interface.
    #@todo: How do commands get associated with arguments.


    def __getSelectedObj(self):
        """
        Returns a tuple with selected listbox index
        and corresponding command object.
        """
        # get the new stored command object
        i = self.__list_box.component('listbox').index(Tkinter.ACTIVE)
        #mnemonic = self.__seq_cmds_list[i].getMnemonic()
        cmd_obj = self.__seq_cmds_list[i]
        #print mnemonic, cmd_obj.getMnemonic(), i
        return cmd_obj


    def selectionCommand(self):
        """
        Single click restores the command entry
        """
        #
        sels = self.__list_box.getcurselection()
        #
        if len(sels) == 0:
            pass
            #print 'No selection'
        else:
            #print 'Selection:', sels[0]
            # get the new stored command object
            i = self.__list_box.component('listbox').index(Tkinter.ACTIVE)
            try:
                mnemonic = self.__seq_cmds_list[i].getMnemonic()
            except exceptions.IndexError:
                mnemonic = self.__seq_cmds_list[Tkinter.END].getMnemonic()
            cmd_obj = self.__seq_cmds_list[i]
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



    def __getCmdString(self, cmd_obj):
        """
        For an command return it stringified.
        """
        mnemonic = cmd_obj.getMnemonic()
        opcode = cmd_obj.getOpCode()
        args = cmd_obj.getArgs()
        #
        cmd = "%s (0x%x)" % (mnemonic,int(opcode))
        for arg in args:
            cmd += ", %s" % arg[2].val
        return cmd


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
        self.__seq_cmds_list.append(new_cmd_obj)
        cmd = self.__getCmdString(new_cmd_obj)
        self.__list_box.insert(Tkinter.END, cmd)


    def __create_args(self, cmd_obj):
        """
        Command interface to command_args_frame to instance
        entries for commands.
        """
        for arg in cmd_obj.getArgs():
            arg_name, arg_desc, arg_type = arg
            e = self.__args_ui_panel_dict[arg_name]
            v = e.component('entry').get()
            #print arg_name, arg_type, arg_type.val, v
            new_obj = command_args.create_arg_type(arg_name, arg_type, v)
            #print "New obj: ", new_obj
            cmd_obj.setArg(arg_name, new_obj)
        #
        #self.__cmds_dict[self.__selected_cmd_mnemonic] = cmd_objself
        #self.__cmds.setCommandDict(self.__cmds_dict)
        self.__cmds.setCommandObj(self.__selected_cmd_mnemonic, cmd_obj)
        return cmd_obj


    def add(self):
        """
        Command to add an immediate command with args to sequence list.
        """
        #cmd_objself = self.__cmds_dict[self.__selected_cmd_mnemonic]
        cmd_objself = self.__cmds.getCommandObj(self.__selected_cmd_mnemonic)
        #print cmd_objself.getMnemonic(), self.__selected_cmd_mnemonic
        #
        cmd_objself = self.__create_args(cmd_objself)
        #print cmd_objself
        #
        self.appendHistoryItem(cmd_objself)


    def cut(self):
        """
        Remove selected sequencer command record and save for insert
        someplace else in the sequence list.
        """
        # Save command
        self.__clipboard = self.__getSelectedObj()
        i = self.__list_box.component('listbox').index(Tkinter.ACTIVE)
        # Remove command from both listbox and list of command objects
        self.__list_box.component('listbox').delete(Tkinter.ANCHOR)
        del self.__seq_cmds_list[i]


    def copy(self):
        """
        Copy selected sequencer command record for insert someplace else
        in the sequence list.
        """
        # Save command
        self.__clipboard = self.__getSelectedObj()


    def paste(self):
        """
        Insert sequencer command record in clip board either before
        or after selected record.  A dialog pops up to select before/after.
        """
        # Insert object into list of objects
        i = self.__list_box.component('listbox').index(Tkinter.ACTIVE)
        i += 1
        self.__seq_cmds_list.insert(i, self.__clipboard)
        # Insert object into vissable list
        cmd_string = self.__getCmdString(self.__clipboard)
        self.__list_box.component('listbox').insert(i, cmd_string)


    def clearHistory(self):
        """
        Clear the command history list here...
        """
        del self.__seq_cmds_list
        self.__seq_cmds_list = []
        self.__list_box.clear()


    def searchHistory(self, event=None):
        """
        Search the current history list here...
        """
        mnemonic_target = self.__search_entry.get()
        mnemonic_list = [obj_cmd.getMnemonic() for obj_cmd in self.__seq_cmds_list]
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
            #print "Cannot find search mnemonic: %s" % mnemonic_target


    def save(self, filename, typename):
        """
        Save sequence as filename of type typename.
        @param filename: path and name of file.
        @param typename: supported type of file, "ascii" or "binary" currently.
        """
        if typename == "ascii":
            s = SeqSave(SeqAsciiWriter())
            s.seqSave(filename, self.__seq_cmds_list)
        elif typename == "binary":
            s = SeqSave(SeqBinaryWriter())
            s.seqSave(filename, self.__seq_cmds_list)
        else:
            exceptions.TypeError


class SeqSave(object):
    """
    Strategy style class allowing connection of
    various writters for generation of different
    types of sequencer files.
    """
    def __init__(self, writer):
        """
        Constructor.
        @param writer: A writer object for writing specific seq. file formats.
        """
        self.__writer = writer


    def seqSave(self, filename, seq_cmds_list):
        """
        Save file to whatever format.
        @param filename: A string file name to save files in.
        @param seq_cmds_list:  A list of command objects to be written into file.
        """
        # Open file
        self.__writer.open(filename)
        # Write out file
        self.__writer.write(seq_cmds_list)
        # Close file
        self.__writer.close()


class SeqBinaryWriter(object):
    """
    Write out the Binary (ASTERIA) form of sequencer file.
    """
    def __init__(self, timebase=0xffff):
        """
        Constructor
        """
        self.__fd = None
        self.__timebase = timebase

    def open(self, filename):
        """
        Open the ASCII file
        """
        self.__fd = open(filename, "wb")

    def __binaryCmdRecord(self, cmd_obj):
        """
        Return the binary command record the sequencer is expecting.
        @todo: Currently the command descriptor is always zero for immediate commands.
        @todo: Execution time tags needed in command objects and seq_panel.
        """

        def __time_tag(cmd_obj):
          '''
          TODO: support a timebase in the cmd obj? This is mission specific, so it is tough to handle. For now
          I am hardcoding this to 2 which is TB_NONE
          '''
          #return TimeType(timeBase=2, seconds=cmd_obj.getSeconds(), useconds=cmd_obj.getUseconds()).serialize()
          # TKC - new command time format
          return U32Type( cmd_obj.getSeconds() ).serialize() + U32Type( cmd_obj.getUseconds() ).serialize()

        def __descriptor(cmd_obj):
          # subtract 1 from the value because enum34 enums start at 1, and this can't be changed
          return U8Type(cmd_obj.getDescriptor().value-1).serialize()

        def __command(cmd_obj):
          command = U32Type( 0 ).serialize() # serialize combuffer type enum: FW_PACKET_COMMAND
          command += U32Type( cmd_obj.getOpCode() ).serialize() # serialize opcode
          # Command arguments
          for arg in cmd_obj.getArgs():
              command += arg[2].serialize()
          return command

        def __length(command):
          return U32Type( len(command) ).serialize()

        def __print(byteBuffer):
            print "Byte buffer size: %d" % len(byteBuffer)
            for entry in range(0,len(byteBuffer)):
                print "Byte %d: 0x%02X (%c)"%(entry,struct.unpack("B",byteBuffer[entry])[0],struct.unpack("B",byteBuffer[entry])[0])

        # This is no longer in the sequence file format.
        #def __checksum(data):
        #  csum = 0
        #  for entry in range(0,len(data)):
        #    byte = struct.unpack("B",data[entry])[0]
        #    csum += byte
        #  return U64Type(long(csum)).serialize()

        # Form header:
        descriptor = __descriptor(cmd_obj)
        time = __time_tag(cmd_obj)
        header = descriptor + time

        # Command opcode:
        command = __command(cmd_obj)

        # Command length:
        length = __length(command)

        # Checksum:
        # This is no longer in the sequence file format.
        #checksum = __checksum(header + length + command)

        # Debug printing (comment out when not debugging):
        # print "descriptor:"
        # __print(descriptor)
        # print "time:"
        # __print(time)
        # print "length:"
        # __print(length)
        # print "command:"
        # __print(command)
        # print "total record:"
        # __print(header + checksum + length + command)

        # Construct the record:
        return header + length + command

    def write(self, seq_cmds_list):
        """
        Write out each record as it appears in the listbox widget.
        """
        num_records = len(seq_cmds_list)
        sequence = ""
        for cmd in seq_cmds_list:
            sequence += self.__binaryCmdRecord(cmd)
        size = len(sequence)
        print "Sequence is %d bytes with timebase %s" % (size, self.__timebase)

        header = ""
        header += U32Type( size + 4 ).serialize() # Write out size of the sequence file in bytes here
        header += U32Type( num_records ).serialize() # Write number of records
        header += U16Type( self.__timebase ).serialize() # Write time base
        header += U8Type( 0xFF ).serialize() # write time context
        sequence = header + sequence # Write the list of command records here
        # compute CRC. Ported from Utils/Hassh/libcrc/libcrc.h (update_crc_32)
        crc = self.computeCrc(sequence)

        print "CRC: %d (0x%04X)"%(crc,crc)
        try:
            sequence += U32Type( crc ).serialize()
        except TypeMismatchException as typeErr:
            print "Exception: %s" % typeErr.getMsg()
            raise

        # Write the list of command records here
        self.__fd.write( sequence )

    def close(self):
        """
        Close the Binary file
        """
        self.__fd.close()

    def computeCrc(self, buff):
        # See http://stackoverflow.com/questions/30092226/how-to-calculate-crc32-with-python-to-match-online-results
        # RE: signed to unsigned CRC
        return zlib.crc32(buff)% (1<<32)

class SeqAsciiWriter(object):
    """
    Write out the ASCII record form of sequencer file.
    """
    def __init__(self):
        """
        Constructor
        """
        self.__fd = None


    def open(self, filename):
        """
        Open the ASCII file
        """
        self.__fd = open(filename, "w")


    def __getCmdString(self, cmd_obj):
        """
        For an command return it stringified.
        """
        mnemonic = cmd_obj.getMnemonic()
        opcode = cmd_obj.getOpCode()
        args = cmd_obj.getArgs()
        #
        cmd = "%s (0x%x)" % (mnemonic,int(opcode))
        for arg in args:
            cmd += ", %s" % arg[2].val
        return cmd


    def write(self, seq_cmds_list):
        """
        Write out each record as it appears in the listbox widget.
        """
        for cmd in seq_cmds_list:
            self.__fd.write(self.__getCmdString(cmd) + "\n")


    def close(self):
        """
        Close the ASCII file
        """
        self.__fd.close()


def main_window_start():
    """
    """
    root = Tkinter.Tk()
    #root.option_readfile('optionDB')
    root.title('F Prime Sequence Panel')
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
