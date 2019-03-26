# TODO Documentation
from __future__ import absolute_import

import wx
from . import GDSLogEventPanelGUI
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.utils.event_severity import EventSeverity
from fprime_gds.common.utils.config_manager import ConfigManager

###########################################################################
## Class LogEventsImpl
###########################################################################

class LogEventsImpl (GDSLogEventPanelGUI.LogEvents):
    '''Implementation class for LogEvents panel. Defines funcitonality.'''


    def __init__( self, parent, config=None ):
        """LogEventsImple constructor

        Arguments:
            parent {wx.Window} -- The parrent for this GUI element
        """

        GDSLogEventPanelGUI.LogEvents.__init__ ( self, parent)
        self.parent = parent

        self.dv_model = EventLogDataViewModel([])

        self.EventLogDataListCtl.AssociateModel(self.dv_model)

        self.EventLogDataListCtl.AppendTextColumn( "Time" , 0, width=150)
        self.EventLogDataListCtl.AppendTextColumn( "Name",1,  width=150 )
        self.EventLogDataListCtl.AppendTextColumn( "ID" ,2)
        self.EventLogDataListCtl.AppendTextColumn( "Severity", 3, width=110)
        self.EventLogDataListCtl.AppendTextColumn( u"Message" ,4)
        self.EventLogSeverityComboBox.Append('')
        for i in EventSeverity:
            self.EventLogSeverityComboBox.Append(i.name)

        self.EventLogSeverityComboBox.SetSelection(0)

        self.EventLogDataListCtl.Bind(wx.EVT_KEY_DOWN, self.onCopyKeyPressed)

        self.scrollEventLogToBottom()
    
    def __del__( self ):
        self.dv_model.DecRef()

    def data_callback(self, data):
        """Recieves data from decoders to which this consumer is registered

        Arguments:
            data {Data Object} -- A Data Object containing the data passed from the decoder (e.g., an EventData object)
        """
        if self.dv_model.RefCount > 1:
            # Use CallAfter to avoid race condition
            wx.CallAfter(self.dv_model.UpdateModel,data)

    def scrollEventLogToBottom(self):
        """Move the event log scroll bar so that the last entry is visible. Called repeatedly when the "scroll" box is checked"
        """

        l = self.dv_model.getDataLen()
        if self.EventLogScrollCheckBox.GetValue() == True and l > 0:
            if self.dv_model.current_filter != (None, None):
                i = self.dv_model.ObjectToItem(self.dv_model.data_filtered[int(l - 1)])
            else:
                i = self.dv_model.ObjectToItem(self.dv_model.data[int(l - 1)])

            self.EventLogDataListCtl.EnsureVisible(i)
            self.EventLogDataListCtl.Refresh()
        wx.CallLater(10, self.scrollEventLogToBottom)

    def getEventLogState(self):
        """Get the internal data list used by the model to populate the data view for telem panel

        Returns:
            list -- list of ChData or PktData objects
        """

        return self.dv_model.data

    def setEventLogState(self, state):
        """Set the internal data list used by the model to populate the data view for telem

        Arguments:
            data {list} -- list of ChData or PktData objects
        """

        for r in state:
            self.dv_model.UpdateModel(r)

    def onCopyKeyPressed(self, event):
        """Callback for key pressed within the data view control
        """

        # Ctrl-C pressed
        if event.ControlDown() and event.GetKeyCode() == 67:
            rows = self.EventLogDataListCtl.GetSelections()
            cpy_out = ""
            for r in rows:
                o = self.dv_model.ItemToObject(r)
                cpy_out += o.get_str(verbose=True, csv=True) + '\n'

            clipboard = wx.TextDataObject()
            # Set data object value
            clipboard.SetText(cpy_out)
            # Put the data in the clipboard
            if wx.TheClipboard.Open():
                wx.TheClipboard.SetData(clipboard)
                wx.TheClipboard.Close()
        event.Skip()

    def onCopyKeyPressedContext(self, event):
        """Called when the copy option is selected from the context menu within the data view ctrl
        
        Arguments:
            event {wx.Event} -- std event arg
        """

        rows = self.EventLogDataListCtl.GetSelections()
        cpy_out = ""
        for r in rows:
            o = self.dv_model.ItemToObject(r)
            cpy_out += o.get_str(verbose=True, csv=True) + '\n'

        clipboard = wx.TextDataObject()
        # Set data object value
        clipboard.SetText(cpy_out)
        # Put the data in the clipboard
        if wx.TheClipboard.Open():
            wx.TheClipboard.SetData(clipboard)
            wx.TheClipboard.Close()
            
    # Override these handlers to implement functionality for GUI elements
    def onLogEventDataViewContextMenu( self, event ):

        # Allows copying of data form the data view through right click context menu
        if not hasattr(self, 'copy_context_id'):
            self.copy_context_id = wx.NewId()
            self.Bind(wx.EVT_MENU, self.onCopyKeyPressedContext, id=self.copy_context_id)

        menu = wx.Menu()
        cpy = menu.Append(self.copy_context_id, "copy")

        self.PopupMenu(menu)
        menu.Destroy()
        event.Skip()

    def onEventLogClearButtonClick( self, event ):
        self.dv_model.DeleteAllItems()

    def onEventLogApplyFilterButtonClick( self, event ):
        search_term = self.EventLogSeachKeywordTextCtl.GetLineText(0)
        if search_term == u'':
            search_term = None
        try:
            severity = EventSeverity[self.EventLogSeverityComboBox.GetStringSelection()]
        except KeyError:
            severity = None

        self.dv_model.ApplyFilter(search_term, severity)

    def onEventLogResetFilterButtonClick( self, event ):
        self.dv_model.ApplyFilter(None, None)

    def onEventLogDataListCtrlScroll(self, event):
        self.EventLogScrollCheckBox.Value = False
        event.Skip()


