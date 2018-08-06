import GDSChannelFilterDialogGUI

###########################################################################
## Class ChannelFilterDialogImpl
###########################################################################

class ChannelFilterDialogImpl(GDSChannelFilterDialogGUI.ChannelFilterDialog):
    
    def __init__( self, parent, ch_dict ):

        GDSChannelFilterDialogGUI.ChannelFilterDialog.__init__ ( self, parent)
        self.ch_dict = ch_dict

        self.ListBoxAllChannels.SetItems([c.get_full_name() for c in ch_dict.values()])
        self.ListBoxShowChannels.SetItems([c.get_full_name() for c in ch_dict.values()])


    # Virtual event handlers, overide them in your derived class
	def onClickAddChannel( self, event ):
		event.Skip()
	
	def onClickAddAllChannels( self, event ):
		event.Skip()
	
	def onClickRemoveChannel( self, event ):
		event.Skip()
	
	def onClickRemoveAllChannels( self, event ):
		event.Skip()
	
	def onClickSaveFilter( self, event ):
		event.Skip()
	
	def onClickLoadFilter( self, event ):
		event.Skip()
	
	def onClickApplyFilter( self, event ):
		event.Skip()
	
	def onClickCancel( self, event ):
		event.Skip()