import Tkinter
import Pmw
import ttk

from models.tpe_models import SessionModel

class PacketView(Tkinter.Frame):

	def __init__(self, root):
		Tkinter.Frame.__init__(self, master=root, background='lightblue') 

		self.model         = None
		self.controller    = None

		top_frame = Tkinter.Frame(self)
		top_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X)
		# Menu Bar
		menuBar = Pmw.MenuBar(top_frame, hull_relief=Tkinter.RAISED, hull_borderwidth=1)
		menuBar.pack(side=Tkinter.LEFT, anchor=Tkinter.NW, expand = 1, fill=Tkinter.X)
		menuBar.addmenu('File', None)
 		menuBar.addmenuitem('File', 'command', font=('StingerLight', 14), label='Save...', command=self.save_session)
		menuBar.addmenuitem('File', 'command', font=('StingerLight', 14), label="Load...", command=self.load_session)

		self.__used_label = Tkinter.Label(top_frame)
		self.__used_label.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE)
		self.__avail_label  = Tkinter.Label(top_frame)
		self.__avail_label.pack(side=Tkinter.RIGHT, anchor=Tkinter.NE)

		## Session View
		session_select_frame = Tkinter.LabelFrame(self, text="Session")
		session_select_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=10, padx=10)
		self.__new_packet_button = Tkinter.Button(session_select_frame, text="New Packet", command=self.__new_packet)
		self.__new_packet_button.pack(side=Tkinter.LEFT)
		self.__select_packet_dd = Pmw.ComboBox(session_select_frame, labelpos='w', label_text='Packet: ', selectioncommand=self.__packet_selected)
		self.__select_packet_dd.pack(side=Tkinter.LEFT, anchor=Tkinter.NW)
		self.__delete_packet_button = Tkinter.Button(session_select_frame, text="Delete Packet", command=self.__delete_packet)
		self.__delete_packet_button.pack(side=Tkinter.LEFT, anchor=Tkinter.NW)

		## Packet Edit Frame
		packet_edit_frame = Tkinter.LabelFrame(self, text='Packet Attributes')
		packet_edit_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=10, padx=10)
		##

		self.__packet_id_entry = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='Packet ID:')
		self.__packet_id_entry.grid(row=0, column=0, sticky=Tkinter.EW)
		#
		self.__freq_entry      = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='Frequency:')
		self.__freq_entry.grid(row=0, column=1, sticky=Tkinter.EW)
		#
		self.__offset_entry    = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='Offset:')
		self.__offset_entry.grid(row=0, column=2, sticky=Tkinter.EW)
		#
		self.__packet_save_button = Tkinter.Button(packet_edit_frame, text="Save Packet", command=self.__save_packet)
		self.__packet_save_button.grid(row=2, column=0, sticky=Tkinter.W)
		#
		self.__packet_name_entry = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='Packet Name:')
		self.__packet_name_entry.grid(row=1, column=0, sticky=Tkinter.EW)

		self.__ff3_channel_entry = Pmw.EntryField(packet_edit_frame, labelpos='w', label_text='FF3 Channel:')
		self.__ff3_channel_entry.grid(row=1, column=1, sticky=Tkinter.EW)
		##

		## Item Edit Frame
		item_edit_frame = Tkinter.LabelFrame(self, text='Items')
		item_edit_frame.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.X, pady=10, padx=10)
		#
		self.__select_comp_dd = Pmw.ComboBox(item_edit_frame, labelpos='w', label_text="Select Component:", selectioncommand=self.__comp_selected)
		self.__select_comp_dd.grid(row=1, column=0, sticky=Tkinter.EW)
		#
		self.__select_channel_dd = Pmw.ComboBox(item_edit_frame, labelpos='w', label_text='Select Channel:', selectioncommand=self.__channel_selected)
		self.__select_channel_dd.grid(row=1, column=1, sticky=Tkinter.EW)

		self.__add_reserve_button = Tkinter.Button(item_edit_frame, text="Add Reserve", command=self.__add_reserve)
		self.__add_reserve_button.grid(row=1, column=3, sticky=Tkinter.E)
		self.__reserve_entry = Pmw.EntryField(item_edit_frame, labelpos='w', label_text='Bits: ')
		self.__reserve_entry.grid(row=1, column=4, sticky=Tkinter.W)

		self.__add_item_button = Tkinter.Button(item_edit_frame, text='Add', command=self.__add_item)
		self.__add_item_button.grid(row=4, column=1, sticky=Tkinter.W)
		#
		self.__delete_item_button = Tkinter.Button(item_edit_frame, text="Delete", command=self.__delete_row)
		self.__delete_item_button.grid(row=5, column=1, sticky=Tkinter.W)
		
		self.__size_entry = Pmw.EntryField(item_edit_frame, labelpos='w', label_text='Size in Bits:')
		self.__size_entry.grid(row=2, column=1, sticky=Tkinter.W + Tkinter.E)

		self.__desc_entry = Pmw.EntryField(item_edit_frame, labelpos='w', label_text='Desc:')
		self.__desc_entry.grid(row=3, column=1, sticky=Tkinter.W + Tkinter.E)

		self.__byte_align_button = Tkinter.Button(item_edit_frame, text="Byte Align", command=self.__byte_align)
		self.__byte_align_button.grid(row=2, column=3, sticky=Tkinter.W)
		self.__word_align_button = Tkinter.Button(item_edit_frame, text="Word Align", command=self.__word_align)
		self.__word_align_button.grid(row=3, column=3, sticky=Tkinter.W)

		##


		# Configure editor frame grid
		for i in range(5):
			packet_edit_frame.grid_columnconfigure(i, weight=1)
			item_edit_frame.grid_columnconfigure(i, weight=1)
		##

		## List Frame
		self.__item_list = ItemList(self)
		self.__item_list.pack(side=Tkinter.TOP, anchor=Tkinter.N, expand=1, fill=Tkinter.BOTH)
		##


	def save_session(self):
		self.controller.save_session()

	def load_session(self):
		self.controller.load_session()


	def load_resources(self, model=None, controller=None, packet_list=None):
		"""
		- Set model and controller.
		- Load dropdowns.
		- Connect labels and entries to model variables
		"""

 		if controller:
			self.controller = controller

		if packet_list:
			self.__select_packet_dd.setlist(packet_list)

		if model:
			self.__load_model(model)


		self.controller.update_idletasks()

	def __load_model(self, model):
		
		self.model = model

		try:
			self.controller.load_model(model)
		except Exception, e:
			pass

		# Connect model variables
		self.__packet_id_entry.component('entry').config(textvariable=model.pid_entry_var)
		self.__offset_entry.component('entry').config(textvariable=model.offset_var)
		self.__freq_entry.component('entry').config(textvariable=model.freq_var)
		self.__ff3_channel_entry.component('entry').config(textvariable=model.ff3_channel_var)
		self.__packet_name_entry.component('entry').config(textvariable=model.packet_name_var)
		self.__reserve_entry.component('entry').config(textvariable=model.reserve_var)
		self.__size_entry.component('entry').config(textvariable=model.size_var)
		self.__desc_entry.component('entry').config(textvariable=model.desc_var)
		self.__used_label.config(textvariable=model.used_space_var)
		self.__avail_label.config(textvariable=model.avail_space_var)

		self.refresh_item_list()
	
	def config_session(self, session_model):
		"""
		Packet select is session variable
		"""
		self.__select_packet_dd.component('entry').config(textvariable=session_model.pid_select_var)
		self.__select_channel_dd.component('entry').config(textvariable=session_model.selected_channel_var)

		# Populate dropdowns
		channels = session_model.get_channel_names()
                channels.sort()
		self.__select_channel_dd.setlist(channels)

		components = session_model.get_comp_names()
		self.__select_comp_dd.setlist(components)


	def __packet_selected(self, entry):
		self.controller.packet_selected(entry)

	def __new_packet(self):
		self.controller.new_packet()

	def __delete_packet(self):
		self.controller.delete_packet()

	def __save_packet(self):
		update = self.__packet_save_button.cget("text")
		self.controller.save_packet(update)

	def __delete_row(self):
		self.controller.delete_item()

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

	def __add_reserve(self):
		self.controller.add_reserve()

	def __byte_align(self):
		self.controller.add_align('Byte', SessionModel.byte_size)
	def __word_align(self):
		self.controller.add_align('Word', SessionModel.word_size)

	def __add_item(self):
		"""
		Get data from editor and pass to model
		"""
		update = self.__add_item_button.cget('text')
		self.controller.add_item(update)

	def set_add_channel_button_text(self, t):
		"""
		Set the button text for item add.
		'Update' or 'Add' invoke different behaviors.
		"""
		self.__add_item_button.config(text=t)

	def hide_save_packet_button(self, b):
		if b:
			self.__packet_save_button.grid_remove()
		else:
			self.__packet_save_button.grid()

	def set_packet_dd(self, idx):
		self.__select_packet_dd.selectitem(idx)

	def set_component_dd(self, idx):
		self.__select_comp_dd.selectitem(idx)

		comp_name = self.__select_comp_dd.get()
		self.__comp_selected(comp_name)

	def set_channel_dd(self, idx):
		self.__select_channel_dd.selectitem(idx)

	def set_packet_select_text(self, t):
		self.__select_packet_dd.component('entry').delete(0, Tkinter.END)
		self.__select_packet_dd.component('entry').insert(0, t)

	def refresh_item_list(self, jump=False):
		self.__item_list.refresh(jump)

	def set_channel_select_text(self, text):
		self.__select_channel_dd.component('entry').delete(0, Tkinter.END)
		self.__select_channel_dd.component('entry').insert(0, text)
		self.__channel_selected(text)

	def set_channel_dd_items(self, channels):
		self.__select_channel_dd.setlist(channels)
		self.set_channel_dd(0)

		ch_name = self.__select_channel_dd.get()
		self.__channel_selected(ch_name)

	def update_item(self, ch_name):
		self.__item_list.update_item(ch_name)


