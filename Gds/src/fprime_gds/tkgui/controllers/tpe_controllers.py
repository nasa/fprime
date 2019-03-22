from __future__ import print_function
import Tkinter
import tkMessageBox
import tkFileDialog
import csv
import collections

from fprime_gds.tkgui.views.tpe_views import PacketView
from fprime_gds.tkgui.tpe_models import PacketModel
from fprime_gds.tkgui.tpe_models import SessionModel
from fprime_gds.tkgui.tpe_models import ItemEntry, FillerEntry


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


    def create_session(self, new_session=True):
        # If we have models
        if self.__session_model.get_model_list():
           # Start session with first model
            model = self.__session_model.get_model_list()[0]
            packet_list = sorted(self.__session_model.get_packet_list())
        else:
            # There are no packets to show
            model=None
            packet_list=None
            if not new_session:
                self.__view.reinitialize_session()

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
            self.__view.load_resources(packet_list=sorted(packet_list))
            self.load_packet(model.get_packet_id())

    def new_packet(self):
        """
        - Create new empty packet.
        - Load empty model into view
        """
        model = PacketModel(controller=self.__controller)
        self.__view.load_resources(model=model)
        self.__view.refresh_item_list(jump=False)

    def delete_packet(self, id):
        """
        - Find and delete model
        - Reload resources
        """
        self.__session_model.delete_packet(id)
        self.create_session(new_session=False)


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
            model.update_space_data(-int(item.get_bits()), load=False)
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

        self.auto_pkt_id = 65535 # Max size of a U16
        self.duration = 10
        self.tlm_rate = 10
        self.bits_per_packet = 448    # num of bits in 64 bytes


    def check_pkt_id(self,id, id_list):
        '''
        # Verify that pkt_id is unique, if not give it a temporary, unique ID
        # Since pkt_ids are checked in a line by line fashion by the load_session method, a unique value cannot be
        # guaranteed, since upcoming values are unknown.  So if there are duplicates or illegal id's the largest U16
        # value is used for the baseline of pkt_ids assigned by the script
        # Check that the id is a U16, then that it is unique
        @param id:
        @return: zero(0) if the pkt_id is valid, if invalid return a new packet id
        '''
        try:
            test_id = id
        except ValueError, e:
            print("!!! ERROR: Invalid packet id: {}".format(e))
            self.auto_pkt_id -= 1
            return str(self.auto_pkt_id)     # Use large U16 values to flag pkt_ids assgined by the stript

        # Check for a valid range
        try:
            if int(test_id) in range(0, 65635) and test_id not in id_list:  #Valid value and unique
                return id
        except Exception, e:
            print("!!! ERROR: Invalid Packet ID: {}".format(id))
            print('Exception: {}'.format(e))
            print(id_list)
            for i in id_list:
                print(i)
                if (self.auto_pkt_id-1) not in id_list:
                    self.auto_pkt_id -= 1
                    print('Valid temporary Packet ID filled in: {}.'.format(self.auto_pkt_id))
                    return str(self.auto_pkt_id)
            print('Could not find a valid Packet ID.  Check CSV file.')
            tkMessageBox.showwarning("Warning", "Invalid Packet ID: {}.  '-1'  will be entered.  Change it to a valid ID.".format(id))
            return(-1)


    def load_session(self):

        try:
            csv_file = tkFileDialog.askopenfilename()
            if csv_file == '':     # Cancel
                return False
            elif '.csv' not in csv_file[-4:]:
                print("ERROR.  File does not have a '.csv' extention.")
                tkMessageBox.showwarning("csv extentions Warning",
                                         "File: '{}' does not have a '.csv' extention.".format(csv_file))
                return False

        except Exception, e:
            print('Exception on reading file.')
            print('Returned: {}'.format(e))
            return False

        pkt_id = None
        pkt_id_list = []  # Running list of packets id's being validated
        model_list = []
        i = 0

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
                    data_model = None

                if new_packet:
                    data_model = PacketModel(controller=self)
                    new_packet = False

                # This condition indicates that all the channels are assigned to the packet
                if row == [''] and data_model != None:
                    # Verify proper packet id
                    val = self.check_pkt_id(pkt_id, pkt_id_list)
                    pkt_id_list.append(val)       # add to list of current pkt_ids
                    data_model.set_packet_id(val) # Assign packet id for display
                    self.__session_model.header_check(data_model)
                    model_list.append(data_model)
                    self.__session_model.validate_item(data_model, pkt_id, total_bits, load=True, verbose=True)

                if data_model:
                    if row_header == 'identifier':
                        pkt_name = row[self.__mnem_col]
                        pkt_id = row[self.__id_col]

                        data_model.set_packet_name(pkt_name)

                        data_model.set_packet_id(pkt_id)
                        total_bits = 0   # new packet, initialize the number of bits in the packet

                    if row_header == 'frequency':
                        f = row[1]
                        try:
                            if f is not 'Time Correlated':
                                freq = f
                                data_model.set_offset('N/A')
                            else:
                                freq = format(float(f), '.1f')
                        except Exception, e:
                            print("Invalid Frequency: {}".format(f))
                            print('Exception: {}'.format(e))
                            print('Frequency set to -1')
                            tkMessageBox.showwarning("Warning",
                                                     "Invalid Frequency in file: {}.  Select a valid value from the pulldown."
                                                     .format(f))
                        data_model.set_freq(freq)
                    if row_header == 'offset':
                        obj = row[1]
                        try:
                            if obj is not 'N/A':
                                obj = int(obj)
                        except Exception, e:
                            print("Invalid Offset: {}".format(obj))
                            print('Exception: {}'.format(e))
                            print('Offset set to 0')
                            tkMessageBox.showwarning("Warning",
                                                     "Invalid Offset in file.  Re-select frequency to refresh the offset pulldown. Offset will be set to 0."
                                                     .format(obj))
                            obj = 0
                        data_model.set_offset(obj)

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
                        data_model.add_item(item, load=True, pkt_id=pkt_id)
                        total_bits += int(bits)

                    if row_header == 'reserve':
                        bits = row[self.__bits_col]
                        desc = row[self.__desc_col]
                        item = FillerEntry(bits, self.__session_model.reserve_name, desc)
                        data_model.add_item(item, load=True, pkt_id=pkt_id)

                    if row_header == 'align':
                        bits = row[self.__mnem_col]
                        desc = row[self.__desc_col]
                        item = FillerEntry(bits, self.__session_model.align_name, desc)
                        data_model.add_item(item, load=True, pkt_id=pkt_id)
                        total_bits += int(bits)

        self.__session_model.set_model_list(model_list)
        self.__session_controller.create_session()
        return True

    def validate_session(self):
        models = self.__session_model.get_model_list()
        if not models:   # no models loaded (an empty list)
            return
        for model in models:
            pkt_id = model.get_packet_id()
            self.__session_model.validate_item(model, pkt_id, load=False, verbose=True)
            self.__session_model.check_packet_space(model)
            self.__session_model.header_check(model)
        print("Validation complete.")
        tkMessageBox.showinfo("Validation Complete",
                              "Validated {} packets".format(len(models)))


    def save_session(self):
        csv_file = tkFileDialog.asksaveasfilename(defaultextension='.csv')
        if not csv_file: return     # "cancel" was selected

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
               # self.__session_model.validate_model(model, load=False)
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

                # Test for and write Frequency if the packet is periodic
                freq = model.get_freq()
                # These are the cases where 'Frequency' is not written to the csv file
                no_freq = ['', 'Aperiodic', 'Time Correlated']
                if freq not in no_freq:
                    writer.writerow(['Frequency', format(float(freq), '.1f')])
                elif freq == 'Time Correlated':
                    writer.writerow(['Frequency, {}'.format(freq)])

                offset = model.get_offset()
                # These cases are for aperiodic packets where offset is meaningless so we do not write it to the file
                no_offset = ['', 'N/A']
                if offset not in no_offset:
                    writer.writerow(['Offset', model.get_offset()])

                channel = model.get_ff3_channel()
                if channel != '':
                    writer.writerow(['Channel', model.get_ff3_channel()])

                # Write Column Names
                writer.writerow(column_list)

                # Write Channels
                for item in model.get_item_list():
                    writer.writerow(item.as_csv_list())

                writer.writerow([''])


    def verify_unique_pkt_id(self, id):
        '''
        Test against the user entering an already existing packet id
        :param id:
        :return: True/False
        '''
        current_list = self.__session_model.get_packet_list()
        if id not in current_list:
            return True
        else:
            print("!!! Packet ID {} is already being used in this Session.  Select another ID.".format(id))
            tkMessageBox.showwarning("Duplicate Packet ID", "Packet ID {} already taken in this Session.  Select another value.".format(id))
            return False


    def __create_file_header_dict(self):
        """
        Get file header from .ini file and return dictionary.
        """
        d = collections.OrderedDict()
        d['Duration']  = [self.duration, 'Duration in seconds only.']
        d['RunPeriod'] = [self.tlm_rate, 'Telemetry Collector rate or Sample Period in Hz.']
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

    def update_reserve_bit_display(self, bits):
        self.__view.set_reserve_bit_display(bits)

    def add_reserve(self):
        bits = self.__model.reserve_var.get()
        if bits == '':
            tkMessageBox.showwarning("Warning", "Enter the size of the 'Reverve' in the 'Bits: entry field.")
            print("Enter the size of the 'Reverve' in the 'Bits: entry field")
            return

        reserve_obj = FillerEntry(bits, self.__session_model.reserve_name, self.__session_model.reserve_desc)
        self.__generic_add(reserve_obj)


    def check_reserve(self):
        '''
        When opening a packet
        '''
        bits = self.__model.reserve_var.get()
        if bits == '':
            self.__view.reserve_button_state("disabled")
            self.__view.set_reserve_bit_display(0)
        else:
            self.__view.reserve_button_state("normal")
            bits = self.__model.get_packet_space()
            self.__view.set_reserve_bit_display(str(bits))


    def add_align(self, desc, bits):
        desc = self.__session_model.align_desc.format(desc)
        item_obj = FillerEntry(bits, self.__session_model.align_name, desc)

        reserve_tweak = self.clear_reserve()   # if a 'Reserve item is present delete it to allow space
        self.__generic_add(item_obj)
        self.__view.reset_reserve()    # set new reserve value

    def clear_reserve(self):
        '''
        This is used when the item list is modified.
        By deleting the reserve we clear all available space before
        adding, updating, byte aligning, word aligning, or deleting
        The reserve can them be added with a simple method call.
        :return:
        '''
        reserve_tweak = False
        if self.__view.reserve_present():
            self.__view.delete_reserve()
            reserve_tweak = True
        return reserve_tweak


    def add_item(self, update):
            """
            Gather data from editor entries and create a
            channel entry.
            """
            # get the name of the selected item to add
            ch_name = self.__session_model.selected_channel_var.get()

            if update.lower() == 'update':
                update = True
                # This has to be saved before the Reserve is deleted and passed to the update_item() method
                size, desc = self.__model.get_size_desc()
            else:
                update = False

            # if a 'Reserve item is present delete it to allow space
            reserve_tweak = self.clear_reserve()

            if update:
                self.__model.update_item(ch_name, size=size, desc=desc, reserve_tweak=reserve_tweak)
                self.__view.refresh_item_list(jump=False)
            else:
                ch_obj  = self.__session_model.get_channel_dict()[ch_name]
                item_entry = ItemEntry(ch_obj=ch_obj)
                self.__generic_add(item_entry)
            #set new reserve value
            self.__view.reset_reserve()

    def __generic_add(self, item_obj):
        """
        Add any item_obj and refresh item list
        """
        try:
            self.__model.add_item(item_obj)
            self.__view.refresh_item_list(jump=True)
        except AttributeError, e:
            print("!!! WARNING: Cannot complete 'Add'\n \
        Perhaps, not enough space left in the packet, check 'Bits Left' in upper right corner of the Gui, or \n \
        Maybe no packet associated with the tlm items. If it's a new packet be sure you clicked 'Save Packet' before selecting channels?")
            print(e)



    def comp_selected(self, comp_name):
        """
        Filter channel names via component.
        """
        if comp_name == "ALL":
            self.__view.set_channel_dd_items(sorted(self.__session_model.get_channel_names()))
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

    def get_offset_list(self):
        '''

        :return: offset list based on rate and frequecy
        '''
        return self.__model._get_offset_list()

    def get_frequency_list(self):
        '''
        :return: frequency list
        '''
        return self.__session_model.get_freq_list()

    def new_packet(self):
        self.__session_controller.new_packet()
        self.__view.hide_save_packet_button(False)


    def delete_packet(self):
        try:
            packet_id = self.__model.get_packet_id()
            self.__session_controller.delete_packet(packet_id)
        except Exception, e:
            print('No Packets in Session')
            tkMessageBox.showinfo("No Packets","There are no packets in this Session.")

    def save_packet(self, update):
        self.__session_controller.save_model(self.__model, update)
        self.__view.hide_save_packet_button(True)

    def get_packet_list(self):
        return sorted(self.__session_model.get_packet_list())

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


    def packet_list_drag(self, startRow, newRow):
        """
        Calls move_selected_row() to implement drag and drop in a packet item list
        Call method to update the packet model with the information from the view
        :param startRow: index to the item being dragged
        :param newRow:   index to the insertion point
        """
        try:
            self.__model.move_selected_row(startRow, newRow)
            self.__view.refresh_item_list(jump=False)
        except Exception, e:
            print("Exception {}: from 'packet_list_drag()' in tpe_controllers.py".format(e))


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
        try:
            ch_name     = self.__session_model.selected_channel_var.get()
            idx, item_obj    = self.__model.get_item_from_list(ch_name=ch_name)
            # If 'Reserve' is deleted, re-enable the button
            no_reserve=False
            # in this case we want to delete the Reserve item, so we do not want to reset it
            if 'Reserve' in ch_name[:7]:
                self.__view.reserve_button_state("normal")
                no_reserve=True

            # if a 'Reserve item is present delete it to allow space
            if not no_reserve:
                reserve_tweak = self.clear_reserve()

            size = int(item_obj.get_bits())
            self.__model.delete_item(idx)
            self.__model.update_space_data(-size, load=False, delete=True, pkt_id='')
            self.__view.refresh_item_list(jump=False)

            if not no_reserve:
                # set new reserve value
                self.__view.reset_reserve()

        except TypeError, e:
            print("Exception {}: from 'delete_item()' in tpe_controllers.py".format(e))

    def reinitialize_session(self):
        self.__view.reinitialize_session()

    def reset_space_data(self):
        print("In controller")
        self.__model.reset_space_data()

    def update_idletasks(self):
        self.__session_controller.update_idletasks()

    def load_model(self, model):
        self.__model = model

    def load_view(self, view):
        self.__view = view
