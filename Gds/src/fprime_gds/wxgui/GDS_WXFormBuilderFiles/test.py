import GDSMainFrameImpl
import wx

app = wx.App(False)
frame = GDSMainFrameImpl.MainFrameImpl(None)
frame.Show(True)

app.MainLoop()