class EventLogDataViewModel(wx.dataview.PyDataViewModel):
    """This class acts as an intermediary between user data and the actual data
    view display. It stores data and maintains a mapping from data to items in
    the data view. Most of the methdos in this class just need to be defined
    and are called automatically by the data view.
    """

    def __init__(self, data, config=None):
        '''
        Constructor

        Args:
            data: TODO
            config (ConfigManager, default=None): object with all config info
                   for colors. If None, defaults used
        '''

        if config==None:
            config = ConfigManager()

        self.config = config

        # Colors in config object are Hex codes stored as strings.
        #  Convert the string to an int, and then convert to a wxPython Colour
        warn_lo_color_val = int(self.config.get('colors', 'warning_lo'), 16)
        warn_hi_color_val = int(self.config.get('colors', 'warning_hi'), 16)
        fatal_color_val = int(self.config.get('colors', 'fatal'), 16)
        command_color_val = int(self.config.get('colors', 'command'), 16)

        self.warn_lo_color = wx.Colour(warn_lo_color_val)
        self.warn_hi_color = wx.Colour(warn_hi_color_val)
        self.fatal_color = wx.Colour(fatal_color_val)
        self.command_color = wx.Colour(command_color_val)


        wx.dataview.PyDataViewModel.__init__(self)
        self.data = data

        self.data_filtered = list()

        self.current_filter = (None, None)

        # The PyDataViewModel derives from both DataViewModel and from
        # DataViewItemObjectMapper, which has methods that help associate
        # data view items with Python objects. Normally a dictionary is used
        # so any Python object can be used as data nodes. If the data nodes
        # are weak-referencable then the objmapper can use a
        # WeakValueDictionary instead.
        self.UseWeakRefs(True)

    # Report how many columns this model provides data for.
    def GetColumnCount(self):
        """Get teh number of columns

        Returns:
            int -- the number of columns
        """

        return 5

    # Map the data column numbers to the data type
    def GetColumnType(self, col):
        """Get the data type associated with the given column

        Arguments:
            col {int} -- the column index of interest

        Returns:
            dict -- mapping from column index to type
        """

        mapper = { 0 : 'string',
                   1 : 'string',
                   2 : 'string',
                   3 : 'string',
                   3 : 'string',
                   }
        return mapper[col]

    def GetChildren(self, parent, children):
        """Return the children of a given parent

        Arguments:
            parent {Item} -- the parent to get children of
            children {List} -- list of the children Items

        Returns:
            int -- length of children
        """


        # The view calls this method to find the children of any node in the
        # control. There is an implicit hidden root node, and the top level
        # item(s) should be reported as children of this node. A List view
        # simply provides all items as children of this hidden root. A Tree
        # view adds additional items as children of the other items, as needed,
        # to provide the tree hierachy.

        # If the parent item is invalid then it represents the hidden root
        # item.

        if not parent:
            for obj in self.data:
                children.append(self.ObjectToItem(obj))
            return len(self.data)
        return 0

    def IsContainer(self, item):
        """Find out if the given item has children

        Arguments:
            item {Item} -- the item to test

        Returns:
            bool -- returns True if the argument has children
        """

        # Return True if the item has children, False otherwise.

        # The hidden root is a container
        if not item:
            return True
        return False

    def GetParent(self, item):
        """Get the parent of the given item

        Arguments:
            item {Item} -- input item

        Returns:
            Item -- the parent of the argument item
        """

        # Return the item which is this item's parent.
        return wx.dataview.NullDataViewItem

    def GetValue(self, item, col):
        """Return the value to be displayed for this item and column

        Arguments:
            item {Item} -- the item whose value we will get
            col {int} -- the column we will get the value from

        Raises:
            RuntimeError -- error if we get an object that we don't know how to handle

        Returns:
            dict -- mapping from column to the value for a given item
        """

        # Return the value to be displayed for this item and column. For this
        # example we'll just pull the values from the data objects we
        # associated with the items in GetChildren.

        # Fetch the data object for this item.
        node = self.ItemToObject(item)
        arg_vals = tuple([arg.val for arg in node.args])

        if isinstance(node, EventData):
            mapper = { 0 : str(node.time.to_readable()),
                       1 : str(node.template.get_full_name()),
                       2 : str(node.template.id),
                       3 : str(node.template.severity.name),
                       4 : str(node.template.format_str%arg_vals)
                       }
            return mapper[col]

        else:
            raise RuntimeError("unknown node type")

    def GetAttr(self, item, col, attr):
        """Get the attributes of the given item at the given column in the list control

        Arguments:
            item {Item} -- item object in question
            col {int} -- column number in question
            attr {attr} -- the attribute object to set

        Returns:
            bool -- True if attributes were set
        """

        node = self.ItemToObject(item)
        if isinstance(node, EventData) and col == 3:
            if node.template.severity == EventSeverity.FATAL:
                attr.SetColour(self.fatal_color)
                attr.SetBold(True)
            elif node.template.severity == EventSeverity.WARNING_HI:
                attr.SetColour(self.warn_hi_color)
                attr.SetBold(True)
            elif node.template.severity == EventSeverity.WARNING_LO:
                attr.SetColour(self.warn_lo_color)
                attr.SetBold(True)
            elif node.template.severity == EventSeverity.COMMAND:
                attr.SetColour(self.command_color)
                attr.SetBold(True)
            return True
        return False

    def UpdateModel(self, new_data):
        """Add a new data item to the event log.

        Arguments:
            new_data {EventData} -- the new event data to be added
        """

        self.data.append(new_data)

        st, sev = self.current_filter

        if st == None and sev == None:
            self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))
            self.data_filtered = list()
        elif st is not None and st in new_data.get_str():
            self.data_filtered.append(new_data)
            self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))
        elif new_data.template.severity == sev:
            self.data_filtered.append(new_data)
            self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))

    def ApplyFilter(self, search_term, severity):
        """Apply a filter to the event log display. Pass None and None to reset the filter

        Arguments:
            search_term {string} -- the term to search by
            severity {EventSeverity} -- severity to filter by
        """

        # Check if the last filter was None
        if self.current_filter == (None, None):
            last_filter_none = True
        else:
            last_filter_none = False

        if self.current_filter == (search_term, severity):
            # Do nothing - filter unchanged
            return
        else:
            self.current_filter = (search_term, severity)

        if search_term == None and severity == None:
            # No filter - restore all data to the control
            for o in self.data_filtered:
                self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(o))
            for o in self.data:
                self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(o))
            self.data_filtered = list()
        else:
            # Loop through data and remove those items which do not match the filter
            if last_filter_none:
                for o in self.data:
                    self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(o))
            else:
                for o in self.data_filtered:
                    self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(o))

            self.data_filtered = list()
            for o in self.data:
                if search_term is not None and search_term in o.get_str():
                    self.data_filtered.append(o)
                    self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(o))
                elif o.template.severity == severity:
                    self.data_filtered.append(o)
                    self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(o))

    def DeleteAllItems(self):
        """Perminently delets all data currently stored in this model
        """

        for d in self.data:
            self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(d))
        self.data = list()

    def getDataLen(self):
        """Get the length of the list of data objects currently being used internally by this model

        Returns:
            int -- length of the data list
        """

        if self.current_filter == (None, None):
            return len(self.data)
        else:
            return len(self.data_filtered)


