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

from fprime_gds.common.models.common.event import Severity

from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import observer

from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.utils import tkGui_misc
from fprime_gds.tkgui.utils import tkGui_persist

class LogEventPanel(observer.Observer):
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
        super(LogEventPanel, self).__init__()

        self.__top = top
        self.__parent = parent
        self.__opt = opt

        # Get periodicity of event panel refresh
        config = ConfigManager.ConfigManager.getInstance()
        self.__update_period = config.get('performance', 'event_table_update_period')

        #
        # Status updater singleton
        #
        self.__status_update = status_updater.StatusUpdater.getInstance()
        #
        ## Container Frame
        #
        f = Tkinter.Frame(parent)
        f.pack(side=Tkinter.TOP, anchor=Tkinter.N, fill=Tkinter.BOTH, expand=1)

        # Id for UI refresh loop
        self.__after_id = None

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
        tkGui_misc.rebind_comboBox(self.__severity_select)

        #
        ## Filter Controls
        #
        self.__filter_reset_button = Tkinter.Button(f2, text="Reset", command=self.__filter_reset)
        self.__filter_reset_button.pack(side=Tkinter.RIGHT)
        #
        self.__apply_button = Tkinter.Button(f2, text="Apply Filter", command=self.__filter_apply)
        self.__apply_button.pack(side=Tkinter.RIGHT)
        self.__filterframe = None

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
        ## Canvas table
        #
        self.__column_list = ['TIME', 'NAME', 'ID', 'SEVERITY', 'MESSAGE']
        self.__table_row_max = 100

        self.__table = TableCanvas(
          f1,
          rows= self.__table_row_max,
          cols= len(self.__column_list),
          width=1200,
          height=800
        )
        # Set font sizes
        font = config.get('tables', 'font')
        font_size = config.get('tables', 'font_size')
        self.__table.thefont = (font, font_size)
        self.__table.rowheight= int(font_size) + 5
        #
        self.__table.createTableFrame()
        self.__table.align = 'w'
        # Remove Row and cell highlight color
        self.__table.rowselectedcolor  = None
        self.__table.selectedcolor = None
        self.__table.maxcellwidth = 1000
        # Mouse movement causes flyover text. This is not needed.
        # Unbind the appropriate event handler
        self.__table.unbind('<Motion>')
         # Rebind <B1-Motion> event to catch exception from column resize error
        self.__table.tablecolheader.unbind('<B1-Motion')
        self.__table.tablecolheader.bind('<B1-Motion>', self.__handle_column_motion)
        self.__table.tablecolheader.unbind('<ButtonRelease-1>')
        self.__table.tablecolheader.bind('<ButtonRelease-1>', self.__handle_column_release)

        # Make table read only
        self.__table.editable = False

        # Update column names
        for idx, column in enumerate(self.__column_list):
          self.__table.model.relabel_Column(idx, column)
        self.__table.redrawTable()

        # Colors for severity
        self.__severity_color = dict(config.items('severity_colors'))
        #
        # Data container for filtering
        self.__all_table_data = None
        self.__filtered_table_data = None

        # Current row index
        self.__next_row = 0

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
        for idx, evr_data in enumerate(msg_obj):
            # Handle Severity
            if isinstance(evr_data, Severity):
                self.__table.model.setValueAt(evr_data.name, self.__next_row, idx)
                color = self.__severity_color[evr_data.name.lower()]
                self.__table.model.setColorAt(self.__next_row, idx, color, key='bg')
            else:
                self.__table.model.setValueAt(evr_data, self.__next_row, idx)

        self.__next_row += 1
        # Check row bounds and increase if close to end
        if self.__next_row + 1 >= (self.__table_row_max-25):
            self.__table.model.autoAddRows(100)
            self.__table_row_max += 100


    def __clear_log(self):
        """
        Clear all Log Event Messages
        """
        self.__table.select_All()
        self.__table.clearData()

        self.__table_row_max = 100
        self.__next_row = 0

        self.__table.model.deleteRows()
        self.__table.model.autoAddRows(100)

        self.__table.redrawTable()


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

    def update(self, observable, arg):
        """
        Update the panel list widget with new log events
        @param e: this is set by observable to the LogEventListener.
        """
        msg_tup = observable.getCurrentEventLogMsg()
        if msg_tup:
            self.insertLogMsg(msg_tup[1])

    def refresh(self):

      # Check scroll selection
      if self.__scroll.get() == 1:
        self.__table.setSelectedRow(self.__next_row-1)
        last_visible_row = self.__table.visiblerows[-1]-4

        # If close to end of visible rows move table to current row
        if self.__next_row > last_visible_row:
          fraction = float(self.__next_row-5)/float(self.__table_row_max)
          self.__table.set_yviews('moveto', fraction)

      # Refresh the table
      self.__table.redrawTable()
      self.adjustColumnWidths()

      # Rerun after delay period
      self.__after_id = self.__top.root().after(self.__update_period, self.refresh)

    def refresh_cancel(self):
        self.__top.root().after_cancel(self.__after_id)

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
