# -*- coding: utf-8 -*- 
import wx
import PexpectRunnerConsolGUI

###########################################################################
## Class PexpectRunnerImp
###########################################################################

class PexpectRunnerImpl ( PexpectRunnerConsolGUI.PexpectRunnerGUI ):
	
	def __init__( self, parent ):
		PexpectRunnerConsolGUI.PexpectRunnerGUI.__init__ ( self, parent)
	
	def __del__( self ):
		pass
	
	# TODO Doesn't really work right now. Just going to leave it autoscrolling which is default. Someone can fix this in the future.
	def AppendMessage(self, msg_text):
		print(self.TextCtrlConsol.GetScrollPos(wx.VERTICAL))
		print(self.TextCtrlConsol.GetScrollRange(wx.VERTICAL))
		print(self.TextCtrlConsol.GetScrollThumb(wx.VERTICAL))
		# If we are at the bottom of the consol, shift the box to show new text
		if self.TextCtrlConsol.GetScrollPos(wx.VERTICAL) + self.TextCtrlConsol.GetScrollThumb(wx.VERTICAL) == self.TextCtrlConsol.GetScrollRange(wx.VERTICAL):
			print('At bottom...')
			self.TextCtrlConsol.write(msg_text + '\n') # Write text with new line to consol
			self.TextCtrlConsol.SetScrollPos(wx.VERTICAL, self.TextCtrlConsol.GetScrollRange(wx.VERTICAL)) # Set the scroll to the end
		# If we are not at the botton of the consol, just append the text and do nothing else
		else:
			print('Not at bottom...')
			self.TextCtrlConsol.Freeze()
			self.TextCtrlConsol.write(msg_text + '\n')
			self.TextCtrlConsol.Thaw()

	# Override these
	def onWindowClose( self, event ):
		event.Skip()
	
	def onMouseWheel( self, event ):
		event.Skip()
	
