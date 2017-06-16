'''
Created on Dec 9, 2014

@author: reder
'''
import time
from enum import Enum
import Tkinter, tkFileDialog
import Pmw

from tkintertable.Tables import TableCanvas
from tkintertable.TableModels import TableModel
from tkintertable.Filtering import FilterFrame

from models.common.event import Severity
from controllers import status_updater
from utils import ConfigManager
from utils import gse_misc
from utils import gse_persist
from _sqlite3 import Row


@gse_persist.PanelMementoOriginator
class LogEventPanel(object):
    '''
    A class that receives log event messages and displays them
    in a scrolled list.  Within the model log messages are
    saved to a default file automatically.  Can save select
    set of log messages to file for post-processing.
    '''
    def __init__(self, parent, top, opt):
        '''
        Constructor
        '''
        self._top = top
        self._parent = parent
        self.__opt = opt

        self.__config = ConfigManager.ConfigManager.getInstance()

        #
        # Status updater singleton
        #
        self.__status_update = status_updater.StatusUpdater.getInstance()
        #
        ## Container Frame
        #
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)


        #
        ## Filter updates to listbox view this entry and refresh.
        #
        f2 = Tkinter.LabelFrame(f, text="Display Only Log Event Messages Containing Keywords", padx=5, pady=5)
        f2.pack(side=Tkinter.BOTTOM, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=0, padx=5, pady=5)
        #
        self.__e1=Pmw.EntryField(f2, validate=None)
        self.__e1.pack(side=Tkinter.LEFT, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)

        #
        ## Selection for severity filter
        #
        self.__selected_severity = None
        self.__severity_select = Pmw.ComboBox(f2, label_text='Severity: ', labelpos='wn',
                                entry_width=4, dropdown=1,
                                selectioncommand=self.__filter_severity,
                                scrolledlist_items= ["ALL"]+[item.name for item in Severity])
        self.__severity_select.pack(side=Tkinter.LEFT, anchor=Tkinter.N, fill=Tkinter.X, expand=1, padx=8, pady=8)
        self.__severity_select.selectitem(0, setentry = 1)
        # Make sure up down keys don't cause an exception when used in combo box
        gse_misc.rebind_comboBox(self.__severity_select)

        #
        ## Filter Controls
        #
        self.__filter_reset_button = Tkinter.Button(f2, text="Reset", command=self.__filter_reset)
        self.__filter_reset_button.pack(side=Tkinter.RIGHT)
        #
        self.__apply_button = Tkinter.Button(f2, text="Apply Filter", command=self.__filter_apply)
        self.__apply_button.pack(side=Tkinter.RIGHT)
        self.__filterframe = None
        #
        #self.test()

        f3 = Tkinter.Frame(f, relief=Tkinter.FLAT, borderwidth=1, padx=1, pady=1)
        f3.pack(side=Tkinter.BOTTOM, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1,padx=6,pady=3)

        self.__clear_button = Tkinter.Button(f3, text="Clear", command=self.__clear_log)
        self.__clear_button.pack(side=Tkinter.RIGHT, anchor=Tkinter.S)
        #
        self.__scroll = Tkinter.IntVar()
        self.__scroll_cb = Tkinter.Checkbutton(f3, text="Scroll", variable=self.__scroll, pady=5)
        self.__scroll_cb.pack(side=Tkinter.BOTTOM, anchor=Tkinter.E)
        self.__scroll_cb.select()


        #
        ## Build immediate command entry area here
        #
        f1 = Tkinter.LabelFrame(f,text="Event Log Messages", relief=Tkinter.GROOVE, borderwidth=2, padx=5, pady=5)
        f1.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1,padx=10,pady=10)

        #
        ## Create a model and connect it to a table on a canvas
        #
        self.__model = TableModel()
        self.__table = TableCanvas(f1, model=self.__model, width=1200, height=800)
        # Set font sizes
        font = self.__config.get('tables', 'font')
        font_size = self.__config.get('tables', 'font_size')
        self.__table.thefont = (font, font_size)
        self.__table.rowheight= int(font_size) + 5
        #
        self.__table.createTableFrame()
        self.__table.align = 'w'
        # Remove Row and cell highlight color
        self.__table.rowselectedcolor  = None
        self.__table.selectedcolor = None
        self.__table.maxcellwidth = 500
        # Mouse movement causes flyover text. This is not needed.
        # Unbind the appropriate event handler
        self.__table.unbind('<Motion>')
         # Rebind <B1-Motion> event to catch exception from column resize error
        self.__table.tablecolheader.unbind('<B1-Motion')
        self.__table.tablecolheader.bind('<B1-Motion>', self.__handle_column_motion)
        self.__table.tablecolheader.unbind('<ButtonRelease-1>')
        self.__table.tablecolheader.bind('<ButtonRelease-1>', self.__handle_column_release)

        #
        # Add columns
        self.__column_list = ('TIME', 'NAME', 'ID', 'SEVERITY', 'MESSAGE')
        for column in self.__column_list:
            self.__table.addColumn(column)
        self.__table.redrawTable()

        #
        # Colors for severity
        self.__severity_color = dict(self.__config.items('severity_colors'))
        #
        # Data container for filtering
        self.__all_table_data = None
        self.__filtered_table_data = None

        self.__table_row = 0
        #

        self.__placeholder_entry = True     # First entry is a place holder. Overwritten by first real entry.
        self.insertLogMsg(("","","","","")) # Insert placeholder event to initalize table
        self.__table.redrawTable()
        top.root().update()


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


    def test(self):
        """
        Insert hardwired log event strings for testing filter.
        """
        e1 = "SomeEvent1 (100) Severity.INFO: My Event 1 2.3 4.5 6.7 8.9 10.11"
        e2 = "SomeEvent2 (101) Severity.INFO: My Event 1 2.0 3"
        e3 = "SomeStringEvent (102) Severity.INFO: My Event 1 Two is a string 255"
        e4 = "SomeEnumEvent (103) Severity.INFO: My Event 1 MEMB2 3"
        e5 = "SomeBoolEvent (104) Severity.INFO: My Event 1 True 255"
        e6 = "SomeEnumEvent (103) Severity.INFO: My Event 2 MEMB3 4"
        e7 = "SomeBoolEvent (104) Severity.INFO: My Event 2 False 255"
        e8 = "SomeEnumEvent (103) Severity.INFO: My Event 3 MEMB1 255"
        e9 = "SomeEvent2 (101) Severity.INFO: My Event 4 5.0 6"
        e10 = "SomeStringEvent (102) Severity.INFO: My Event 2 Three is a string 255"
        msgs = [e1,e2,e3,e4,e5,e6,e7,e8,e9,e10]
        for m in msgs:
            self.insertLogMsg(m)


    def __del__(self):
        """
        Destructor
        """
        pass

    # copied from tkintertable since scale seems to be off
    def adjustColumnWidths(self):
        """
        Optimally adjust col widths to accomodate the longest entry
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
            #print col, size, self.cellwidth
            if size >= self.__table.maxcellwidth:
                size = self.__table.maxcellwidth
            self.__table.model.columnwidths[colname] = size + float(fontsize)/12*6
        return

    def insertLogMsg(self, msg_obj):
        """
        Insert log message at top of table widget.
        """
        # Temporarily reset filter so we can add entry
        self.__table.showAll()

        row = "%s" % self.__table_row
        # Skip row creation so first real entry can overwrite placeholder
        if not self.__placeholder_entry and row == '0':
            pass
        else:
            self.__table.addRow(row)

        for idx, evr_data in enumerate(msg_obj):
            # print "idx = %s, evr_data = %s" % (idx, evr_data)
            # Handle Severity
            if isinstance(evr_data, Severity):
                self.__table.model.data[row][self.__column_list[idx]] = evr_data.name
                color = self.__severity_color[evr_data.name.lower()]
                self.__table.model.setColorAt(self.__table_row, idx, color, key='bg')
            else:
                self.__table.model.data[row][self.__column_list[idx]] = evr_data

        # Reapply filter
        self.__filter_apply()

        if self.__scroll:
            # Having difficulty making auto scroll work
            # properly. Currently scrolls to bottom. But scroll
            # bar is not updated.
            self.__table.yview_scroll(1, "pages")

            # This should move the scrollbar!
            # Something might be happening in the TkinterTable TableCanvas class.
            # See: https://github.com/dmnfarrell/tkintertable/blob/master/tkintertable/Tables.py
            #
            self.__table.Yscrollbar.set(1.0, 1.0)


        self.adjustColumnWidths()

        if self.__placeholder_entry:
            self.__placeholder_entry = False
        else:
            self.__table_row += 1


    def selectionCommand(self):
        sels = self.__list_box.getcurselection()
#         if len(sels) == 0:
#             print 'No selection'
#         else:
#             print 'Selection:', sels[0]


    def __clear_log(self):
        """
        Clear all Log Event Messages
        """
        self.__table.select_All()
        self.__table.clearData()

        self.__table.model.deleteRows()

        self.__table.redrawTable()
        self.__placeholder_entry = True
        self.__table_row = 0


    def __filter_severity(self, entry):
        self.__selected_severity = entry

    def __filter_apply(self):
        do_filter   = False
        filter_idx  = 0
        self.__filterframe = FilterFrame(None, self.__table.model.columnNames, self.__table.doFilter, self.__table.closeFilterFrame)


        # Severity Filter
        if self.__selected_severity and self.__selected_severity != "ALL":
            # Get filter
            f = self.__filterframe.filters[filter_idx]
            # Set filter properties
            f.filtercol.set('SEVERITY')
            f.operator.set('contains')
            f.booleanop.set("AND")
            f.filtercolvalue.set(self.__selected_severity)
            #print f.getFilter()
            filter_idx += 1
            do_filter = True


        # Keyword filter
        if self.__e1.get():
            if filter_idx != 0: # Then we added one previously and need a new one
                self.__filterframe.addFilterBar()

            # Add filters for all columns
            for col in self.__column_list:
                if col == 'SEVERITY': # Skip...we have already filtered for this
                    continue

                f = self.__filterframe.filters[filter_idx]
                f.filtercol.set(col)
                f.operator.set('contains')
                f.booleanop.set("OR")
                f.filtercolvalue.set(self.__e1.get())

                filter_idx += 1
                # Add if not last column
                if filter_idx != len(self.__column_list):
                    self.__filterframe.addFilterBar()

            do_filter = True


        if do_filter:
            names = self.__filterframe.doFiltering(searchfunc=self.__table.model.filterBy)

            # Show filtered entries
            self.__table.model.filteredrecs = names
            self.__table.filtered = True
            self.__table.redrawTable()


    def __filter_reset(self):
        if self.__filterframe:
            self.__table.showAll()
            self.__filterframe.destroy()

        # Reset entry fields
        self.__e1.clear()
        self.__severity_select.selectitem(0, setentry=1)
        self.__filter_severity(self.__severity_select.get(0))

    def update(self, e):
        """
        Update the panel list widget with new log events
        @param e: this is set by observer to the LogEventListener.
        """
        msg_tup = e.getCurrentEventLogMsg()
        #print msg_tup
        if msg_tup:
            self.insertLogMsg(msg_tup[1])
            # LJR added fix 4 May 2017
            self.__table.redrawTable()
        #print "update test", e, self._top

    def set_memento(self, m):
        """
        Get saved state and restore components.
        """
        state = m.getState()
        severity = state['selected_severity']
        self.__selected_severity = severity
        self.__severity_select.component('entry').setentry(severity)
        self.__filter_apply()

    def create_memento(self):
        """
        Must return (memento, wid, panel_name).
        """
        wid = self._top.id()
        panel_name = self.get_name()
        state = {'selected_severity':self.__selected_severity}
        m = gse_persist.Memento(state)

        return m, wid, panel_name


    def get_name(self):
     return "log_event_panel"



def main_window_start():
    """
    """
    root = Tkinter.Tk()
    #root.option_readfile('optionDB')
    root.title('F Prime Command Panel')
    Pmw.initialise()
    c=LogEventPanel(root)
    for i in range(100):
        c.insertLogMsg("LOG_MSG%2d" % i)
    root.update()
    return root

if __name__ == "__main__":
    root = main_window_start()

    root.mainloop()
