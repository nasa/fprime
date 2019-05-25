'''
@brief Implementation class for the channel telemetry panel

@date Created July 16, 2018
@author Josef X. Biberstein

@bug No known bugs
'''
from __future__ import absolute_import

import wx
from . import GDSChannelTelemetryPanelGUI
import inspect
from . import GDSChannelFilterDialogImpl
from copy import deepcopy

from fprime_gds.common.data_types.ch_data import *
from fprime_gds.common.data_types.pkt_data import *
from fprime.common.models.serialize.serializable_type import SerializableType
from fprime_gds.common.utils.config_manager import ConfigManager

###########################################################################
## Class ChannelTelemetryImpl
###########################################################################

class ChannelTelemetryImpl (GDSChannelTelemetryPanelGUI.ChannelTelemetry):
    '''Implmentation class. Defines functionality of the channel telemetry panel.'''

    def __init__( self, parent, ch_dict={}, config=None ):
        """Constructor for the ChannelTelemetryImpl

        Arguments:
            parent {wx.Window} -- The parent window for this panel
        """
        self.ch_dict = ch_dict
        self.config = config

        GDSChannelTelemetryPanelGUI.ChannelTelemetry.__init__ ( self, parent)

        self.dv_model = ChannelTelemDataViewModel(self, ch_dict)
    
        self.ChannelTelemDataViewCtl.AssociateModel(self.dv_model)

        self.ChannelTelemDataViewCtl.AppendTextColumn("Channel", 0, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=350, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewCtl.AppendTextColumn("ID", 1, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=50, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewCtl.AppendTextColumn("Time", 2, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=150, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewCtl.AppendTextColumn("Value", 3, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=-1, align=wx.ALIGN_NOT)

        # NOTE could just make the first column sortable cause sorting by the others doesn't really make sense
        for c in self.ChannelTelemDataViewCtl.Columns:
            c.Sortable = True

        self.ChannelTelemDataViewCtl.Bind(wx.EVT_KEY_DOWN, self.onCopyKeyPressed)

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

    def getChannelTelemDataViewState(self):
        """Get the internal data list used by the model to populate the data view for telem panel

        Returns:
            list -- list of ChData or PktData objects
        """

        return self.dv_model.GetData()

    def setChannelTelemDataViewState(self, data):
        """Set the internal data list used by the model to populate the data view for telem

        Arguments:
            data {list} -- list of ChData or PktData objects
        """

        self.dv_model.SetData(data)

    def onCopyKeyPressed(self, event):
        """Callback for key pressed within the data view control
        """

        # Ctrl-C pressed
        if event.ControlDown() and event.GetKeyCode() == 67:
            rows = self.ChannelTelemDataViewCtl.GetSelections()
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
        """Callback for pressing the copy option in the context menu of the data view ctrl
        
        Arguments:
            event {wx.Event} -- standard event arg from WX
        """

        rows = self.ChannelTelemDataViewCtl.GetSelections()
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
    def onChannelTelemContextMenu( self, event ):

        # This is called to create a context menu in the data view to allow calling of copy through a right click
        if not hasattr(self, 'copy_context_id'):
            self.copy_context_id = wx.NewId()
            self.Bind(wx.EVT_MENU, self.onCopyKeyPressedContext, id=self.copy_context_id)

        menu = wx.Menu()
        cpy = menu.Append(self.copy_context_id, "copy")

        self.PopupMenu(menu)
        menu.Destroy()

        event.Skip()

    def onChannelTelemSelectChannelsButtonClick( self, event ):

        # Get a new channel filter by prompting the user with a dialog
        dlog = GDSChannelFilterDialogImpl.ChannelFilterDialogImpl(self, self.ch_dict, config=self.config)
        ret = dlog.ShowModal()
        if ret == 0:
            self.dv_model.ChangeFilter(dlog.GetFilter())
        self.ChannelTelemFilterSelectedTextCtl.SetValue(dlog.current_filter_name)
        dlog.Destroy()
        event.Skip()

    def onChannelTelemShowHexCheckBoxClick( self, event ):

        # Display channel IDs as hexidecimal values
        self.ChannelTelemDataViewCtl.SelectAll()
        s = self.ChannelTelemDataViewCtl.GetSelections()
        for i in s:
            self.dv_model.ItemChanged(i)
        self.ChannelTelemDataViewCtl.UnselectAll()
        event.Skip()

    def onClickResetFilter( self, event ):

        # Clear the filter
        self.dv_model.ChangeFilter([])
        self.ChannelTelemFilterSelectedTextCtl.SetValue("")
        event.Skip()


class ChannelTelemDataViewModel(wx.dataview.PyDataViewModel):
    """This class acts as an intermediary between user data and the actual data
    view display. It stores data and maintains a mapping from data to items in
    the data view. Most of the methdos in this class just need to be defined
    and are called automatically by the data view.
    """

    def __init__(self, parent, ch_dict, config=None):
        '''
        Constructor

        Args:
            ch_dict (dict() object with all ChTemplate objects as values):
                    Channel dictionary
            config (ConfigManager obj, default=None): ConfigManager with color
                   information. If None, defaults used
        '''

        wx.dataview.PyDataViewModel.__init__(self)

        if config==None:
            config = ConfigManager()

        self.config = config
        self.parent = parent

        # Colors in config object are Hex codes stored as strings.
        #  Convert the string to an int, and then convert to a wxPython Colour
        self.red = wx.Colour(int(self.config.get('colors', 'red'), 16))
        self.orange = wx.Colour(int(self.config.get('colors', 'orange'), 16))
        self.yellow = wx.Colour(int(self.config.get('colors', 'yellow'), 16))


        # All the possible ChData objects that we can possibly recieve
        self.data = []
        for c in ch_dict.values():
            self.data.append(ChData.get_empty_obj(c))

        # The ChData objects that we have recieved
        self.chs_seen = []

        # The current list of full_names that we want to filter for
        self.filter = []
        self.prev_filter = []

        # The PyDataViewModel derives from both DataViewModel and from
        # DataViewItemObjectMapper, which has methods that help associate
        # data view items with Python objects. Normally a dictionary is used
        # so any Python object can be used as data nodes. If the data nodes
        # are weak-referencable then the objmapper can use a
        # WeakValueDictionary instead.
        self.UseWeakRefs(True)

    # Report how many columns this model provides data for.
    def GetColumnCount(self):

        """Get the number of columns

        Returns:
            int -- the number of columns
        """

        return 4

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
                   3 : 'string', # the real value is an int, but the renderer should convert it okay
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
        # item, so we'll use the genre objects as its children and they will
        # end up being the collection of visible roots in our tree.
        if not parent:
            # If there is no filter
            if self.filter == []:
                for obj in self.chs_seen:
                    children.append(self.ObjectToItem(obj))

                return len(self.chs_seen)
            else:
                
                gen = [x for x in self.data if x.template.get_full_name() in self.filter]
                for obj in gen:
                    children.append(self.ObjectToItem(obj))

                return len(gen)

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
        if isinstance(node, ChData):
            if node.val_obj != None:
                if self.parent.ChannelTelemShowHexCheckBox.Value == True:
                    mapper = { 0 : str(node.template.get_full_name()),
                            1 : str(hex(node.template.id)),
                            2 : str(node.time.to_readable()),
                            3 : node.get_val_str()
                            }     
                else:
                    mapper = { 0 : str(node.template.get_full_name()),
                            1 : str(node.template.id),
                            2 : str(node.time.to_readable()),
                            3 : node.get_val_str()
                            }     
            else:
                mapper = { 0 : str(node.template.get_full_name()),
                        1 : str(node.template.id),
                        2 : u"",
                        3 : u""
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
        if isinstance(node, ChData):
            
            if node.val_obj != None:
                if node.template.low_red != None and node.val_obj.val < node.template.low_red:
                
                    attr.SetColour(self.red)
                    attr.SetBold(True)
                elif node.template.high_red != None and node.val_obj.val > node.template.high_red:
                    
                    attr.SetColour(self.red)
                    attr.SetBold(True)
                elif node.template.low_orange != None and node.val_obj.val < node.template.low_orange:
                    
                    attr.SetColour(self.orange)
                    attr.SetBold(True)
                elif node.template.high_orange != None and node.val_obj.val > node.template.high_orange:
                    
                    attr.SetColour(self.orange)
                    attr.SetBold(True)
                elif node.template.low_yellow != None and node.val_obj.val < node.template.low_yellow:
                    
                    attr.SetColour(self.yellow)
                    attr.SetBold(True)
                elif node.template.high_yellow != None and node.val_obj.val > node.template.high_yellow:
                    
                    attr.SetColour(self.yellow)
                    attr.SetBold(True)
        return True

    def UpdateModel(self, new_data):
        """Add a new data item to the event log.

        Arguments:
            new_data {EventData} -- the new event data to be added
        """

        if isinstance(new_data, PktData):
            for c in new_data.get_chs():
                self.UpdateModel(c)
        elif isinstance(new_data, ChData):

            # Update list of all possible ChData with new_data
            null_member = [x for x in self.data if x.template == new_data.template]
            null_member = null_member[0]
            null_member.val_obj = deepcopy(new_data.val_obj)
            null_member.time = deepcopy(new_data.time)

            # Check if we have seen this Ch before
            match = [x for x in self.chs_seen if x.template == new_data.template]

            # If we haven't...
            if len(match) == 0:
                self.chs_seen.append(new_data)

                # If there is no filter, add the new_data to the view
                if self.filter == []:
                    self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))
                # If there is a filter, tell the updated member of the full Ch list to update in the view
                elif self.new_data.template.get_full_name() in self.filter:
                    self.ItemChanged(self.ObjectToItem(null_member))
            # If we have...
            else:
                # Update teh old data in the list
                old_data = match[0]
                old_data.val_obj.__dict__ = new_data.val_obj.__dict__.copy()
                old_data.time.__dict__ = new_data.time.__dict__.copy()

                if self.filter == []:
                    self.ItemChanged(self.ObjectToItem(old_data))
                elif new_data.template.get_full_name() in self.filter:
                    self.ItemChanged(self.ObjectToItem(null_member))

    def ChangeFilter(self, filt):
        """Change the filter on this model
        
        Arguments:
            filt {list} -- list of channel full names that we want to display
        """


        # Don't do anything if the filters are the same
        if self.filter == filt:
            return

        # Set filter and prev_filter
        self.prev_filter = self.filter
        self.filter = filt

        # Reset filter - add back all the channels in the data_seen list
        if filt == []:
            c = [self.ObjectToItem(d) for d in self.data if d.template.get_full_name() in self.prev_filter]

            for i in c:
                self.ItemDeleted(wx.dataview.NullDataViewItem, i)
            for d in self.chs_seen:
                self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(d))

        else:
            # If the previous filter was empty...
            if self.prev_filter == []:

                # Remove everything in the data seen list
                for d in self.chs_seen:
                    self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(d))

                # Add everything from data that is in the filter
                for d in self.data:
                    if d.template.get_full_name() in filt:
                        self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(d))

            else:
                for d in self.data:
                    # Remove everything in prev filter but not in new filter
                    if d.template.get_full_name() in self.prev_filter and d.template.get_full_name() not in filt:
                        self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(d))
                    # Add everything not in prev filter but in new filter
                    elif d.template.get_full_name() not in self.prev_filter and d.template.get_full_name() in filt:
                        self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(d))

    def SetData(self, data):
        """Set the data used by this model to populate the data view

        Arguments:
            data {list} -- list of ChData and/or PktData objects
        """

        for d in data:
            self.UpdateModel(d)

    def GetData(self):
        """Get the list of data used by this model to populate the data view

        Returns:
            list -- the data used in this model
        """

        return self.chs_seen

