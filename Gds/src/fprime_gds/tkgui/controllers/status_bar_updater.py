import Pmw
import time

from fprime_gds.tkgui.controllers import consumer


class StatusBarUpdater(consumer.Consumer):
    """
    A singleton class that will update the status
    bar with number of bytes sent and recived; and
    status updates. Connected to StatusUpdater inside
    of StatusUpdater.
    """
    __instance = None

    def __init__(self):

        super(StatusBarUpdater, self).__init__()

        #
        # Variables used by standard Update interface
        #
        self.__num_recv     = 0
        self.__num_sent     = 0
        self.__message      = ""
        self.__mesage_color = "black"
        self.__balloon      = None
        self.__show_help    = False
        self.__help_message = ""
        self.__data_incoming= False
        #
        self.__exit         = False
        self.__current_time = 0


    def getInstance():
        """
        Return instance of singleton.
        """
        if(StatusBarUpdater.__instance is None):
            StatusBarUpdater.__instance = StatusBarUpdater()
        return StatusBarUpdater.__instance

    #define static method
    getInstance = staticmethod(getInstance)


    def register_balloon(self, top):
        """
        Link our balloon with the top
        panel.
        """
        if self.__balloon is None:
            self.__balloon = Pmw.Balloon(top)
            self.__balloon.configure(statuscommand=self.update_helpmessage)


    def register_help(self, widget, text):
        """
        Register help text for widget.
        """
        if self.__balloon is None:
             raise NotImplementedError("Help balloon must be registered.")
        self.__balloon.bind(widget, balloonHelp=None, statusHelp=text)


    def set_help(self, val):
        self.__show_help = val


    def get_help(self):
        return self.__show_help


    def get_help_message(self):
        return self.__help_message


    def get_exit(self):
        return self.__exit


    def get_num_packets_sent(self):
        return self.__num_sent


    def get_num_packets_recv(self):
        return self.__num_recv


    def get_message(self):
        return self.__message


    def get_message_state(self):
        return self.__mesage_state


    def get_message_color(self):
        return self.__mesage_color


    def notify_exit(self):
        self.__exit = True
        self.notifyObservers()


    def update_helpmessage(self, message):
        self.__show_help    = True
        self.__help_message = message

        self.notifyObservers(message)


    def update_message(self, message, color="black"):
        self.__message = message
        self.__mesage_color = color

        self.notifyObservers(message)


    def process_data(self, data):
      # Data is tuple (number of recevied bytes, number of send bytes)
      num_recv, num_sent = data

      self.__num_recv += num_recv
      self.__num_sent += num_sent