class ItemList(ttk.Treeview):
	"""
	List of packets
	"""

	def __init__(self, master):
		ttk.Treeview.__init__(self, master)

		self.__master = master
		# Starting row
		self.__row = 0

		self["columns"] = ('mnem', 'tlmid', 'dt', 'bits', 'desc')

		# Configure columns
		self.column('#0', width=5)
		self.column('mnem')
		self.column('tlmid', width=50)
		self.column('dt', width=50)
		self.column('bits', width=25)
		self.column('desc')
		# Set headings
		self.heading('mnem', text='Mnemonic')
		self.heading('tlmid', text='Telem ID')
		self.heading('dt', text='Data Type')
		self.heading('bits', text='Bits')
		self.heading('desc', text='Description')

		# Bind Events
		self.bind("<Button-1>", self.__item_list_click)
		#self.bind("<ButtonRelease-1>", self.__packet_list_release)
		self.bind("<Control-c>", self.__ctr_c)
		self.bind("<Control-v>", self.__ctr_p)
		self.bind("<Control-x>", self.__ctr_x)

	def __set_headers(self):
		# Add column descriptions to list
		for idx, col in enumerate(self.__col_list):
			self.grid_columnconfigure(index=idx, weight=1)
			col.grid(row=0, column=idx)


	def refresh(self, jump):
		"""
		Delete existing items
		Insert items from master's model.

		@param jump: Boolean. Controlls whether selection jumps to last item. 
		"""
		self.delete(*self.get_children())

		item_list = self.__master.model.get_item_list()
		for idx, item in enumerate(item_list):
			self.insert("" , idx, text=item.get_row(), values=item.as_list())		

		if jump:
			try:
				last_item = self.get_children()[-1]
				self.selection_set(last_item)
				row = self.item(last_item)['text']
				self.__master.controller.item_list_click(row)
			except IndexError, e:
				pass

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
		#self.__master.controller.packet_list_drag(row)

	def add_space(self, bits):
		pass


