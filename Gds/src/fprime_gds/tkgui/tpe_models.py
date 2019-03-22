import Tkinter
from fprime_gds.tkgui.controllers import channel_loader

class FillerEntry(object):
	"""
	Object for Align and Reserve entries.

	Has same interface as ItemEntry.

	FillerEntry.instance ensures unique names.
	"""
	instance = 0

	def __init__(self, bits, name, desc):
		FillerEntry.instance += 1

		self.__bits = bits
		self.__row  = None
		self.__name = name
		self.__desc = desc

		# Needed to differentiate between multiple Reserves and Aligns
		self.__instance = FillerEntry.instance

	def set_row(self, row):
		self.__row = row
	def get_row(self):
		return self.__row
	def get_desc(self):
		return self.__desc
	def set_desc(self, d):
		self.__desc = d
	def set_size(self, bits):
		self.__bits = bits
	def get_bits(self):
		return self.__bits
	def get_name(self):
		return self.__name + str(self.__instance)
	def as_list(self):
		return [self.__name, self.__bits, '', '', self.__desc]
	def as_csv_list(self):
		if self.__name.lower() == 'reserve':
			return [self.__name, '', '', '', self.__bits, self.__desc]
		if self.__name.lower() == 'align':
			return  [self.__name, self.__bits, '', '', '', self.__desc]
	def __repr__(self):
		return '|'.join(map(str,self.as_list()))




class ItemEntry(object):
	"""
	Object representing an item row.
	"""
	def __init__(self, mnem=None, tlmid=None, dt=None, bits=None, desc=None, ch_obj=None):

		self.__ch_obj    = None
		self.__row       = None
		self.__mnem      = mnem
		self.__tlmid     = tlmid
		self.__datatype  = dt
		self.__bits      = bits
		self.__desc      = desc

		if ch_obj:
			self.__set_ch_obj(ch_obj)

	def __repr__(self):
		return ('|').join(map(str,self.as_list()))

	def __set_ch_obj(self, ch_obj):
		self.__ch_obj    = ch_obj
		self.__mnem      = ch_obj.getName()
		self.__tlmid     = ch_obj.getId()
		self.__datatype  = ch_obj.getType()
		self.__bits      = self.__datatype.getSize() * SessionModel.byte_size
		self.__desc      = ch_obj.getChDesc()

	def set_tlmid(self, id):
		self.__tlmid = id
	def get_tlmid(self):
		return self.__tlmid

	def set_row(self, row):
		self.__row = row
	def get_row(self):
		return self.__row
	def set_size(self, bits):
		self.__bits = bits
	def get_bits(self):
		return int(self.__bits)
	def as_list(self):
		return [self.__mnem, self.__tlmid, self.__datatype, self.__bits, self.__desc]
	def as_csv_list(self):
		return ["Item", self.__mnem, self.__tlmid, self.__datatype, self.__bits, self.__desc]
	def set_desc(self, string):
		self.__desc = string
	def get_desc(self):
		return self.__desc
	def get_ch_obj(self):
		return self.__ch_obj
	def get_name(self):
		return self.__mnem



