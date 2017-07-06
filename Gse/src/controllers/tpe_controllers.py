
import Tkinter
import tkFileDialog
import csv
import collections

from views.tpe_views import PacketView
from models.tpe_models import PacketModel
from models.tpe_models import SessionModel
from models.tpe_models import ItemEntry, FillerEntry


class SessionController(object):
	"""
	A session represents the running application. 
	SessionController handles the interaction between the SessionModel, PacketController, and PacketView.
	SessionModel holds data important to the application. Namely PacketModels.

	PacketView and PacketController are created without a PacketModel. 
	PacketModels can be created and loaded into the PacketView and PacketController.

	SessionController can load PacketModels into the PacketView. The PacketView loads the model into PacketController.
	(It would be better for SessionController to load the model into PacketController.)

	On init the SessionController creates a PacketView and PacketController.
	PacketView represents the application UI.
	PacketController handles the interaction between the PacketView and the loaded PacketModel. 
	PacketController also handles the loading and saving of PacketModels into csv files. 
	(This would better be a SessionController responsibility.)
	


	Changing between PacketModels allows the user to create/edit multiple packets during a session.
	"""


	def __init__(self, opts, root, session_model):

		self.__root = root
		self.__opts = opts

		self.__controller = PacketController(self, session_model)
		self.__view       = PacketView(root)

		self.__session_model = session_model

		self.__view.pack(anchor=Tkinter.N, expand=1, fill=Tkinter.BOTH)
		self.__view.config_session(session_model)

		self.__controller.load_view(self.__view)

	def create_session(self):

		# If we have models 
		if self.__session_model.get_model_list():

			# Start session with first model
			model = self.__session_model.get_model_list()[0]
			packet_list = self.__session_model.get_packet_list()
		
		else:
			# There are no packets to show
			model=None
			packet_list=None

		self.__view.load_resources(model=model, controller=self.__controller, packet_list=packet_list)

		# Select first packet
		try:
			pid = packet_list[0]
			self.__controller.packet_selected(pid)
		except TypeError, e:
			pass

	def save_model(self, model, update):
		"""
		- Append model to list
		- Get packet list
		- Load packet list
		- Load packet
		"""
		if update.lower() == 'update packet':
			update = True
		else:
			update = False

		if update:
			self.__session_model.update_model(model)
		else:
			self.__session_model.add_model(model)
			packet_list = self.__session_model.get_packet_list()
			self.__view.load_resources(packet_list=packet_list)
			self.load_packet(model.get_packet_id())

	def new_packet(self):
		"""
		- Create new empty packet.
		- Load empty model into view
		"""
		model = PacketModel(controller=self.__controller)
		model.set_packet_id('None Set')
		self.__view.load_resources(model=model)
		self.__view.refresh_item_list()

	def delete_packet(self, id):
		"""
		- Find and delete model
		- Reload resources
		"""
		self.__session_model.delete_packet(id)
		self.create_session() 


	def load_packet(self, packet_id):

		idx, model = self.__session_model.find_model(packet_id)
		self.__view.load_resources(model=model)
		self.__view.refresh_item_list(jump=True)
		self.__view.set_packet_dd(idx)

	def copy_items(self, item_list):
		"""
		Copy items into session model clipboard
		"""
		self.__session_model.copy_items(item_list)
	
	def cut_items(self, model, item_list):
		"""
		- Copy item_list into clipboard
		- Delete items from model
		"""
		self.__session_model.copy_items(item_list)

		for item in item_list:
			model.update_space_data(-int(item.get_bits()))
			model.delete_item(item.get_row())

		self.__view.refresh_item_list(jump=False)

	def paste_items(self, model, row):
		"""
		- Get copied items from clipboard
		- Splice into the model's item list.
		- Re add items to model
		- Refresh view
		"""

		item_list = self.__session_model.get_clipboard()
		
		old_items = model.get_item_list()
		model.clear_item_list()

		old_items[row+1:row+1] = item_list

		for item in old_items:
			model.add_item(item)

		self.__view.refresh_item_list(jump=False)

	def update_idletasks(self):
		self.__root.update_idletasks()

