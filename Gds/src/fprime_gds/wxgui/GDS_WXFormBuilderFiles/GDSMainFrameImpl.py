import GDSChannelTelemetryPanelImpl
import GDSCommandPanelImpl
import GDSLogEventPanelImpl
import GDSMainFrameGUI
import GDSStatusPanelImpl

###########################################################################
## Class MainFrameImpl
###########################################################################


class MainFrameImpl(GDSMainFrameGUI.MainFrame):
    def __init__(self, parent):
        GDSMainFrameGUI.MainFrame.__init__(self, parent)

        self.cmd_pnl = GDSCommandPanelImpl.CommandsImpl(self.TabNotebook)
        self.status_pnl = GDSStatusPanelImpl.StatusImpl(self.TabNotebook)
        self.event_pnl = GDSLogEventPanelImpl.LogEventsImpl(self.TabNotebook)
        self.telem_pnl = GDSChannelTelemetryPanelImpl.ChannelTelemetryImpl(
            self.TabNotebook
        )

        self.TabNotebook.AddPage(self.cmd_pnl, "Commands", False)
        self.TabNotebook.AddPage(self.status_pnl, "Status", False)
        self.TabNotebook.AddPage(self.event_pnl, "Log Events", False)
        self.TabNotebook.AddPage(self.telem_pnl, "Channel Telemetry", False)

        self.child_main_instances = []

    def __del__(self):
        pass

    # Override these handlers to implement functionality for GUI elements
    def onNewMenuItemClick(self, event):
        frame = MainFrameImpl(self)
        self.child_main_instances.append(frame)
        frame.Show(True)

    def onSaveMenuItemClick(self, event):
        event.Skip()

    def onLoadMenuItemClick(self, event):
        event.Skip()

    def onAboutMenuItemClick(self, event):
        event.Skip()

    def onSaveWinCfgMenuItemClick(self, event):
        event.Skip()

    def onRestoreWinMenuItemClick(self, event):
        event.Skip()

    def onExitMenuItemClick(self, event):
        event.Skip()
