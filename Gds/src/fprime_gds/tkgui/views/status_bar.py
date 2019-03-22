import Tkinter
import Pmw

from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import status_bar_updater

from fprime_gds.tkgui.utils import ConfigManager
import time


class StatusBar(Tkinter.Frame):
    """
    Status bar to show text and visual status of gse.py.
    """

    def __init__(self, root, bg):
        Tkinter.Frame.__init__(self, root, relief=Tkinter.RAISED, borderwidth=0, bg=bg)

        self.__root = root

        # Get periodicity of event panel refresh
        config = ConfigManager.ConfigManager.getInstance()
        self.__update_period = config.get('performance', 'status_bar_update_period')

        self.__message_bar = Pmw.MessageBar(self, labelmargin=0)
        # Configure message timing
        self.__message_bar['messagetypes']['help']= (1, 1, 0, 0)
        self.__message_bar.pack(side=Tkinter.LEFT, fill=Tkinter.X, expand=1, padx=0, pady=0)
        self.__message_bar._messageBarEntry.config(readonlybackground=bg, relief=Tkinter.FLAT, borderwidth=0)
        self.__message_bar._messageBarEntry.update_idletasks()

        self.__indicator_light = Tkinter.Canvas(self, height=30, width=30, bg=bg,highlightthickness=0, borderwidth=0)
        self.__indicator_light.create_oval(4,4,27,27,fill="red", tags="status_light")
        self.__indicator_light.pack(side=Tkinter.RIGHT)

        self.__green_light = False
        self.__deadband = int(config.get("performance", "status_light_deadband")) # Time in seconds to keep light green
        self.__last_change = 0
        self.__recv_count = 0

        self.__packet_label_format = "<BYTES> Sent: {s} Recv: {r}"
        self.__packet_label = Tkinter.Label(self, text=self.__packet_label_format.format(s="0",r="0"), bd=1, bg=bg)
        self.__packet_label.pack(side=Tkinter.RIGHT)

        # Id for UI refresh loop
        self.__after_id = None

        self.pack(side=Tkinter.BOTTOM, expand=0, fill=Tkinter.X)

        self.__status_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()


    def __clear_message(self):
        self.__message_bar.resetmessages('state')


    def __update_light(self, data_incoming):
        """

        If data incoming switch green.
        Else swtich red.

        But only switch to red after deadband time has elapsed.
        """
        if data_incoming:
            self.__last_change = time.time()
            if not self.__green_light:
                self.__indicator_light.itemconfig("status_light", fill="green")
                self.__green_light = True
        else:
            if self.__green_light:
                if time.time() - self.__last_change > self.__deadband:
                    self.__last_change = time.time()
                    self.__indicator_light.itemconfig("status_light", fill="red")
                    self.__green_light = False


    def __update_telem(self):
        sent = self.__status_bar_updater.get_num_packets_sent()
        recv = self.__status_bar_updater.get_num_packets_recv()
        self.__packet_label.config(text=self.__packet_label_format.format(s=sent, r=recv))

        if recv > self.__recv_count:
          self.__update_light(True)
          self.__recv_count = recv
        else:
          self.__update_light(False)


    def __update_text(self):
        color     = self.__status_bar_updater.get_message_color()
        text      = self.__status_bar_updater.get_message()
        help_text = self.__status_bar_updater.get_help_message()

        # Help Messages
        if self.__status_bar_updater.get_help():
            self.__message_bar.message('help', help_text)
            self.__status_bar_updater.set_help(False)
            return

        # Standard Status messages
        self.__message_bar.message('state', text)
        self.__message_bar._messageBarEntry.config(fg=color)

    def refresh(self):
        self.__update_telem()
        self.__update_text()

        # Rerun after delay period
        self.__after_id = self.__root.after(self.__update_period, self.refresh)

    def refresh_cancel(self):
        self.__root.after_cancel(self.__after_id)

if __name__ == '__main__':
    root = Tkinter.Tk()
    sb = StatusBar(root)
    sb.pack(expand=1, fill=Tkinter.X)

    sb.setText("Hello","black")
    root.mainloop()
