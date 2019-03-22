import os
import Tkinter
import signal
import datetime

from fprime_gds.tkgui.controllers import observer

class StripChartListener(observer.Observable):
    """
    Provides a common interface for the stripchart panel and
    channel listener. DataMediator relays channel telemetry to
    the correct locations.
    """
    __instance = None


    def __init__(self):
        """
        Constructor.
        """
        super(StripChartListener, self).__init__()


        self.doUpdate = False

        self.__channel_dict = None

        self.__tlm = None
        self.__severity = None
        self.__event_name = None

    def update_telem(self, msg):
        """
        Called from channel listener.
        Gives mediator data to give to stripchart panel.
        @param msg: Tuple of channel telem values
        """
        (name, i, ch, t, ch_value,f,lr,lo,ly,hy,ho,hr) = msg

        # Time handling here?
        (tb, tc, ts, tus) = t

        dt_object = datetime.datetime.fromtimestamp(ts)
        dt_object = dt_object + datetime.timedelta(microseconds=tus)

        self.__tlm = {'ch_name':name , 'ch_value':ch_value, 'datetime_object':dt_object}

        self.notifyObservers(self.__tlm)

        self.__tlm = None

    def update_severity(self, severity):
        """
        Called from event listener.
        Gives mediator an event severity to give to stripchart panel.
        """

        self.__severity = severity
        self.notifyObservers(self.__severity)

        self.__severity = None
    def update_events(self, event_name):
        """
        Called from event listener.
        Gives mediator an event name to give to stripchart panel.
        """
        self.__event_name = event_name
        self.notifyObservers(self.__event_name)

        self.__event_name = None

    def get_tlm(self):
        return self.__tlm
    def get_event_name(self):
        return self.__event_name
    def get_severity(self):
        return self.__severity


    def getInstance():
        """
        Return instance of singleton.
        """
        if(StripChartListener.__instance is None):
            StripChartListener.__instance = StripChartListener()
        return StripChartListener.__instance

    #define static method
    getInstance = staticmethod(getInstance)
