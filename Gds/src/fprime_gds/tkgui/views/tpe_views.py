import Tkinter
import tkMessageBox
import Pmw
import ttk

from fprime_gds.tkgui.tpe_models import SessionModel

# This class defines color styles
# It should be relatively easy for users to make their own if they are so inclined
# The styles must be set for both the PacketView class and the ItemList class
# Currently that means changing the style name on lines:
#    Packetview:  Line 86
#    ItemList:    Line 636
class StyleColors():

    def default_style(self):
        self.topFrameColor = 'lightblue'
        self.frameColor = 'white'
        self.highlightColor = 'powderblue'
        self.entryHighlight = 'white'
        self.entryColor = 'white'
        self.arrowButton = 'white'
        self.arrowBg = 'white'
        self.bitsLeftColor = 'white'
        self.bitsTextColor = 'black'
        self.treeviewText='black'
        self.treeviewBackground='white'

    def desert_style(self):
        self.topFrameColor = 'NavajoWhite3'
        self.frameColor = 'linen'
        self.highlightColor = 'steelblue'
        self.entryHighlight = 'old lace'
        self.entryColor = 'white smoke'
        self.arrowButton = 'white smoke'
        self.arrowBg = 'linen'
        self.bitsLeftColor = 'NavajoWhite3'
        self.bitsTextColor = '#3f1d00'
        self.treeviewText = 'grey7'
        self.treeviewBackground = 'NavajoWhite3'

    def teal_style(self):
        self.topFrameColor = '#00695c'
        self.frameColor = '#b2dfdb'
        self.highlightColor = '#e0f2f1'
        self.entryHighlight = '#b2dfdb'
        self.entryColor = '#eaf2f1'
        self.arrowButton = '#eaf2f1'
        self.arrowBg = '#b2dfdb'
        self.bitsLeftColor = '#00695c'
        self.bitsTextColor = 'yellow'
        self.treeviewText = 'grey7'
        self.treeviewBackground = '#b2dfdb'

    def pastel_style(self):
        self.topFrameColor = '#c7fbf5'
        self.frameColor = '#fff8e1'
        self.highlightColor = 'steelblue'
        self.entryHighlight = '#fff8e1'
        self.entryColor = '#e3f2fd'
        self.arrowButton = 'white smoke'
        self.arrowBg = '#fff8e1'
        self.bitsLeftColor = '#ffcdd2'
        self.bitsTextColor = '#3f1d00'
        self.treeviewText = 'grey7'
        self.treeviewBackground = '#c7fbf5'

    def ocean_style(self):
        self.topFrameColor = '#40c4ff'
        self.frameColor = '#e0ffff'
        self.highlightColor = 'steelblue'
        self.entryHighlight = '#e0ffff'
        self.entryColor = 'floral white'
        self.arrowButton = 'floral white'
        self.arrowBg = '#e0ffff'
        self.bitsLeftColor = '#ffcdd2'
        self.bitsTextColor = '#3f1d00'
        self.treeviewText = 'grey7'
        self.treeviewBackground = '#40c4ff'