'''
Implementation for nesting channels inside of packets in the DataViewCtrl in case someone ever wants to implement that again - Josef Biberstein (jxb@mit.edu)

    def __init__(self, data):
        wx.dataview.PyDataViewModel.__init__(self)
        self.data = data
        self.filter = []

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
        return 4

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
                   3 : 'string', # the real value is an int, but the renderer should convert it okay
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
        # item, so we'll use the genre objects as its children and they will
        # end up being the collection of visible roots in our tree.
        if not parent:
            for obj in self.data:
                children.append(self.ObjectToItem(obj))
            return len(self.data)

        # Otherwise we'll fetch the python object associated with the parent
        # item and make DV items for each of its child objects.
        node = self.ItemToObject(parent)
        if isinstance(node, PktData):
            for ch in node.chs:
                children.append(self.ObjectToItem(ch))
            return len(node.chs)
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
        # and in this model the genre objects are containers
        node = self.ItemToObject(item)
        if isinstance(node, PktData):
            return True
        # but everything else (the song objects) are not
        return False



    def GetParent(self, item):
        """Get the parent of the given item

        Arguments:
            item {Item} -- input item

        Returns:
            Item -- the parent of the argument item
        """
        # Return the item which is this item's parent.

        if not item:
            return wx.dataview.NullDataViewItem

        node = self.ItemToObject(item)
        if isinstance(node, PktData):
            return wx.dataview.NullDataViewItem
        elif isinstance(node, ChData):
            if node.pkt is None:
                return wx.dataview.NullDataViewItem
            else:
                return self.ObjectToItem(node.pkt)


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

        if isinstance(node, PktData):
            # We'll only use the first column for the Genre objects,
            # for the other columns lets just return empty values
            mapper = { 0 : str(node.template.get_full_name()),
                       1 : str(node.template.id),
                       2 : str(node.time.to_readable()),
                       3 : ""
                       }
            return mapper[col]


        elif isinstance(node, ChData):
            mapper = { 0 : str(node.template.get_full_name()),
                       1 : str(node.template.id),
                       2 : str(node.time.to_readable()),
                       3 : str(node.val_obj.val)
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
        if isinstance(node, PktData):
            attr.SetColour('blue')
            attr.SetBold(True)
            return True
        if isinstance(node, ChData):
            if node.val_obj.val < node.template.low_yellow or node.val_obj.val > node.template.high_yellow:
                attr.SetColour('yellow')
                attr.SetBold(True)
            elif node.val_obj.val < node.template.low_orange or node.val_obj.val > node.template.high_orange:
                attr.SetColour('orange')
                attr.SetBold(True)
            elif node.val_obj.val < node.template.low_red or node.val_obj.val > node.template.high_red:
                attr.SetColour('red')
                attr.SetBold(True)

        return False

    def UpdateModel(self, new_data):
        """Add a new data item to the event log.

        Arguments:
            new_data {EventData} -- the new event data to be added
        """
        match = [x for x in self.data if x.template == new_data.template]

        if len(match) == 0:

            if isinstance(new_data, PktData):
                self.data.append(new_data)
                self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))
                for c in new_data.get_chs():
                    self.UpdateModel(c)
            elif isinstance(new_data, ChData):
                if new_data.get_pkt() is None:
                    self.data.append(new_data)
                    self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))
                else:
                    self.ItemAdded(self.ObjectToItem(new_data.get_pkt()), self.ObjectToItem(new_data))

        else:

            old_data = match[0]

            if isinstance(new_data, PktData):
                for o, n in zip(old_data.chs, new_data.chs):
                    o.val_obj.__dict__ = n.val_obj.__dict__.copy()
                    o.time.__dict__ = n.time.__dict__.copy()
                    self.ItemChanged(self.ObjectToItem(o))

            elif isinstance(new_data, ChData):
                old_data.val_obj.__dict__ = new_data.val_obj.__dict__.copy()
                old_data.time.__dict__ = new_data.time.__dict__.copy()
                self.ItemChanged(self.ObjectToItem(old_data))
'''


