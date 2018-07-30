import wx
import GDSLogEventPanelGUI
from data_types.event_data import EventData
from pprint import pprint
from utils.event_severity import EventSeverity

###########################################################################
## Class LogEventsImpl
###########################################################################

class LogEventsImpl (GDSLogEventPanelGUI.LogEvents):
    '''Implementation class for LogEvents panel. Defines funcitonality.'''


    def __init__( self, parent ):
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
        
        self.scrollEventLogToBottom()
    def __del__( self ):
        self.dv_model.DecRef()

    def data_callback(self, data):
        """Recieves data from decoders to which this consumer is registered

        Arguments:
            data {Data Object} -- A Data Object containing the data passed from the decoder (e.g., an EventData object)
        """
        if self.dv_model.RefCount > 1:
            self.dv_model.UpdateModel(data)

    def scrollEventLogToBottom(self):
        """Move the event log scroll bar so that the last entry is visible. Called repeatedly when the "scroll" box is checked"
        """

        l = self.dv_model.getDataLen()

        if self.EventLogScrollCheckBox.GetValue() == True and l > 0:
            i = self.dv_model.ObjectToItem(self.dv_model.data[int(l - 1)])
            self.EventLogDataListCtl.EnsureVisible(i)
            self.EventLogDataListCtl.Refresh()
        wx.CallLater(10, self.scrollEventLogToBottom)

    def getEventLogState(self):
        return self.dv_model.data

    def setEventLogState(self, state):
        for r in state:
            self.dv_model.UpdateModel(r)

    # Override these handlers to implement functionality for GUI elements
    def onEventLogClearButtonClick( self, event ):
        self.dv_model.DeleteAllItems()

    def onEventLogApplyFilterButtonClick( self, event ):
        event.Skip()

    def onEventLogResetFilterButtonClick( self, event ):
        event.Skip()

    def onEventLogDataListCtrlScroll(self, event):
        self.EventLogScrollCheckBox.Value = False
        event.Skip()


class EventLogDataViewModel(wx.dataview.PyDataViewModel):

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
        return 5

    # Map the data column numbers to the data type
    def GetColumnType(self, col):
        mapper = { 0 : 'string',
                   1 : 'string',
                   2 : 'string',
                   3 : 'string',
                   3 : 'string', 
                   }
        return mapper[col]

    def GetChildren(self, parent, children):
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
        return 0


    def IsContainer(self, item):
        # Return True if the item has children, False otherwise.

        # The hidden root is a container
        if not item:
            return True
        return False



    def GetParent(self, item):
        # Return the item which is this item's parent.
        return wx.dataview.NullDataViewItem


    def GetValue(self, item, col):
        # Return the value to be displayed for this item and column. For this
        # example we'll just pull the values from the data objects we
        # associated with the items in GetChildren.

        # Fetch the data object for this item.
        node = self.ItemToObject(item)
        arg_vals = tuple([arg.val for arg in node.args])
        
        if isinstance(node, EventData):
            mapper = { 0 : str(node.time.to_readable()),
                       1 : str(node.template.name),
                       2 : str(node.template.id),
                       3 : str(node.template.severity.name),
                       4 : str(node.template.format_str%arg_vals)
                       }                                                                                                                                                                                                        
            return mapper[col]

        else:
            raise RuntimeError("unknown node type")


    def GetAttr(self, item, col, attr):

        node = self.ItemToObject(item)
        if isinstance(node, EventData) and col == 3:
            if node.template.severity == EventSeverity.FATAL:
                attr.SetColour('red')
                attr.SetBold(True)
            elif node.template.severity == EventSeverity.ACTIVITY_HI or node.template.severity == EventSeverity.WARNING_HI:
                attr.SetColour('orange')
                attr.SetBold(True)
            return True
        return False

    def UpdateModel(self, new_data):
        self.data.append(new_data)
        self.ItemAdded(wx.dataview.NullDataViewItem, self.ObjectToItem(new_data))
        print(len(self.data))

    def DeleteAllItems(self):
        for d in self.data:
            self.ItemDeleted(wx.dataview.NullDataViewItem, self.ObjectToItem(d))
        self.data = list()

    def getDataLen(self):
        return len(self.data)
    
