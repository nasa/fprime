import Tkinter
import Pmw

from controllers import event_listener
from controllers import status_updater

from utils import ConfigManager
import time


class StatusBar(Tkinter.Frame):
    """
    Status bar to show text and visual status of gse.py.
    """

    def __init__(self, root, bg):
        Tkinter.Frame.__init__(self, root, relief=Tkinter.RAISED, borderwidth=0, bg=bg)

        self.__root = root
        self.__config = ConfigManager.ConfigManager.getInstance()

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
        self.__last_change = 0 
        self.__deadband    = int(self.__config.get("performance", "status_light_deadband")) # Time in seconds to keep light green
        self.__last_update = 0

        self.__packet_label_format = "<BYTES> Sent: {s} Recv: {r}"
        self.__packet_label = Tkinter.Label(self, text=self.__packet_label_format.format(s="0",r="0"), bd=1, bg=bg)
        self.__packet_label.pack(side=Tkinter.RIGHT)

        self.pack(side=Tkinter.BOTTOM, expand=0, fill=Tkinter.X)


    def __clear_message(self):
        self.__message_bar.resetmessages('state')


    def __update_light(self, observer):
        """
        is_data_incoming == True when queue is being processed.
        is_data_incoming == False when queue is empty.

        If data incoming switch green.
        Else swtich red.

        But only switch to red after deadband time has elapsed.
        """
        is_data_incoming = observer.is_data_incoming()
        if is_data_incoming:
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

    def __update_telem(self, observer):
        sent = observer.get_num_packets_sent()
        recv = observer.get_num_packets_recv()
        self.__packet_label.config(text=self.__packet_label_format.format(s=sent, r=recv))

        #self.__root.after_idle(self.handle_label, sent, recv)
        #self.__packet_label.update_idletasks()
        


    def __update_text(self, observer):
        color     = observer.get_message_color()
        text      = observer.get_message()
        help_text = observer.get_help_message()

        # Help Messages
        if observer.get_help():
            self.__message_bar.message('help', help_text)
            observer.set_help(False)
            return

        # Standard Status messages
        self.__message_bar.message('state', text) 
        self.__message_bar._messageBarEntry.config(fg=color) 
        #self.__root.update()      
    

    def update(self, observer):
        self.__update_light(observer)

        if time.time() - self.__last_update > 1:
            self.__last_update = time.time()
            self.__root.after_idle(self.do_update, observer)

    def do_update(self, *args):
        observer = args[0]
        self.__update_telem(observer)
        self.__update_text(observer)


def get_ms():
    return time.time() * 1000


if __name__ == '__main__':
    root = Tkinter.Tk()
    sb = StatusBar(root)
    sb.pack(expand=1, fill=Tkinter.X)

    sb.setText("Hello","black")
    root.mainloop()   