import GDSChannelFilterDialogGUI

###########################################################################
## Class ChannelFilterDialogImpl
###########################################################################

class ChannelFilterDialogImpl(GDSChannelFilterDialogGUI.ChannelFilterDialog):
    
    def __init__( self, parent, ch_dict ):

        GDSChannelFilterDialogGUI.ChannelFilterDialog.__init__ ( self, parent)
        self.ch_dict = ch_dict
        self.filter = []
        self.ListBoxAllChannels.SetItems([c.get_full_name() for c in ch_dict.values()])

        # List of channel templates that should be displayed with the current filter


    def Swap(self, dest, src, all_itms=False):

        if all_itms:
            itms = src.Items
            dest.AppendItems(itms)
            src.Clear()
        else:
            idxs = src.GetSelections()
            itms = [src.GetString(i) for i in idxs]
            dest.AppendItems(itms)
            for i in itms:
                src.Delete(src.FindString(i))
        
    def GetFilter(self):
        return self.filter
            
    # Virtual event handlers, overide them in your derived class
    def onCloseChannelFilterDialog( self, event ):
        self.EndModal(-1)
	
    def onClickAddChannel( self, event ):
        self.Swap(self.ListBoxShowChannels, self.ListBoxAllChannels)
        event.Skip()
    
    def onClickAddAllChannels( self, event ):
        self.Swap(self.ListBoxShowChannels, self.ListBoxAllChannels, all_itms=True)
        event.Skip()
    
    def onClickRemoveChannel( self, event ):
        self.Swap(self.ListBoxAllChannels, self.ListBoxShowChannels)
        event.Skip()
    
    def onClickRemoveAllChannels( self, event ):
        self.Swap(self.ListBoxAllChannels, self.ListBoxShowChannels, all_itms=True)
        event.Skip()
    
    def onClickSaveFilter( self, event ):
        event.Skip()
    
    def onClickLoadFilter( self, event ):
        event.Skip()
    
    def onClickApplyFilter( self, event ):
        self.filter = self.ListBoxShowChannels.Items
        self.EndModal(0)

    def onClickClose( self, event ):
        self.EndModal(-1)