'''
@brief Implementation class for the channel telemetry panel

@date Created July 16, 2018
@author Josef X. Biberstein

@bug No known bugs
'''

import wx
import GDSChannelTelemetryPanelGUI
from pprint import pprint
import inspect
import GDSChannelFilterDialogImpl

from data_types.ch_data import *
from data_types.pkt_data import *
from models.serialize.serializable_type import SerializableType

###########################################################################
## Class ChannelTelemetryImpl
###########################################################################

class ChannelTelemetryImpl (GDSChannelTelemetryPanelGUI.ChannelTelemetry):
    '''Implmentation class. Defines functionality of the channel telemetry panel.'''
    
    def __init__( self, parent, ch_dict={} ):
        """Constructor for the ChannelTelemetryImpl
        
        Arguments:
            parent {wx.Window} -- The parent window for this panel
        """

        self.ch_dict = ch_dict

        GDSChannelTelemetryPanelGUI.ChannelTelemetry.__init__ ( self, parent)

        self.dv_model = ChannelTelemDataViewModel([])
    
        self.ChannelTelemDataViewCtl.AssociateModel(self.dv_model)

        self.ChannelTelemDataViewCtl.AppendTextColumn("Channel", 0, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=350, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewCtl.AppendTextColumn("ID", 1, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=50, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewCtl.AppendTextColumn("Time", 2, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=150, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewCtl.AppendTextColumn("Value", 3, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=-1, align=wx.ALIGN_NOT)

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


    # Override these handlers to implement functionality for GUI elements
    def onChannelTelemSelectChannelsButtonClick( self, event ):
        dlog = GDSChannelFilterDialogImpl.ChannelFilterDialogImpl(self, self.ch_dict)
        dlog.ShowModal()
        dlog.Destroy()
        event.Skip()

    def onChannelTelemShowHexCheckBoxClick( self, event ):
        event.Skip()

class ChannelTelemDataViewModel(wx.dataview.PyDataViewModel):
    """This class acts as an intermediary between user data and the actual data 
    view display. It stores data and maintains a mapping from data to items in 
    the data view. Most of the methdos in this class just need to be defined 
    and are called automatically by the data view.
    """

    def __init__(self, data):
        wx.dataview.PyDataViewModel.__init__(self)
        self.data = data

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

        return self.data




