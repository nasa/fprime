import wx
import GDSChannelTelemetryPanelGUI
from pprint import pprint
###########################################################################
## Class ChannelTelemetryImpl
###########################################################################

class ChannelTelemetryImpl (GDSChannelTelemetryPanelGUI.ChannelTelemetry):

    #TODO Need to figure out the TreeCtrl, using ListCtrl now...
    def __init__( self, parent ):
        GDSChannelTelemetryPanelGUI.ChannelTelemetry.__init__ ( self, parent)
        self.ChannelTelemDataViewTreeCtl.AppendTextColumn("Channel", 0, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=-1, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewTreeCtl.AppendTextColumn("ID", 1, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=-1, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewTreeCtl.AppendTextColumn("Time", 2, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=-1, align=wx.ALIGN_NOT)
        self.ChannelTelemDataViewTreeCtl.AppendTextColumn("Value", 3, mode=wx.dataview.DATAVIEW_CELL_ACTIVATABLE, width=-1, align=wx.ALIGN_NOT)

        #self.ChannelTelemDataViewTreeCtl.AppendContainer(wx.dataview.NullDataViewItem, 'Test Root', data='Test data')

        self._channel_items = dict()

    def __del__( self ):
        pass

    def data_callback(self, data):

        print data

    # Override these handlers to implement functionality for GUI elements
    def onChannelTelemSelectChannelsButtonClick( self, event ):
        event.Skip()

    def onChannelTelemShowHexCheckBoxClick( self, event ):
        event.Skip()

class ChannelTelemDataViewModel(wx.dataview.PyDataViewModel):

    def __init__(self, data):
        dv.PyDataViewModel.__init__(self)
        self.data = data

        # The PyDataViewModel derives from both DataViewModel and from
        # DataViewItemObjectMapper, which has methods that help associate
        # data view items with Python objects. Normally a dictionary is used
        # so any Python object can be used as data nodes. If the data nodes
        # are weak-referencable then the objmapper can use a
        # WeakValueDictionary instead.
        self.UseWeakRefs(False)


    # Report how many columns this model provides data for.
    def GetColumnCount(self):
        return 4

    # Map the data column numbers to the data type
    def GetColumnType(self, col):
        mapper = { 0 : 'string',
                   1 : 'string',
                   2 : 'datetime',
                   3 : 'string', # the real value is an int, but the renderer should convert it okay
                   }
        return mapper[col]

    def GetChildren(self, parent, children):
        # The view calls this method to find the children of any node in the
        # control. There is an implicit hidden root node, and the top level
        # item(s) should be reported as children of this node. A List view
        # simply provides all items as children of this hidden root. A Tree
        # view adds additional items as children of the other items, as needed,
        # to provide the tree hierachy.
        ##self.log.write("GetChildren\n")

        # If the parent item is invalid then it represents the hidden root
        # item, so we'll use the genre objects as its children and they will
        # end up being the collection of visible roots in our tree.
        if not parent:
            for genre in self.data:
                children.append(self.ObjectToItem(genre))
            return len(self.data)

        # Otherwise we'll fetch the python object associated with the parent
        # item and make DV items for each of its child objects.
        node = self.ItemToObject(parent)
        if isinstance(node, Genre):
            for song in node.songs:
                children.append(self.ObjectToItem(song))
            return len(node.songs)
        return 0


    def IsContainer(self, item):
        # Return True if the item has children, False otherwise.
        ##self.log.write("IsContainer\n")

        # The hidden root is a container
        if not item:
            return True
        # and in this model the genre objects are containers
        node = self.ItemToObject(item)
        if isinstance(node, Genre):
            return True
        # but everything else (the song objects) are not
        return False


    #def HasContainerColumns(self, item):
    #    self.log.write('HasContainerColumns\n')
    #    return True


    def GetParent(self, item):
        # Return the item which is this item's parent.
        ##self.log.write("GetParent\n")

        if not item:
            return dv.NullDataViewItem

        node = self.ItemToObject(item)
        if isinstance(node, Genre):
            return dv.NullDataViewItem
        elif isinstance(node, Song):
            for g in self.data:
                if g.name == node.genre:
                    return self.ObjectToItem(g)


    def GetValue(self, item, col):
        # Return the value to be displayed for this item and column. For this
        # example we'll just pull the values from the data objects we
        # associated with the items in GetChildren.

        # Fetch the data object for this item.
        node = self.ItemToObject(item)

        if isinstance(node, Genre):
            # We'll only use the first column for the Genre objects,
            # for the other columns lets just return empty values
            mapper = { 0 : node.name,
                       1 : "",
                       2 : "",
                       3 : "",
                       4 : wx.DateTime.FromTimeT(0),  # TODO: There should be some way to indicate a null value...
                       5 : False,
                       }
            return mapper[col]


        elif isinstance(node, Song):
            mapper = { 0 : node.genre,
                       1 : node.artist,
                       2 : node.title,
                       3 : node.id,
                       4 : node.date,
                       5 : node.like,
                       }
            return mapper[col]

        else:
            raise RuntimeError("unknown node type")



    def GetAttr(self, item, col, attr):
        ##self.log.write('GetAttr')
        node = self.ItemToObject(item)
        if isinstance(node, Genre):
            attr.SetColour('blue')
            attr.SetBold(True)
            return True
        return False


    def SetValue(self, value, item, col):
        self.log.write("SetValue: %s\n" % value)

        # We're not allowing edits in column zero (see below) so we just need
        # to deal with Song objects and cols 1 - 5

        node = self.ItemToObject(item)
        if isinstance(node, Song):
            if col == 1:
                node.artist = value
            elif col == 2:
                node.title = value
            elif col == 3:
                node.id = value
            elif col == 4:
                node.date = value
            elif col == 5:
                node.like = value
        return True
