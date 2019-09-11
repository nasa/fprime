'''
Created on Dec 9, 2014

@author: reder
'''

import time
import Tkinter
import Pmw
from tkintertable.Tables import TableCanvas
from tkintertable.TableModels import TableModel

from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.controllers import channel_loader
from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import observer
import telemetry_filter_panel


class TelemetryPanel(observer.Observer):
    '''
    This panel is a table of currently observed
    telemetry channels.
    '''

    def __init__(self, parent, top, opt):
        '''
        Constructor
        '''
        self.__parent = parent
        self.__top = top
        self.__opt = opt

        config = ConfigManager.ConfigManager.getInstance()

        # Period to delay between table refreshes
        self.__update_period = config.get('performance', 'telem_table_update_period')

        #
        # Status updater singleton
        #
        self.__status_update = status_updater.StatusUpdater.getInstance()
        #
        # Instance the channel loader here and get all channel names
        #
        self.__channel_loader = channel_loader.ChannelLoader.getInstance()
        self.__channel_names_list = self.__channel_loader.getNameDict().values()
        self.__channel_names_dict = self.__channel_loader.getNameDict()
        num_channels = len(self.__channel_loader.getNameDict())
        #
        # Container Frame
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)
        #
        # Create a model and connect it to a table on a canvas
        #
        self.__model = TableModel()
        self.__table = TableCanvas(f, model=self.__model, width=1200, height=800)
        # Set font
        font = config.get('tables', 'font')
        font_size = config.get('tables', 'font_size')
        self.__table.thefont = (font, font_size)
        self.__table.rowheight= int(font_size) + 5
        self.__table.createTableFrame()
        self.__table.redrawTable()

        # Mouse movement causes flyover text. This is not needed.
        # Unbind the appropriate event handler
        self.__table.unbind('<Motion>')

        #
        # Init. the table with a standard set of column headers
        #
        data = {'1': {'Channel':None}}
        self.__model.importDict(data) # can import from a dictionary to populate model
        self.__table.addColumn('Id')
        self.__table.addColumn('Time')
        self.__table.addColumn('Value')
        for i in range(2,num_channels+1):
            self.__table.addRow("%s" % i)
        self.__table.align = 'w'
        self.__table.redrawTable()
        self.__table.maxcellwidth = 500
        # Remove Row and cell highlight color
        self.__table.rowselectedcolor  = None
        self.__table.selectedcolor = None
        # Rebind <B1-Motion> event to catch exception from column resize error
        self.__table.tablecolheader.unbind('<B1-Motion')
        self.__table.tablecolheader.bind('<B1-Motion>', self.__handle_column_motion)
        self.__table.tablecolheader.unbind('<ButtonRelease-1>')
        self.__table.tablecolheader.bind('<ButtonRelease-1>', self.__handle_column_release)

        # Make table read only
        self.__table.editable = False

        #
        # Pop-up Channel Telemetry Filter Selection and Active Filter Status.
        #
        f2 = Tkinter.LabelFrame(f, text="Channel Telemetry Filtering:", padx=5, pady=5)
        f2.grid(row=2, column=0, columnspan=4, sticky=Tkinter.W+Tkinter.E)
        #
        b1=Tkinter.Button(f2, text="Select Channels", command=self.__select)
        b1.pack(side=Tkinter.LEFT)
        # Hex value toggle
        self.__print_hex = Tkinter.IntVar()
        self.__print_hex.set(0)
        self.__hex_cb = Tkinter.Checkbutton(f2, text="Show Hex", variable=self.__print_hex, pady=5)
        self.__hex_cb.pack(side=Tkinter.RIGHT)
        #
        self.__e1=Pmw.ScrolledField(f2, labelpos=Tkinter.W, labelmargin=0, text="None")
        self.__e1.pack(side=Tkinter.LEFT, expand=1, fill=Tkinter.X)
        self.__e1.component('entry').config(width=60)
        self.__e1.component('label').config(text='Active Filter Selected:')

        #
        # This is a channel id key to row value dictionary
        # built up over runtime to update table.
        #
        self.__row_dict = dict()
        self.__row_max  = 1
        #
        # List of active channels to display
        self.__channel_names_list.sort()
        self.__channels_active_list = self.__channel_names_list
        self.__inactive_channels = []

    def __handle_column_motion(self, event):
        """
        Used to rebind the table's ColumnHeader <B1-Motion> event.
        """
        try:
            self.__table.tablecolheader.handle_mouse_drag(event)
        except ValueError, e:
            pass
    def __handle_column_release(self, event):
        try:
            self.__table.tablecolheader.handle_left_release(event)
        except Exception, e:
            pass


    def __select(self):
        """
        Dialog for setting up filtered telemetry channels.
        """
        ch = telemetry_filter_panel.telemetryFilterDialog(self.__parent, self.__inactive_channels, self.__channels_active_list)
        if ch[0] == None: # if cancelled, keep current state
            return

        self.__e1.configure(text=ch[0])
        self.__channels_active_list = ch[1]
        self.__inactive_channels   = ch[3]

        if ch[0] != "None":
            self.clearTelmItems()
            self.initTelmItems()
            self.refresh()


    # copied from tkintertable since scale seems to be off
    def adjustColumnWidths(self):
        """
        Optimally adjust col widths to accomodate the longest entry
        in each column - usually only called  on first redraw
        """
        #self.cols = self.model.getColumnCount()
        try:
            fontsize = self.__table.thefont[1]
        except:
            fontsize = self.__table.fontsize
        scale = 10.5 * float(fontsize)/12
        for col in range(self.__table.cols):
            colname = self.__table.model.getColumnName(col)
            if self.__table.model.columnwidths.has_key(colname):
                w = self.__table.model.columnwidths[colname]
            else:
                w = self.__table.cellwidth
            maxlen = self.__table.model.getlongestEntry(col)
            size = maxlen * scale
            if size < w:
                continue

            if size >= self.__table.maxcellwidth:
                size = self.__table.maxcellwidth
            self.__table.model.columnwidths[colname] = size + float(fontsize)/12*6
        return


    def refresh(self):
        if self.__table.winfo_exists():
            self.adjustColumnWidths()
            self.__table.redrawTable()

        self.__after_id = self.__top.root().after(self.__update_period, self.refresh)

    def refresh_cancel(self):
        self.__top.root().after_cancel(self.__after_id)


    def setTelmItem(self, row, channel, id, time, value, limit):
        """
        Set all parts of a telemetry channel.
        """
        row = "%s" % row
        self.__table.model.data[row]['Channel'] = channel
        self.__table.model.data[row]['Id']      = id
        self.__table.model.data[row]['Time']    = time
        self.__table.model.data[row]['Value']   = value

        if not self.__table.model.colors['bg'].has_key(row):
            self.__table.model.colors['bg'][row] = {}
        self.__table.model.colors['bg'][row]['Value'] = str(limit)


    def clearTelmItems(self):
        """
        For all channels clear the items.
        """
        for row in range(1,len(self.__channel_names_list)+1):
            self.setTelmItem(row, "", "", "", "",self.__table.cellbackgr)
        self.__row_dict = dict()
        self.__row_max  = 1


    def initTelmItems(self):
        """
        For display telemetry channels name, id
        and set up id to row dictionary hear.
        """
        for name in self.__channels_active_list:
            # dict of id key to name values
            ch = self.__channel_names_dict
            # convert to name key to id values
            ch_id_list = [(v, k) for k, v in ch.iteritems()]
            ch_id_dict = dict()
            for t in ch_id_list:
                ch_id_dict[t[0]] = t[1]
        #
        row = 1
        self.__row_dict = dict()
        for name in self.__channels_active_list:
            id = ch_id_dict[name]
            self.__row_dict[id] = row
            self.setTelmItem(row, name, id, "", "",self.__table.cellbackgr)
            row += 1

    def update(self, observable, arg):
        """
        Update Channel Telemetry here.
        """
        msg = observable.getCurrentChannelTelemetryItem()
        if msg != None:
            (name, id, ch, (tb, tc, ts, tus), val, format_string, low_red, low_orange, low_yellow, high_yellow, high_orange, high_red) = msg
            if name in self.__channels_active_list:
                if id in self.__row_dict:
                    row = self.__row_dict[id]
                else:
                    row = self.__row_max
                    self.__row_dict[id] = row
                    self.__row_max += 1
                # See if set to workstation time
                if tb == 2:
                    if self.__opt.log_time == "local":
                        t = time.strftime("%m/%d-%H:%M:%S",time.localtime(ts))
                    else:
                        t = time.strftime("%m/%d-%H:%M:%S",time.gmtime(ts))

                else:
                    t = "%f" % (float(ts) + float(tus)/1000000.0)
                valString = ""
                if (format_string == None):
                    valString = str(val)
                else:
                    valString = format_string%val

                limit = self.__table.cellbackgr # default color
                # check limits and hex value if numeric
                if type(val) == type(int()) or type(val) == type(float()):
                    if self.__print_hex.get() == 1:
                        valString += " ({})".format(hex(val))

                    red_limit = '#F69898' # HTML color picker, light red
                    yellow_limit = 'Yellow'
                    orange_limit = '#FFA533' # HTML color picker, light orange
                    if low_red != None:
                        if val <= float(low_red):
                            limit = red_limit
                    if high_red != None:
                        if val >= float(high_red):
                            limit = red_limit
                    if limit != red_limit:
                        if low_orange != None:
                            if val <= float(low_orange):
                                limit = orange_limit
                        if high_orange != None:
                            if val >= float(high_orange):
                                limit = orange_limit
                        if limit != orange_limit:
                            if low_yellow != None:
                                if val <= float(low_yellow):
                                    limit = yellow_limit
                            if high_yellow != None:
                                if val >= float(high_yellow):
                                    limit = yellow_limit

                self.setTelmItem(row, name, id, t, valString, limit)


if __name__ == "__main__":
    pass