class PacketController(object):
	"""
	Tlm Packet Editor Controller
	"""
	def __init__(self, session_controller, session_model):

		self.__session_controller = session_controller
		self.__session_model      = session_model

		self.__model = None
		self.__view  = None

		self.__last_row = None

		# CSV Attributes
		self.__mnem_col   = 1
		self.__id_col     = 2
		self.__dt_col     = 3
		self.__bits_col   = 4
		self.__desc_col   = 5
		

	def load_session(self):
		csv_file = tkFileDialog.askopenfilename()

		if csv_file == '':
			return

		model_list = []
		with open(csv_file, 'rU') as csvfile:
			reader = csv.reader(csvfile, delimiter='\t')
			
			new_packet = False
			data_model = None
			for row in reader:
				row = row[0] # Extract string from list
				row = row.split(',') # Split into list
				row_header = row[0]
				row_header = row_header.lower()

				if row_header == 'packet':
					new_packet = not new_packet
					
					if data_model:
						model_list.append(data_model)

					data_model = None

				if new_packet:
					data_model = PacketModel(controller=self)
					new_packet = False

				if data_model:

					if row_header == 'identifier':
						pkt_name = row[self.__mnem_col]
						pkt_id = row[self.__id_col]
						data_model.set_packet_name(pkt_name)
						data_model.set_packet_id(pkt_id)

					if row_header == 'frequency':
						f = row[1]
						data_model.set_freq(f)
					if row_header == 'offset':
						o = row[1]
						data_model.set_offset(o)
					if row_header == 'channel':
						c = row[1]
						data_model.set_ff3_channel(c)

					if row_header == 'item':
						mnem   = row[self.__mnem_col]
						tlm_id = row[self.__id_col]
						dt     = row[self.__dt_col]
						bits   = row[self.__bits_col]
						desc   = row[self.__desc_col]
						item   = ItemEntry(mnem, tlm_id, dt, bits, desc)
						data_model.add_item(item)
					
					if row_header == 'reserve':
						bits = row[self.__bits_col]
						desc = row[self.__desc_col]
						item = FillerEntry(bits, self.__session_model.reserve_name, desc)
						data_model.add_item(item)

					if row_header == 'align':
						bits = row[self.__mnem_col]
						desc = row[self.__desc_col]
						item = FillerEntry(bits, self.__session_model.align_name, desc)
						data_model.add_item(item)

			if data_model:
				self.__session_model.validate_model(data_model, load=True)
				model_list.append(data_model)

		self.__session_model.set_model_list(model_list)
		self.__session_controller.create_session()


	def save_session(self):
		csv_file = tkFileDialog.asksaveasfilename(defaultextension='.csv')
		
		file_header_dict = self.__create_file_header_dict()
		header_dict  = self.__create_header_dict()
		column_list    = self.__create_column_list()
		

		with open(csv_file, 'w') as csvfile:
			writer = csv.writer(csvfile, delimiter=',')

			# Write File Header
			for item_name, columns in file_header_dict.iteritems():
				writer.writerow([item_name] + columns)

			writer.writerow([''])


			for model in self.__session_model.get_model_list():

				self.__session_model.validate_model(model, load=False)
				
				writer.writerow(['Packet'])

				# Write fields if they exist
				pid = model.get_packet_id()
				pnm = model.get_packet_name()
				if pid is not '' and pnm is not '':
					writer.writerow(['Identifier', pnm, pid])

				# Write headers
				for item_name, columns  in header_dict.iteritems():
					row = ['Header', item_name] + columns
					writer.writerow(row)

				freq = model.get_freq()
				if freq is not '':
					writer.writerow(['Frequency', freq])
				
				offset = model.get_offset()
				if offset is not '':
					writer.writerow(['Offset', model.get_offset()])
				
				channel = model.get_ff3_channel()
				if channel is not '':
					writer.writerow(['Channel', model.get_ff3_channel()])


				# Write Column Names
				writer.writerow(column_list)
				
				# Write Channels
				for item in model.get_item_list():
					writer.writerow(item.as_csv_list())

				writer.writerow([''])


	def __create_file_header_dict(self):
		"""
		Get file header from .ini file and return dictionary. 
		"""
		d = collections.OrderedDict()
		d['Duration']  = [10, 'Duration in seconds only.']
		d['RunPeriod'] = [10, 'Run Period or Sample Perioud in Hz']
		return d

	def __create_header_dict(self):
		"""
		Get packet header from .ini file and return dictionary.
		"""
		
		d = collections.OrderedDict()
		d['ID'] = ['','',8]
		d['Sequence'] = ['','',8, 'Sequence Number']
		d['Time'] = []

		return d

	def __create_column_list(self):
		return ["Comment", "Mnemonic", "Tlm DB Identifier", "Tlm DB Datatype", "Bits", "Desc"]

	def add_reserve(self):
		bits = self.__model.reserve_var.get()
		if bits == '':
			return

		reserve_obj = FillerEntry(bits, self.__session_model.reserve_name, self.__session_model.reserve_desc)
		self.__generic_add(reserve_obj)

	def add_align(self, desc, bits):
		desc = self.__session_model.align_desc.format(desc)
		item_obj = FillerEntry(bits, self.__session_model.align_name, desc)

		self.__generic_add(item_obj)

	def add_item(self, update):
			"""
			Gather data from editor entries and create a
			channel entry.
			"""
			if update.lower() == 'update':
				update = True
			else:
				update = False

			ch_name = self.__session_model.selected_channel_var.get()

			if update:
				self.__model.update_item(ch_name)
				self.__view.refresh_item_list(jump=False)
			else:
				ch_obj  = self.__session_model.get_channel_dict()[ch_name]
				item_entry = ItemEntry(ch_obj=ch_obj)
				self.__generic_add(item_entry)

	def __generic_add(self, item_obj):
		"""
		Add any item_obj and refresh item list
		"""
		self.__model.add_item(item_obj)
		self.__view.refresh_item_list(jump=True)


	def comp_selected(self, comp_name):
		"""
		Filter channel names via component.
		"""
		if comp_name == "ALL":
			self.__view.set_channel_dd_items(self.__session_model.get_channel_names())
		
		else:
			component_channels = []
			for channel in self.__session_model.get_channel_dict().values():
				if channel.getCompName() == comp_name:
					component_channels.append(channel.getName())

			self.__view.set_channel_dd_items(component_channels)

	def channel_selected(self, ch_name):
		"""
		- Get channel object
		- Load it's info into the model
		"""
		try:
			ch_obj = self.__session_model.get_channel_dict()[ch_name]
			self.__model.set_channel_info(ch_obj=ch_obj)
			self.__view.set_add_channel_button_text('Add')
		except KeyError, e:
			pass

	def packet_selected(self, packet_id):
		"""
		- Load packet
		- Set Save to Update
		"""
		self.__session_controller.load_packet(packet_id)
		self.__view.hide_save_packet_button(True)

	def new_packet(self):
		self.__session_controller.new_packet()
		self.__view.set_packet_select_text("None Set")
		self.__view.hide_save_packet_button(False)
		#self.__view.set_component_dd(0)


	def delete_packet(self):
		packet_id = self.__model.get_packet_id()
		self.__session_controller.delete_packet(packet_id)

	def save_packet(self, update):
		self.__session_controller.save_model(self.__model, update)
		self.__view.hide_save_packet_button(True)


	def item_list_click(self, row):
		idx = int(row)
		try:
			item_obj = self.__model.get_item_list()[idx]
			if item_obj:
				self.__view.set_channel_select_text(item_obj.get_name())
				self.__model.set_channel_info(item_obj=item_obj)
				self.__view.set_add_channel_button_text('Update')

		except IndexError, e:
			pass

			
	def packet_list_drag(self, row):
		print self.__last_row
		if self.__last_row == None:
			self.__last_row = row
		elif self.__last_row == row:
			self.__last_row = None
		else:
			try:
				self.__model.swap_rows(self.__last_row, row)
				self.__view.refresh_item_list()
			except Exception, e:
				print e
			finally:
				self.__last_row = None


	def copy_items(self, item_list):
		self.__session_controller.copy_items(item_list)

	def cut_items(self, item_list):
		self.__session_controller.cut_items(self.__model, item_list)

	def paste_items(self, row):
		"""
		Request session controller to paste items
		into current model. Paste after treeview row.  
		"""
		self.__session_controller.paste_items(self.__model, row)

	def delete_item(self):
		"""
		- Get item.
		- Increase available size. 
		- Delete from model.
		- Update view.
		"""
		ch_name     = self.__session_model.selected_channel_var.get()
		idx, item_obj    = self.__model.get_item_from_list(ch_name=ch_name)	

		size = int(item_obj.get_bits())
		self.__model.update_space_data(-size)
		self.__model.delete_item(idx)
		self.__view.refresh_item_list(jump=False)


	def update_idletasks(self):
		self.__session_controller.update_idletasks()

	def load_model(self, model):
		self.__model = model

	def load_view(self, view):
		self.__view = view