class SessionModel(object):
	"""
	Running application Model.

	Holds a list of packets.
	Holds clipboard for cut/copy/paste operations.
	Holds filler names

	"""

	byte_size	  = 8
	word_size     = byte_size * 2
	packet_size   = 56

	def __init__(self, ch_loader):

		self.__ch_loader = ch_loader
		self.__ch_dict   = self.__ch_loader.getChDictByName()
		self.__ch_comp_dict = self.__ch_loader.getCompDict()

		self.selected_channel_var = Tkinter.StringVar()
		self.pid_select_var = Tkinter.StringVar()

		self.reserve_name = "Reserve"
		self.reserve_desc = "Spare"
		self.align_name   = "Align"
		self.align_desc   = "{} Align"

		self.__model_list = []
		self.__model_clipboard = []


	def get_model_list(self):
		return self.__model_list

	def get_packet_list(self):
		"""
		Iterate through model_list and get packet ids
		"""
		packet_list = [m.get_packet_id() for m in self.__model_list]
		return packet_list

	def find_model(self, packet_id):
		for idx, m in enumerate(self.__model_list):
			if m.get_packet_id() == packet_id:
				return idx, m

	def set_model_list(self, ml):
		"""
		- Set model list.
		"""
		self.__model_list = ml

	def update_model(self, new_model):
		id = new_model.get_packet_id()

		try:
			idx, old_model = self.find_model(id)
		except Exception, e:
			raise e

		old_model.update(new_model)

	def add_model(self, model):
		self.__model_list.append(model)

	def delete_packet(self, id):
		for idx, model in enumerate(self.__model_list):
			if model.get_packet_id() == id:
				del self.__model_list[idx]

	def validate_model(self, packet_model, load=False):
		"""
		Iterate through items in packet and compare to dicationary.
		"""
		print "Validating Packets...\n"

		for item in packet_model.get_item_list():

			mnem = item.get_name()

			try:
				ch_obj = self.__ch_dict[mnem]
			except KeyError, e:
				print "Cannot find channel object with mnem {}".format(mnem)

			# Check size
			datatype  = ch_obj.getType()
			ch_bits   = datatype.getSize() * SessionModel.byte_size

			if item.get_bits() != ch_bits:
				print "{} Bit size mismatch. Dictionary: {} Received: {}".format(item.get_name(), ch_bits, item.get_bits())


			# Check telem id
			# Only Change values on save.
			if int(item.get_tlmid()) != int(ch_obj.getId()):

				msg  = "{} Telem ID mismatch. Dictionary: {}. Recieved: {}".format(item.get_name(), ch_obj.getId(), item.get_tlmid())
				if not load:

					msg = msg + "\nChanging {} to dictionary value: {}".format(item.get_name(), item.get_tlmid(),
																							 ch_obj.getId())
					item.set_tlmid(ch_obj.getId())

				print msg
			#

	def copy_items(self, item_list):
		self.__item_clipboard = item_list

	def get_clipboard(self):
		return self.__item_clipboard

	def get_channel_names(self):
		return self.__ch_dict.keys()

	def get_channel_dict(self):
		return self.__ch_dict

	def get_comp_names(self):
		return ['ALL'] + self.__ch_comp_dict.values()