class PacketView(Tkinter.Frame):

    def __init__(self, root):

        style = StyleColors()
        style.ocean_style()    # Select a style for the StyleColors class  Note: should proabably match the ItemView selection below

        top_frame_color = style.topFrameColor
        frame_color = style.frameColor
        highlight_color = style.highlightColor
        entry_highlight = style.entryHighlight
        entry_color = style.entryColor
        arrow_button = style.arrowButton
        arrow_bg = style.arrowBg
        bits_text_color = style.bitsTextColor
        bits_left_color = style.bitsLeftColor

        Tkinter.Frame.__init__(self, master=root, background=top_frame_color)

        self.model         = None
        self.controller    = None

        top_frame = Tkinter.Frame(self, background=frame_color)

        top_frame.configure()
        top_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=(0,15))
        # Menu Bar
        menuBar = Pmw.MenuBar(top_frame, hull_relief=Tkinter.RIDGE, hull_borderwidth=0, padx=(10,0), hull_background=frame_color)
        menuBar.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, expand = 1, fill=Tkinter.X)
        menuBar.addmenu('File', None,  background=frame_color)
        menuBar.addmenuitem('File', 'command', font=('StingerLight', 14), label="Load...", command=self.load_session)
        menuBar.addmenuitem('File', 'command', font=('StingerLight', 14), label="Validate..", command=self.validate_session)
        menuBar.addmenuitem('File', 'command', font=('StingerLight', 14), label='Save As..', command=self.save_session)

        self.__used_label = Tkinter.Label(top_frame)
        self.__used_label.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE)
        self.__used_label.configure(background=bits_left_color, foreground=bits_text_color)
        self.__avail_label  = Tkinter.Label(top_frame)
        self.__avail_label.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE)
        self.__avail_label.configure(background=bits_left_color, foreground=bits_text_color)

        ## Session View
        session_select_frame = Tkinter.LabelFrame(self, text="Session", background=frame_color)
        session_select_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=(0,15), padx=(10,10))

        self.__new_packet_button = Tkinter.Button(session_select_frame, text="New Packet", highlightbackground=frame_color, command=self.__new_packet)
        self.__new_packet_button.pack(side=Tkinter.LEFT, padx=(20,0))

        self.__select_packet_dd = Pmw.ComboBox(session_select_frame, labelpos='w', label_text='Packet:',
                                               label_background=frame_color, entry_background=frame_color,
                                               hull_background=frame_color, label_highlightbackground=highlight_color,
                                               entry_highlightbackground=entry_highlight,
                                               selectioncommand=self.__packet_selected)
        self.__select_packet_dd.component('entry').config(background=entry_color)
        self.__select_packet_dd.component('arrowbutton').configure(background=arrow_button, highlightbackground=arrow_bg)
        self.__select_packet_dd.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, padx=(20,20))
        self.__delete_packet_button = Tkinter.Button(session_select_frame, text="Delete Packet", highlightbackground=frame_color, command=self.__delete_packet)
        self.__delete_packet_button.pack(side=Tkinter.LEFT, anchor=Tkinter.NW)

        self.__duration_label = Tkinter.Label(session_select_frame, text="", background=frame_color)  #note: text is set in the config_session method
        self.__duration_label.pack(side=Tkinter.LEFT, anchor=Tkinter.SE, padx=(80,0), pady=(0,4))

        self.__tlm_rate_label = Tkinter.Label(session_select_frame, text="", background=frame_color)  #note: text is set in the config_session method
        self.__tlm_rate_label.pack(side=Tkinter.LEFT, anchor=Tkinter.SE, padx=(10, 0), pady=(0, 4))

        ## Packet Edit Frame
        packet_edit_frame = Tkinter.LabelFrame(self, text='Packet Attributes', background=frame_color)

        packet_edit_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=(5,20), padx=(10,10))

        self.__packet_id_entry = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='Packet ID:',
                                                label_background=frame_color, entry_background=entry_color,
                                                hull_background=frame_color, label_highlightbackground=highlight_color,
                                                entry_highlightbackground=entry_highlight,
                                                validate={'validator': 'numeric','min':0, 'max':65535,
                                                          'minstrict':0, 'maxstrict':1})
        self.__packet_id_entry.grid(row=0, column=0, sticky=Tkinter.E)


        self.__select_freq = Pmw.ComboBox(packet_edit_frame, labelpos='w', label_text=" Frequency:", entry_width=16,
                                          label_background=frame_color, entry_background=entry_color,
                                          hull_background=frame_color, label_highlightbackground=highlight_color,
                                          entry_highlightbackground=entry_highlight,
                                          entryfield_entry_state='readonly', selectioncommand=self.__freq_selected)
        self.__select_freq.component('entry').config(readonlybackground=entry_color)
        self.__select_freq.component('arrowbutton').configure(background=arrow_button, highlightbackground=arrow_bg)
        self.__select_freq.grid(row=0, column=1, sticky=Tkinter.E)

        self.__select_offset = Pmw.ComboBox(packet_edit_frame, labelpos='w', label_text='Offset:', entry_width=16,
                                            label_background=frame_color, entry_background=entry_color,
                                            hull_background=frame_color, label_highlightbackground=highlight_color,
                                            entry_highlightbackground=entry_highlight,
                                            entryfield_entry_state='readonly', selectioncommand=self.__offset_selected)
        self.__select_offset.component('entry').config(readonlybackground=entry_color)
        self.__select_offset.component('arrowbutton').configure(background=arrow_button, highlightbackground=arrow_bg)
        self.__select_offset.grid(row=0, column=2, sticky=Tkinter.E)


        self.__packet_name_entry = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='Packet Name:',
                                                  label_background=frame_color, entry_background=entry_color,
                                                  hull_background=frame_color, label_highlightbackground=highlight_color,
                                                  entry_highlightbackground=entry_highlight)
        self.__packet_name_entry.grid(row=1, column=0, sticky=Tkinter.E)

        self.__ff3_channel_entry = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='FF3 Channel:',
                                                  label_background=frame_color, entry_background=entry_color,
                                                  hull_background=frame_color,
                                                  label_highlightbackground=highlight_color,
                                                  entry_highlightbackground=entry_highlight,
                                                  validate={'validator': 'numeric','min':0, 'max':65535,
                                                            'minstrict':0, 'maxstrict':1})
        self.__ff3_channel_entry.grid(row=1, column=1, sticky=Tkinter.E)

        self.__packet_save_button = Tkinter.Button(packet_edit_frame, text="Save Packet",

                                                   highlightbackground=frame_color, command=self.__save_packet)
        self.__packet_save_button.grid(row=2, column=0, sticky=Tkinter.W, padx=(121,0))

        ## Item Edit Frame
        item_edit_frame = Tkinter.LabelFrame(self, text='Items', background=frame_color)

        item_edit_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=(0,15), padx=(10,10))

        self.__select_comp_dd = Pmw.ComboBox(item_edit_frame, labelpos='w', label_text="Component:", entry_width=22,
                                             label_background=frame_color, entry_background=frame_color,
                                             hull_background=frame_color, label_highlightbackground=highlight_color,
                                             entry_highlightbackground=entry_highlight,
                                             entryfield_entry_state='readonly',
                                             selectioncommand=self.__comp_selected)
        self.__select_comp_dd.component('entry').config(readonlybackground=entry_color)
        self.__select_comp_dd.component('arrowbutton').configure(background=arrow_button, highlightbackground=arrow_bg)
        self.__select_comp_dd.grid(row=0, column=0, columnspan = 1, sticky=Tkinter.E, padx=(16,0))

        self.__select_channel_dd = Pmw.ComboBox(item_edit_frame, labelpos='w', label_text='Channel:',
                                                label_background=frame_color, entry_background=frame_color,
                                                hull_background=frame_color, label_highlightbackground=highlight_color,
                                                entry_highlightbackground=entry_highlight,
                                                entryfield_entry_state='readonly',selectioncommand=self.__channel_selected)
        self.__select_channel_dd.component('entry').config(readonlybackground=entry_color)
        self.__select_channel_dd.component('arrowbutton').configure(background=arrow_button, highlightbackground=arrow_bg)

        self.__select_channel_dd.grid(row=0, column=1, columnspan = 3, sticky=Tkinter.EW, padx=(9,5))

        self.__size_entry = Pmw.EntryField(item_edit_frame, labelpos='w', entry_width= 30, label_text='Size (bits):',
                                           label_background = frame_color, entry_background = entry_color,
                                           hull_background = frame_color, label_highlightbackground = highlight_color,
                                           entry_highlightbackground = entry_highlight)
        self.__size_entry.grid(row=1, column=1, columnspan = 1, sticky=Tkinter.W, padx=(0,0))

        self.__add_reserve_button = Tkinter.Button(item_edit_frame, text="Add Reserve", highlightbackground=frame_color,
                                                   command=self.__add_reserve)
        self.__add_reserve_button.grid(row=1, column=2, columnspan = 1,  sticky=Tkinter.E, padx=(20,0))
        self.__reserve_entry = Pmw.EntryField(item_edit_frame, labelpos='w', entry_width= 5, label_text='Bits:',
                                              label_background = frame_color, entry_background = entry_color,
                                              hull_background = frame_color, label_highlightbackground = highlight_color,
                                              entry_highlightbackground = entry_highlight)
        self.__reserve_entry.grid(row=1, column=3, columnspan=1, sticky=Tkinter.W, padx=(0,38))

        self.__desc_entry = Pmw.EntryField(item_edit_frame, labelpos='w', entry_width=30, label_text='Desc:',
                                           label_background = frame_color, entry_background = entry_color,
                                           hull_background = frame_color, label_highlightbackground = highlight_color,
                                           entry_highlightbackground = entry_highlight)
        self.__desc_entry.grid(row=2, column=1, columnspan=1,  sticky=Tkinter.W, padx=(30,0))

        self.__byte_align_button = Tkinter.Button(item_edit_frame, text="Byte Align    ", highlightbackground=frame_color, command=self.__byte_align)
        self.__byte_align_button.grid(row=2, column=2, columnspan=1, sticky=Tkinter.EW, padx=(20,0))

        self.__add_item_button = Tkinter.Button(item_edit_frame, text='Add    ', highlightbackground=frame_color, command=self.__add_item)
        self.__add_item_button.grid(row=3, column=1, columnspan=1, sticky=Tkinter.W, padx=(72,0))

        self.__word_align_button = Tkinter.Button(item_edit_frame, text="Word Align  ", highlightbackground=frame_color, command=self.__word_align)
        self.__word_align_button.grid(row=3, column=2, columnspan=1, sticky=Tkinter.EW, padx=(20,0))

        self.__delete_item_button = Tkinter.Button(item_edit_frame, text="Delete", highlightbackground=frame_color, command=self.__delete_row)
        self.__delete_item_button.grid(row=4, column=1, columnspan=1, sticky=Tkinter.W, padx=(72,0))

        # Configure editor frame grid
        for i in range(5):
            packet_edit_frame.grid_columnconfigure(i, weight=1)
            packet_edit_frame.grid_rowconfigure(i, weight=1)
            item_edit_frame.grid_columnconfigure(i, weight=1)
            item_edit_frame.grid_rowconfigure(i, weight=1)

        ## List Frame
        self.__item_list = ItemList(self)
        self.__item_list.pack(side=Tkinter.TOP, anchor=Tkinter.N, expand=1, fill=Tkinter.BOTH)


    def validate_session(self):
        print("Validating Session.")
        self.controller.validate_session()
        self.__item_list.refresh(jump=False)

    def save_session(self):
        self.controller.save_session()

    def load_session(self):
        if self.controller.load_session():
            self.enable_comboBox_arrow()
            self.change_items_state('normal')
            self.refresh_item_list()

    def load_resources(self, model=None, controller=None, packet_list=None):
        """
        - Set model and controller.
        - Load dropdowns.
        - Connect labels and entries to model variables
        """
        if controller:
            self.controller = controller
        if packet_list:
            self.__select_packet_dd.setlist(sorted(packet_list))
            self.__select_packet_dd.selectitem(0,setentry=1)
        if model:
            self.__load_model(model)
        self.controller.update_idletasks()

    def __load_model(self, model):
        
        self.model = model

        try:
            self.controller.load_model(model)
        except Exception, e:
            pass

        # Determine available space from "Bit Left: xxx" string
        # This will be used to initailzie the __reserve_entry Entry field below
        # In the GUI the lable is 'Bits:'
        avail_space = str(model.avail_space_var.get())
        reserve = avail_space[avail_space.find(':')+2:]

        # Connect model variables
        self.__packet_id_entry.component('entry').config(textvariable=model.pid_entry_var)
        self.__select_offset.component('entry').config(textvariable=model.offset_var)
        self.__select_freq.component('entry').config(textvariable=model.freq_var)
        self.__ff3_channel_entry.component('entry').config(textvariable=model.ff3_channel_var)
        self.__packet_name_entry.component('entry').config(textvariable=model.packet_name_var)
        self.__reserve_entry.component('entry').config(textvariable=model.reserve_var)
        self.__size_entry.component('entry').config(textvariable=model.size_var)
        self.__desc_entry.component('entry').config(textvariable=model.desc_var)
        self.__used_label.config(textvariable=model.used_space_var)
        self.__avail_label.config(textvariable=model.avail_space_var)
        self.refresh_item_list()
        # Initalize _reserve_entry field so the Add Reserve button makes sense when we first come up
        self.__reserve_entry.setvalue(reserve)

    def config_session(self, session_model):
        """
        Packet select is session variable
        """
        self.__select_packet_dd.component('entry').config(textvariable=session_model.pid_select_var)
        self.__select_channel_dd.component('entry').config(textvariable=session_model.selected_channel_var)

        # disable 'Item' frame buttons, and Hide the 'Packet Attributes' save button
        self.change_items_state('disabled')
        self.hide_save_packet_button(True)

        # Populate dropdowns
        channels = session_model.get_channel_names()
        self.__select_channel_dd.setlist(sorted(channels))
        # Disable dropdowns
        self.disable_comboBox_arrow()

        components = session_model.get_comp_names()
        self.__select_comp_dd.setlist(sorted(set(components)))

        frequencies = session_model.get_freq_list()
        self.__select_freq.setlist(frequencies)

        # Set minimum size
        self.master.update()
        self.master.minsize(self.master.winfo_width(), self.master.winfo_height())
        self.sm = session_model  # used by reinitialize_session()

        duration_text = 'Duration: {} sec.'.format(session_model.duration)
        self.__duration_label.config(text=duration_text)

        tlm_rate_text = 'Tlm Rate: {} Hz.'.format(session_model.tlm_rate)
        self.__tlm_rate_label.config(text=tlm_rate_text)


    def reinitialize_session(self):
        '''
        This is needed when a user manually deletes the last packet in a session.
        At that point everything needs to set up as if it were a brand new session.
        '''
        # Clear all the entry boxes and combo boxes
        self.__select_packet_dd.clear()
        self.__packet_id_entry.clear()
        self.__select_freq.clear()
        self.__select_offset.clear()
        self.__packet_name_entry.clear()
        self.__ff3_channel_entry.clear()
        self.__select_comp_dd.clear()
        self.__select_channel_dd.clear()
        self.__size_entry.clear()
        self.__reserve_entry.clear()
        self.__desc_entry.clear()
        self.config_session(self.sm)
        # clear all the entries in the table
        self.__item_list.clear_entries()
        # reset the used and available space display widgets
        self.controller.reset_space_data()
        # configure GUI to come up as if it had just started
        self.sm.update()

    def change_items_state(self, new_state):
        # Disable buttons until a packet id is selected
        self.__add_item_button.config(state=new_state)
        self.__delete_item_button.config(state=new_state)
        self.__add_reserve_button.config(state=new_state)
        self.__byte_align_button.config(state=new_state)
        self.__word_align_button.config(state=new_state)
        if new_state == 'normal':
            self.__select_comp_dd.component('arrowbutton').bind('<Button-1>', self.__select_comp_dd._postList)
            self.__select_channel_dd.component('arrowbutton').bind('<Button-1>', self.__select_channel_dd._postList)
        else:
            self.__select_comp_dd.component('arrowbutton').unbind('<Button-1>')
            self.__select_channel_dd.component('arrowbutton').unbind('<Button-1>')

    def disable_comboBox_arrow(self):
        '''
        This has to be disabled because PacketID entry is blocked if a comboBox pulldown is clicked before "New Packet"
        is clicked.  It made sense to disable all combo pulldowns until either a new packet is being made or a csv file
        is loaded.
        '''
        self.__select_freq.component('arrowbutton').unbind('<Button-1>')
        self.__select_offset.component('arrowbutton').unbind('<Button-1>')
        self.__select_packet_dd.component('arrowbutton').unbind('<Button-1>')
        self.__select_comp_dd.component('arrowbutton').unbind('<Button-1>')
        self.__select_channel_dd.component('arrowbutton').unbind('<Button-1>')

    def enable_comboBox_arrow(self):
        '''
        Enables the dropdown menus
        '''
        self.__select_freq.component('arrowbutton').bind('<Button-1>', self.__select_freq._postList)
        self.__select_packet_dd.component('arrowbutton').bind('<Button-1>', self.__select_packet_dd._postList)
        self.__select_comp_dd.component('arrowbutton').bind('<Button-1>', self.__select_comp_dd._postList)
        self.__select_channel_dd.component('arrowbutton').bind('<Button-1>', self.__select_channel_dd._postList)

    def check_entries(self):
        self.__packet_id_entry.checkentry()
        self.__packet_name_entry.checkentry()
        self.__ff3_channel_entry.checkentry()

    def __packet_selected(self, entry):
        self.controller.packet_selected(entry)
        self.check_entries()

    def __new_packet(self):
        # clear the offset choices, they are only meaningful after frequency has been selected
        self.__select_offset.clear()
        self.controller.new_packet()
        self.enable_comboBox_arrow()
        # enable item frame
        self.change_items_state('disable')
        self.hide_save_packet_button(False)

    def __delete_packet(self):
        self.controller.delete_packet()

    def empty_struct(self, struct):
        if struct: return False
        else: return True

    def check_attribute_fields(self):
        '''
        Check the attribute fields when new packets are created and saved in the interface
        '''
        missing_attributes = []
        attributes = {}
        attributes["Packet ID"]   = self.__packet_id_entry.getvalue()
        attributes['Frequency']   = self.__select_freq.getvalue()
        attributes['Offset']      = self.__select_offset.getvalue()
        attributes['Packet Name'] = self.__packet_name_entry.getvalue()
        attributes['FF3 Channel'] = self.__ff3_channel_entry.getvalue()

        for key in attributes:
            if self.empty_struct(attributes[key]):
                # make a list of tuples to be returned if some packet attributes have not been defined
                missing_attributes.append((key, attributes[key]))
        # Check that the Packet ID is unique to this session
        if not self.empty_struct(attributes['Packet ID']):  # make sure a value has been entered
            unique = self.controller.verify_unique_pkt_id(attributes['Packet ID'])
            if not unique:
                return -1
        return missing_attributes


    def __save_packet(self):
        missing = self.check_attribute_fields()
        if missing == -1:   # Duplicate packet ID
            return
        elif not missing:   # Nothing missing, all items entered
            update = self.__packet_save_button.cget("text")
            self.controller.save_packet(update)
            self.change_items_state('normal')
        else:
            print("Save Packet ERROR:")
            for i, value in enumerate(missing):
                print("Missing '{}' attribute, current value is: {}".format(value[0], value[1]))
                if 'Frequency' in value[0]:    # enable the Frequency for updating
                    self.__select_freq.component('arrowbutton').bind('<Button-1>', self.__select_freq._postList)
            tkMessageBox.showinfo("ALERT", "Cannot save packet until all the fields are filled in.")

    def __delete_row(self):
        self.controller.delete_item()
        self.clear_items()

    def __comp_selected(self, entry):
        """
        Filter channel dropdown via component
        """
        try:
            self.controller.comp_selected(entry)
        except AttributeError, e:
            raise e

    def __channel_selected(self, entry):
        """
        Update model of selected channel
        """
        try:
            self.controller.channel_selected(entry)    	
        except AttributeError, e:
            pass

    def __freq_selected(self, entry):
        """
        Simple select of the frequency from the pulldown
        """
        try:
            self.__select_freq.setvalue(entry)
            # frequency is set, we can know compute offset
            if entry == 'Aperiodic' or entry == 'Time Correlated':
                self.__select_offset.setlist(['N/A'])
                self.model.set_offset('N/A')
            else:
                offset_range = self.controller.get_offset_list()
                self.__select_offset.clear()
                self.__select_offset.setlist(offset_range)
                self.model.set_offset(0)   # if changing from non-periodic to fixed period, initialize with zero offset
            # enable the  offset pulldown
            self.__select_offset.component('arrowbutton').bind('<Button-1>', self.__select_offset._postList)

        except AttributeError, e:
            raise e

    def __offset_selected(self, entry):
        """
								Simple select of the offset from the pulldown
								"""
        try:
            self.__select_offset.setvalue(entry)
        except AttributeError, e:
            raise e

    def __byte_align(self):
        self.controller.add_align('Byte', SessionModel.byte_size)

    def __word_align(self):
        self.controller.add_align('Word', SessionModel.word_size)

    def clear_items(self):
        self.__select_channel_dd.setentry('')
        self.__size_entry.clear()
        self.__desc_entry.clear()

    def __add_item(self):
        """
        Get data from editor and pass to model
        """
        update = self.__add_item_button.cget('text')
        self.controller.add_item(update)
        self.clear_items()

    def set_add_channel_button_text(self, t):
        """
        Set the button text for item add.
        'Update' or 'Add' invoke different behaviors.
        """
        self.__add_item_button.config(text=t)

    def hide_save_packet_button(self, hide):
        if hide:
            self.__packet_save_button.grid_remove()
        else:
            self.__packet_save_button.grid()

    def set_packet_dd(self, idx, set=0):
        self.__select_packet_dd.selectitem(idx,setentry=set)

    def set_component_dd(self, idx):
        self.__select_comp_dd.selectitem(idx)
        comp_name = self.__select_comp_dd.get()
        self.__comp_selected(comp_name)

    def set_channel_dd(self, idx):
        self.__select_channel_dd.selectitem(idx)

    def set_packet_select_text(self, t):
        self.__select_packet_dd.component('entry').delete(0, Tkinter.END)
        self.__select_packet_dd.component('entry').insert(0, t)

    def reserve_button_state(self, mode):
        self.__add_reserve_button.config(state=mode)

    def __add_reserve(self):
        self.controller.add_reserve()

    def reserve_present(self):
        return(self.__item_list.reserve_present())

    def delete_reserve(self):
        self.__item_list.refresh(jump=True, last=True)
        self.__delete_row()
        self.refresh_item_list(jump=False)  # this should reset the list

    def reset_reserve(self):
        self.__add_reserve()

    def set_reserve_bit_display(self, bits):
        bit = int(bits)
        self.__reserve_entry.setvalue(bits)

    def refresh_item_list(self, jump=False):
        self.__item_list.refresh(jump)
        new_state = self.__item_list.check_reserve()
        self.reserve_button_state(new_state)

    def set_channel_select_text(self, text):
        self.__select_channel_dd.component('entry').delete(0, Tkinter.END)
        self.__select_channel_dd.component('entry').insert(0, text)
        self.__channel_selected(text)
        self.__select_channel_dd.setentry(text)    # this allows the update to the readonly comboBox

    def set_channel_dd_items(self, channels):
        self.__select_channel_dd.setlist(channels)
        self.set_channel_dd(0)

        ch_name = self.__select_channel_dd.get()
        self.__channel_selected(ch_name)

    def update_item(self, ch_name):
        self.__item_list.update_item(ch_name)
        self.clear_items()

