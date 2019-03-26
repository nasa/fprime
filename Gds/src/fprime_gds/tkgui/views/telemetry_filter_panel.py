'''
Created on Apr. 29, 2015

@author: reder
'''

import time
import Tkinter
import tkFileDialog
import Pmw
import os

from fprime_gds.tkgui.controllers import channel_loader

def telemetryFilterDialog(parent, inactive_channels, selected_channels=[]):
    """
    Wrapper call for TelemetryFilterPanel class to operate as a dialog.
    """
    t = TelemetryFilterPanel(parent, inactive_channels, selected_channels)
    return t.get()


class TelemetryFilterPanel(object):
    '''
    This panel is a dialog that returns the list of
    viewable telemetry channels.
    '''

    def __init__(self, parent, inactive_channels, selected_channels=[]):
        '''
        Constructor
        '''
        self.__inactive_channels = inactive_channels
        self.__inactive_channels.sort()
        selected_channels.sort()

        ch_loader = channel_loader.ChannelLoader.getInstance()
        self.__channel_names_list = ch_loader.getNameDict().values()
        self.__channel_names_list.sort()

        # Update inactive_channels if channels were selected earlier
        # Use the fact that empty sequences (lists, etc...) are False (PEP 8)
        if not selected_channels:
            for c in selected_channels:
                try:
                    self.__inactive_channels.remove(c)
                except ValueError:
                    pass

        self.__inactive_channels = inactive_channels

        top = Tkinter.Toplevel(parent)
        top.title("F Prime Channel Telemetry Filter Selection")
        top.focus()
        # Constrain size on window so geometry works.
        top.maxsize(1000,400)
        top.minsize(600,400)
        top.lift()
        self.__top = top

        # Container Frame
        #
        f = Tkinter.Frame(top)
        f.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)

        #
        f2 = Tkinter.Frame(top)
        f2.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)
        f3 = Tkinter.Frame(top)
        f3.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)
        #
        # Build list boxes for channels
        #
        ff1 = Tkinter.LabelFrame(f2, text="Channels Avaliable:", relief=Tkinter.GROOVE, borderwidth=2, padx=5, pady=5)
        ff1.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)
        #
        # Scrolled log message list box here...
        #
        self.__list_box_channels = Pmw.ScrolledListBox(ff1, listbox_selectmode=Tkinter.EXTENDED,
                                  listbox_height=15,
                                  selectioncommand=self.__selectionCommandToSelected,
                                  dblclickcommand=self.__selectionCommandToSelected,
                                  items=tuple(self.__inactive_channels))
        self.__list_box_channels.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, fill=Tkinter.BOTH, expand=1, padx=5, pady=5)
        #
        bbox = Pmw.ButtonBox(f2, orient=Tkinter.VERTICAL)
        bbox.pack(side=Tkinter.LEFT)
        bbox.add(">", command=self.__selectChannels)
        bbox.add("<", command=self.__deselectChannels)
        bbox.add('>>', command=self.__add_all)
        bbox.add('<<', command=self.__clear_all)
        #
        ff2 = Tkinter.LabelFrame(f2, text="Channels Selected:", relief=Tkinter.GROOVE, borderwidth=2, padx=5, pady=5)
        ff2.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)
        #
        # Scrolled log message list box here...
        #
        self.__list_box_selected = Pmw.ScrolledListBox(ff2, listbox_selectmode=Tkinter.EXTENDED,
                                  listbox_height=15,
                                  selectioncommand=self.__selectionCommandToAvaliable,
                                  dblclickcommand=self.__selectionCommandToAvaliable,
                                  items=selected_channels)
        self.__list_box_selected.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, fill=Tkinter.BOTH, expand=1, padx=5, pady=5)
        #
        # Buttons to implement all the dialog commands...
        #
        buttonBox = Pmw.ButtonBox(f3, labelpos='nw', label_text='Channel Selection Commands:')
        buttonBox.pack(side=Tkinter.BOTTOM, fill=Tkinter.BOTH, expand=1, padx=10, pady=10)

        buttonBox.add('Save', command=self.__save)
        buttonBox.add('Load', command=self.__load)
        buttonBox.add('Cancel', command=self.__cancel)
        buttonBox.add('Ok', command=self.__ok)
        #
        self.__all_channels_selected = inactive_channels
        self.__channels_selected = selected_channels
        self.__active_filter_selected = None
        # Mouse selection lists for each ScrolledListbox
        self.__select_to_selected = []
        self.__select_to_avaliable = []
        #
        self.__top.wait_window(self.__top)


    def __selectionCommandToSelected(self):
        self.__select_to_selected  = self.__list_box_channels.getcurselection()
        if len(self.__select_to_selected) == 0:
            self.__select_to_selected = []

    def __selectionCommandToAvaliable(self):
        self.__select_to_avaliable  = self.__list_box_selected.getcurselection()
        if len(self.__select_to_avaliable) == 0:
            self.__select_to_avaliable = []

    def refresh(self, event=None):
        pass


    # Button callbacks here...
    def __selectChannels(self):
        """
       Selects channels for filter.
        """
        self.__active_filter_selected = 'User Selected Channels'
        if len(self.__select_to_selected) > 0:
            # Send items to Channels Selected
            [self.__list_box_selected.insert(Tkinter.END, item) for item in self.__select_to_selected]
            # Remove items from Channels Avaliable
            items = list(self.__list_box_channels.get(0, Tkinter.END))
            self.__list_box_channels.delete(0,Tkinter.END)
            [items.remove(i) for i in self.__select_to_selected]
            [self.__list_box_channels.insert(Tkinter.END, i) for i in items]
        # Get selected
        self.__channels_selected = list(self.__list_box_selected.get(0, Tkinter.END))
        self.__inactive_channels    = list(self.__list_box_channels.get(0, Tkinter.END))
        self.__select_to_avaliable = ()
        self.__select_to_selected = ()


    def __deselectChannels(self):
        """
        Deselects channels for filter.
        """
        self.__active_filter_selected = 'User Selected Channels'
        if len(self.__select_to_avaliable) > 0:
            # Send items to Channels Avaliable
            [self.__list_box_channels.insert(Tkinter.END, item) for item in self.__select_to_avaliable]
            items = list(self.__list_box_channels.get(0, Tkinter.END))
            items.sort()
            self.__list_box_channels.delete(0,Tkinter.END)
            [self.__list_box_channels.insert(Tkinter.END, i) for i in items]

            # Remove items from Channels Selected
            items = list(self.__list_box_selected.get(0, Tkinter.END))
            self.__list_box_selected.delete(0,Tkinter.END)
            [items.remove(i) for i in self.__select_to_avaliable]
            [self.__list_box_selected.insert(Tkinter.END,i) for i in items]
        # Get selected
        self.__channels_selected = list(self.__list_box_selected.get(0, Tkinter.END))
        self.__inactive_channels = list(self.__list_box_channels.get(0, Tkinter.END))
        self.__select_to_avaliable = ()
        self.__select_to_selected = ()


    def __add_all(self):
        """
        Resets the filter to all channels selected state.
        """
        self.__active_filter_selected = 'Reset'
        self.__channels_selected = self.__channel_names_list
        self.__inactive_channels = []
        self.__list_box_channels.delete(0,Tkinter.END)
        [self.__list_box_selected.insert(Tkinter.END,i) for i in  self.__channels_selected]

    def __clear_all(self):
      """
      Resets the filter to clear all channels.
      """
      self.__active_filter_selected = 'Clear'
      self.__channels_selected = []
      self.__inactive_channels = self.__channel_names_list
      self.__list_box_selected.delete(0,Tkinter.END)
      [self.__list_box_channels.insert(Tkinter.END,i) for i in  self.__inactive_channels]

    def __save(self):
        """
        Saves a filter configuration in an ascii file.
        """
        p = "."
        #
        # Various file save as dialogs and save method calls.
        f = tkFileDialog.asksaveasfilename(defaultextension =".txt", \
                                           filetypes =[(".txt","*")], \
                                           initialdir = p, \
                                           initialfile = "Channels", \
                                           title = "Save Selected Channels")
        if f:
            self.__active_filter_selected = f
            f = open(f,"w+")
            f.writelines(map(lambda x: x + '\n', self.__channels_selected))
            f.close()


    def __load(self):
        """
        Loads a filter configuration from an ascii file.
        """
        p = "."
        #
        # Various file save as dialogs and save method calls.
        f = tkFileDialog.askopenfilename(defaultextension =".txt", \
                                     filetypes =[('text files',".txt")], \
                                     initialdir = p, \
                                     title = "Open Selected Channels (*.txt)")
        if f:
            self.__active_filter_selected = f
            f = open(f,"r")
            loaded_channels = f.readlines()
            f.close()
            loaded_channels = map(lambda x: x[:-1],loaded_channels)

            # Move all selected into available
            [self.__list_box_channels.insert(Tkinter.END, i) for i in self.__channels_selected]

            # Insert into selected list
            self.__list_box_selected.delete(0,Tkinter.END)
            [self.__list_box_selected.insert(Tkinter.END,i) for i in loaded_channels]
            self.__channels_selected = loaded_channels

            # Remove from avaliable list
            items = list(self.__list_box_channels.get(0, Tkinter.END))
            self.__list_box_channels.delete(0,Tkinter.END)
            [items.remove(i) for i in loaded_channels if i in items]
            [self.__list_box_channels.insert(Tkinter.END, i) for i in items]
            self.__inactive_channels = items


    def __cancel(self):
        """
        Cancel filter operation and return all channels.
        """
        self.__active_filter_selected = None
        self.__channels_selected = None
        self.__top.destroy()


    def __ok(self):
        """
        Ok returns the currently selected channels for filtering.
        """
        self.__top.destroy()


    def get(self):
        """
        Returns the current list of channels selected.
        """
        return (self.__active_filter_selected, self.__channels_selected, self.__list_box_channels, self.__inactive_channels)


if __name__ == "__main__":
    root = Tkinter.Tk()
    #root.option_readfile('optionDB')
    root.title('Test Root Window')
    Pmw.initialise()
    t = telemetryFilterDialog(root, ["RateGroup1","RateGroup2", "RateGroup3", "CommandsDispatched"])
    print t
    root.destroy()
    root.mainloop()