class PacketModel(object):
	"""
	Tlm Packet Editor Model.

	This model represents a packet. Channels are stored in
	item_list.
	"""
	def __init__(self, controller):

		self.__controller = controller

		# Packet Attributes
		self.pid_entry_var    	  = Tkinter.StringVar()
		self.offset_var 		  = Tkinter.StringVar()
		self.freq_var   		  = Tkinter.StringVar()
		self.ff3_channel_var      = Tkinter.StringVar()
		self.packet_name_var      = Tkinter.StringVar()


		# Active Entries
		self.__item_list = []

		# Space variables
		self.used_space           = 0
		self.avail_space          = SessionModel.packet_size * SessionModel.byte_size # 56 byte payload * 8 bit/byte
		self.__avail_text         = "Bits Left: {}"
		self.__used_text          = "Bits Used: {}"

		# Tkinter variables to hold entry and dropdown data
		self.reserve_var          = Tkinter.StringVar()
		self.size_var  			  = Tkinter.StringVar()
		self.used_space_var       = Tkinter.StringVar()
		self.avail_space_var      = Tkinter.StringVar()
		self.desc_var 			  = Tkinter.StringVar()

		# Initalize space data
		self.update_space_data(0)


	def set_packet_id(self, id):
		self.pid_entry_var.set(id)

	def get_packet_id(self):
		return self.pid_entry_var.get()

	def set_packet_name(self, name):
		self.packet_name_var.set(name)

	def get_packet_name(self):
		return self.packet_name_var.get()

	def set_freq(self, f):
		self.freq_var.set(f)

	def get_freq(self):
		return self.freq_var.get()

	def set_offset(self, o):
		self.offset_var.set(o)

	def get_offset(self):
		return self.offset_var.get()

	def set_ff3_channel(self, c):
		self.ff3_channel_var.set(c)

	def get_ff3_channel(self):
		return self.ff3_channel_var.get()

	def update_channel_entry(self, ch_name, new_ch_obj):
		"""
		Search for matching entry.
		"""
		update_idx = None
		for idx, ch_obj in enumerate(self.__entry_list):
			if ch_obj:
				if ch_name == ch_obj.get_name():
					update_idx = idx

		self.__entry_list[update_idx] = new_ch_obj
		self.set_editor_data(new_ch_obj)

	def set_editor_data(self, item_obj):
		"""
		Set editor entry_object data.
		"""
		#self.pid_var.set(entry_object.get_packetId())
		self.offset_var.set(entry_object.get_offset())
		self.freq_var.set(entry_object.get_freq())

	def add_item(self, item_obj):
		"""
		- Return if duplicate
		- Else: Add entry object to item list.
		- Hold index of item in dictionary.
		- Update space data.
		"""
		for item in self.__item_list:
			if item.get_name() == item_obj.get_name():
				return

		next_idx = len(self.__item_list)
		item_obj.set_row(next_idx)
		self.__item_list.append(item_obj)

		size = item_obj.get_bits()
		self.update_space_data(size)

	def update_item(self, ch_name):
		"""
		- Search for item
		- Update size
		- Update fields
		"""
		idx, item = self.get_item_from_list(ch_name=ch_name)

		old_size = item.get_bits()
		new_size = self.size_var.get()
		new_desc = self.desc_var.get()

		diff_size = old_size - int(new_size)
		self.update_space_data(-diff_size)

		item.set_size(new_size)
		item.set_desc(new_desc)

	def update_space_data(self, channel_size):
		"""
		@param channel_size: Size in bits
		"""
		self.used_space  += int(channel_size)
		self.avail_space -= int(channel_size)

		self.used_space_var.set(self.__used_text.format(self.used_space))
		self.avail_space_var.set(self.__avail_text.format(self.avail_space))

	def set_channel_info(self, ch_obj=None, item_obj=None):
		"""
		Set size and description fields given a channel object
		or TPE item object.
		"""
		size = 0
		desc = ''
		if ch_obj:
			size = ch_obj.getType().getSize() * SessionModel.byte_size
			desc = ch_obj.getChDesc()
		if item_obj:
			size = item_obj.get_bits()
			desc = item_obj.get_desc()

		self.size_var.set(size)
		self.desc_var.set(desc)

	def delete_item(self, idx):
		"""
		- Remove item from list
		- Update rows
		"""
		del self.__item_list[idx]

		for idx, item in enumerate(self.__item_list):
			item.set_row(idx)

	def clear_item_list(self):
		"""
		- Delete all items from list.
		- Assign to empty list
		"""
		for item in self.__item_list:
			ch_size = int(item.get_bits())
			self.update_space_data(-ch_size)

			del item

		self.__item_list = []

	def get_item_from_list(self, ch_name=None, row=None):
		"""
		Get item given ch_name or treeview row
		"""
		for idx, item_obj in enumerate(self.__item_list):
			if ch_name == item_obj.get_name():
				return idx, item_obj
			if row == item_obj.get_row():
				return idx, item_obj


	def swap_rows(self, old, new):
		tmp_old = self.__entry_list[old]
		tmp_new = self.__entry_list[new]
		self.__entry_list[old] = tmp_new
		self.__entry_list[new] = tmp_old


	def get_item_list(self):
		return self.__item_list



	def __del__(self):
		"""
		Remove variable observer
		"""
		pass
		#self.selected_channel_var.trace_vdelete('w', self.__sc_obs)