class ItemList(ttk.Treeview):
    """
    List of packets
    """

    def __init__(self, master):

        myStyle = StyleColors()
        myStyle.ocean_style()    # Select a style for the StyleColors class  Note: should proabably mat the PacketView selection above

        ttk.Treeview.__init__(self, master)

        self.__master = master
        # Starting row
        self.__row = 0

        self["columns"] = ('mnem', 'tlmid', 'dt', 'bits', 'desc')

        # Configure columns
        self.column('#0', width=15, minwidth=15)
        self.column('mnem', width=400, minwidth=250)
        self.column('tlmid', width=40 ,minwidth=40)
        self.column('dt', width = 40, minwidth=40)
        self.column('bits', width=20, minwidth=20)
        self.column('desc')
        # Set headings
        self.heading('mnem', text='Mnemonic')
        self.heading('tlmid', text='Telem ID')
        self.heading('dt', text='Data Type')
        self.heading('bits', text='Bits')
        self.heading('desc', text='Description')

        # Bind Events
        self.bind("<ButtonPress-1>", self.__item_list_click)
        self.bind("<Control-c>", self.__ctr_c)
        self.bind("<Control-v>", self.__ctr_p)
        self.bind("<Control-x>", self.__ctr_x)

        # These bindings allow for dragging and dropping of rows
        self.bind("<ButtonPress-2>",self.left_click_down, add='+')
        self.bind("<ButtonRelease-2>", self.left_click_release, add='+')
        self.bind("<ButtonRelease-2>",self.move_row, add='+')
        self.bind("<B2-Motion>",self.move_row, add='+')
        self.bind("<Shift-ButtonPress-2>", self.bDown_Shift, add='+')
        self.bind("<Shift-ButtonRelease-2>", self.bUp_Shift, add='+')

        self.row_selected = True
        self.start_row = 0
        self.new_row = 0

        style = ttk.Style()
        style.configure('Treeview', background=myStyle.frameColor, foreground=myStyle.treeviewText, fieldbackground=myStyle.treeviewBackground)

    def __set_headers(self):
        # Add column descriptions to list
        for idx, col in enumerate(self.__col_list):
            self.grid_columnconfigure(index=idx, weight=1)
            col.grid(row=0, column=idx)

    def left_click_down(self, event):
        if self.row_selected == True:
            row = event.widget
            self.start_row = row.index(row.identify_row(event.y))
            self.row_selected = False
        row = event.widget
        if row.identify_row(event.y) not in row.selection():
            row.selection_set(row.identify_row(event.y))

    def left_click_release(self, event):
        self.row_selected = True
        row = event.widget
        if row.identify_row(event.y) in row.selection():
            row.selection_set(row.identify_row(event.y))
        try:
            self.__master.controller.packet_list_drag(self.start_row, self.new_row)
        except Exception, e:
            print("{}. In left_click_release() method in ItemList class (tpe_view.py)".format(e))

    def move_row(self, event):
        row = event.widget
        self.new_row = row.index(row.identify_row(event.y))
        for s in row.selection():
            row.move(s, '', self.new_row)

    def bUp_Shift(self, event):
        pass

    def bDown_Shift(self, event):
        pass


    def clear_entries(self):
        '''
        Delete all the items in the list
        This is called when the interface is reset
        '''
        self.delete(*self.get_children())


    def refresh(self, jump, last=False):
        """
        Delete existing items
        Insert items from master model.

        @param jump: Boolean. Controls whether selection jumps to last item.
        """
        self.delete(*self.get_children())

        try:
            item_list = self.__master.model.get_item_list()
        except Exception, e:
            print("That operation cannot be performed, there are no items currently loaded")
            tkMessageBox.showinfo("Info","Cannot perform that operation, there are no items currently loaded.")
            return

        for idx, item in enumerate(item_list):
            self.insert("" , idx, text=item.get_row(), values=item.as_list())
        if jump:   # Jump to a spot in the list
            try:
                if not last:   # Jump to the first slot
                    slot = self.get_children()[0]
                else:      # Jump to the last slot
                    slot = self.get_children()[-1]
                self.selection_set(slot)
                row = self.item(slot)['text']
                self.__master.controller.item_list_click(row)
            except IndexError, e:
                pass


    def reserve_present(self):
        item_list = self.__master.model.get_item_list()
        for i in item_list:
            temp = str(i)
            if 'Reserve' in temp[:7]:
                return True
        return False


    def check_reserve(self):
        '''
        Checks the item list of a packet to see if a Reserve item is present.
        If present it disables the 'Add Reserve' button so multiple reserves cannot be added to a packet
        If not present it allows a reserve to be added
        :return: parameter to set the state of the Add Reserve button (diabled/normal)
        '''

        item_list = self.__master.model.get_item_list()
        for i in item_list:
            temp = str(i)
            if 'Reserve' in temp[:7]:
                return "disabled"  # disable Add Reserve button because Reserve is already in the packet
        return "normal"

    def update_item(self, ch_name):
        """
        - Get id of current selection
        - Get item from model
        - Set item values
        """
        
        item_id = self.selection()[0]

        idx, item = self.__master.model.get_item_from_list(ch_name=ch_name)
        self.item(item_id, values=item.as_list())
            

    def __item_list_click(self, event):
        """
        Get selection list. 
        Hand off to controller.
        """

        item_id = self.identify_row(event.y)
        row = self.item(item_id)['text']

        #self.__master.controller.packet_list_drag(row)
        try:
            self.__master.controller.item_list_click(row)
        except IndexError, e:
            pass
        except ValueError, e:
            pass

    def __ctr_c(self, event):
        """
        Copy channel items into session controller clipboard.
        """
        selection = self.selection()
        item_list = self.__get_item_list_from_selection(selection)
        self.__master.controller.copy_items(item_list)

    def __ctr_p(self, event):
        """
        Request paste from session controller clipboard.
        """
        try:
            item_id = self.selection()[0]
            row = self.item(item_id)['text']
        except IndexError, e:
            row = 0
        self.__master.controller.paste_items(row)

    def __ctr_x(self, event):
        """
        Cut channel items into session controller clipboard.
        """
        selection = self.selection()
        item_list = self.__get_item_list_from_selection(selection)
        self.__master.controller.cut_items(item_list)

    def __get_item_list_from_selection(self, selection):
        """
        Return item list given treeview selection
        """
        item_list = []
        for item_id in selection:
            row = self.item(item_id)['text']
            idx, item = self.__master.model.get_item_from_list(row=row)
            item_list.append(item)
        return item_list


    def __item_list_release(self, event):
        x = event.x
        y = event.y
        col, row = self.grid_location(x,y)

    def add_space(self, bits):
        pass


