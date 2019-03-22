#!/bin/env python
#===============================================================================
# NAME: Commander.py
#
# DESCRIPTION: This module contains a set of classes to mediate the
#              command objects generated from each command_panel
#              instance and serialize them into useful commands for
#              socket transfer.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: Dec 9, 2014
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import collections
from fprime.common.models.serialize import type_base
from fprime.common.models.serialize import u32_type
from fprime_gds.tkgui.controllers import status_bar_updater
#from fprime_gds.gse.views import main_panel

class Commander:
    """
    A singleton mediator class that serves as
    both factory and registration for a mediator.
    A decorator is used to add mediated on_change
    when the command_panel "Send" button is clicked.
    """
    __instance = None

    def __init__(self):
        """
        Constructor.
        """
        self.create_mediator()
        self.__the_main_panel = None

        # Update the status bar whenever packets are sent
        self.__status_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()


    def getInstance():
        """
        Return instance of singleton.
        """
        if(Commander.__instance is None):
            Commander.__instance = Commander()
        return Commander.__instance

    #define static method
    getInstance = staticmethod(getInstance)


    def create_mediator(self):
        self.mediator = Mediator()


    def register(self, widget, caller):
        """
        Register widget with caller.
        Note Caller will always be cmd_send.
        """
        self.mediator.register(widget, caller)
        return widget


    def unregister(self, widget):
        """
        When a command_panel is destroyed
        this is executed to remove it.
        """
        self.mediator.unregister(widget)


    def register_main_panel(self, the_main_panel):
        """
        Register the main panel to get socket handle
        if one is set.  This is an indication of connection.
        """
        self.__the_main_panel = the_main_panel


    def cmd_send(self, widget, cmd_obj):
        #print "Command serialized: %s (0x%x)" % (cmd_obj.getMnemonic(), cmd_obj.getOpCode())
        data = cmd_obj.serialize()
        #type_base.showBytes(data)
        #
        # Package and send immediate command here...
        # A5A5 <Target Name> <command descriptor 0x5A5A5A5A>
        # <4 Bytes of command packet size, data only>
        # <command opcode data> <command args data> <checksum-tbd>
        desc = u32_type.U32Type( 0x5A5A5A5A )
        #
        # Added desc. type for FSW to know it is a command (0).
        desc_type = u32_type.U32Type(0)
        #
        data_len = u32_type.U32Type( len(data) + desc_type.getSize() )
        #
        cmd = "A5A5 " + "FSW " + desc.serialize() + data_len.serialize() + desc_type.serialize() + data
        #type_base.showBytes(cmd)
        sock = self.__the_main_panel.getSock()
        if sock != None:
            # Format (data_recv, data_sent)
            self.__status_bar_updater.put_data((0, len(cmd)))
            sock.send(cmd)
        else:
            self.__the_main_panel.statusUpdate("No socket connection for FSW commands", "red")


class Mediator(object):

    def __init__(self):
        self.callablesForWidget = collections.defaultdict(list)

    def register(self, widget, caller):
        """
        Register a new input object with caller.
        """
        self.callablesForWidget[widget].append(caller)
        widget.mediator = self
        return widget


    def unregister(self, widget):
        """
        Remove widget from dictionary.
        """
        if widget in self.callablesForWidget: self.callablesForWidget[widget]


    def on_change(self, widget, cmd_obj):
        callables = self.callablesForWidget.get(widget)
        if callables is not None:
            for caller in callables:
                caller(widget, cmd_obj)
        else:
            raise AttributeError("No on_change() method registered for {}"
                    .format(widget))

#
# adding the on_change call.
def mediated(Class):
    setattr(Class, "mediator", None)
    def on_change(self, cmd_obj):
        #print "mediator on_change"
        if self.mediator is not None:
            self.mediator.on_change(self, cmd_obj)
    setattr(Class, "on_change", on_change)
    return Class

if __name__ == "__main__":
    pass
