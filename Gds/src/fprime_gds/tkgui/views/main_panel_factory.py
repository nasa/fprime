#!/bin/env python
#===============================================================================
# NAME: MainPanelFactory.py
#
# DESCRIPTION: This is a factory class for instancing the main GSE panel.
#                It exists as a singlton and also destroys the toplevel
#                windows as well.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Jan. 11, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#

import os
import sys
import glob
import time
import socket
import logging
import exceptions
import Tkinter
import Pmw
import subprocess


from fprime_gds.tkgui.views import main_panel
from fprime_gds.tkgui.views import command_panel
from fprime_gds.tkgui.views import log_event_panel
from fprime_gds.tkgui.views import telemetry_panel
from fprime_gds.tkgui.views import seq_panel
from fprime_gds.tkgui.views import status_panel
from fprime_gds.tkgui.views import status_bar
from fprime_gds.tkgui.views import file_panel
from fprime_gds.tkgui.views import stripchart_panel
from fprime_gds.tkgui.views import help_panel

from fprime_gds.tkgui.controllers import command_loader
from fprime_gds.tkgui.controllers import commander
from fprime_gds.tkgui.controllers import event_loader
from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import channel_loader
from fprime_gds.tkgui.controllers import channel_listener
from fprime_gds.tkgui.controllers import packetize_channel_loader
from fprime_gds.tkgui.controllers import packetize_channel_listener
from fprime_gds.tkgui.controllers import status_bar_updater
from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.tkgui.controllers import file_listener
from fprime_gds.tkgui.controllers import stripchart_listener
from fprime_gds.tkgui.controllers import socket_listener

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

class MainPanelFactory:
    """
    This is a concrete factory method object used to create the
    main UI panel instances for the GSE application.  It is
    also used to destroy and clean up mediator and observer
    associations.
    """
    __parsed   = None
    __instance = None


    def __init__(self, opts):
        """
        Private Constructor (singleton pattern)
        """
        self.__parsed   = None
        self.__instance = None
        self.__window_instance = 0
        self.__configured_visitors = dict()
        self.__opts = opts
        #
        self.__main_panels = dict()
    def getInstance():
        """
        Return instance of singleton.
        """
        if(MainPanelFactory.__instance is None) :
            MainPanelFactory.__instance = MainPanelFactory()

        return MainPanelFactory.__instance


    #define static method
    getInstance = staticmethod(getInstance)


    def getOpts(self):
        """
        Return Options Parsed.
        """
        return self.__opts


    def create(self, root, top=True):
        """
        Create a main panel here.
        """
        #
        # Load and initialize commands here.
        # Read in generated command modules here and instance Command objects
        #
        cmdl = command_loader.CommandLoader.getInstance()
        #
        # Load and initialize event log messages here.
        #
        eventl = event_loader.EventLoader.getInstance()
        #
        # Load and initialize channel telemetry here.
        #
        channell = channel_loader.ChannelLoader.getInstance()
        #
        # Load and initialize packetized telemetry
        #
        pkt_channell = packetize_channel_loader.PacketizeChannelLoader().getInstance()
        #
        # Read all the command/event/channel telemetry, etc.
        if top:
            try:
                # add path for serializables
                sys.path.append(self.__opts.generated_path + os.sep + "serializable")

                sys.path.append(self.__opts.generated_path)
                generated_path1 = self.__opts.generated_path + os.sep + "commands"
                cmdl.create(generated_path1)
                generated_path2 = self.__opts.generated_path + os.sep + "events"
                eventl.create(generated_path2)
                generated_path3 = self.__opts.generated_path + os.sep + "channels"
                channell.create(generated_path3)
                pkt_channell.create(self.__opts.packetspec)

            except exceptions.GseControllerUndefinedDirectoryException, e:
                sys.stderr.write(e.getMsg())
                sys.stderr.write("\n\nPlease specify the correct dictionary location")
                sys.stderr.write("\nvia the command line or a gse.ini file.\n\n")

                gse_path = None
                if 'BUILD_ROOT' in os.environ:
                    gse_path = os.environ['BUILD_ROOT'] + os.sep + "Gse" + os.sep + "bin" + os.sep + "gse.py"
                elif './bin' in glob.glob("./*"):
                    gse_path = "./bin/gse.py"
                elif "../bin" in glob.glob("../*"):
                    gse_path = "../bin/gse.py"
                elif "./Gse" in glob.glob("./*"):
                    gse_path = "./Gse/bin/gse.py"

                if gse_path:
                    cmd = "python {} --help".format(gse_path)
                    os.system(cmd)

                sys.exit(-1)

        #
        the_panel = main_panel.TopPanel(root, self.__opts, self.__window_instance, top=top)
        the_panel.configureStyle(self.__opts)
        # Register this call with the main panel so new instances can be created.
        the_panel.register(self)

        # Hold references to main panels here
        self.__main_panels[self.__window_instance] = the_panel

        #
        # Populate each panel with widgets for the instance.
        #

        #
        # Create status bar and updater
        #
        stat_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()
        stat_bar = status_bar.StatusBar(root, bg="lightblue")
        # Start refresh loop
        root.after(100, stat_bar.refresh())
        # Register with main panel
        the_panel.register_panel(stat_bar)
        the_panel.register_refreshable(stat_bar)

        # Register help balloon with top
        if top == True:
            stat_bar_updater.register_balloon(root)

        cmd_panel = command_panel.CommandPanel(the_panel.p1(), the_panel, self.__opts)

        the_panel.register_panel(cmd_panel)
        #
        logger_panel = log_event_panel.LogEventPanel(the_panel.p2(), the_panel, self.__opts)
        # Start refresh loop
        root.after(100, logger_panel.refresh())
        the_panel.register_panel(logger_panel)
        the_panel.register_refreshable(logger_panel)

        telm_panel = telemetry_panel.TelemetryPanel(the_panel.p3(), the_panel, self.__opts)
        # Start refresh loop
        root.after(100, telm_panel.refresh())
        the_panel.register_panel(telm_panel)
        the_panel.register_refreshable(telm_panel)
        #
        sequencer_panel = seq_panel.SeqPanel(the_panel.p4(), the_panel)
        the_panel.register_seq_panel(sequencer_panel)

        #
        # Create a socket listener to read data from the socket,
        # and pass to the event and telemetry listeners
        #
        socket_listen = socket_listener.SocketListener.getInstance()
        #
        # This will set up logging of the raw data from the socket if
        # command line flag is set.
        #
        if top == True:
            socket_listen.register_main_panel(the_panel)
            socket_listen.setupBinaryLogging(self.__opts)

        #
        # Create an event_listener thread and periodic updater here.
        # This will receive messages and update whatever panels are
        # instanced.
        #
        ev_listen = event_listener.EventListener.getInstance()
        ev_listen.addObserver(logger_panel)
        if top == True:
            ev_listen.setupLogging(self.__opts)

        #
        # Create a channelized telemetry listener here. This uses
        # the event_listener thread so when messages are channel
        # telemetry this decodes them and sends the notify to the UI.
        #
        ch_listen = channel_listener.ChannelListener.getInstance()
        ch_listen.addObserver(telm_panel)
        #
        if top == True:
            ch_listen.setupLogging(self.__opts)
            ch_listen.setupCsvLogging(self.__opts)

        #
        # Create status panel
        #
        stat_panel = status_panel.StatusPanel(the_panel.p8(), the_panel)
        the_panel.register_panel(stat_panel)

        #
        # Get / create a status_updater singlton/observer to
        # update all status text areas of all instanced panels.
        #
        the_updater = status_updater.StatusUpdater.getInstance()
        the_updater.addObserver(stat_panel)
        if top:
            the_updater.setupLogging(self.__opts)
        #
        # Create File Panel
        #
        f_panel    = file_panel.FilePanel(the_panel.p10(), the_panel, self.__opts)
        f_panel.set_default_upload_folder()
        the_panel.register_panel(f_panel)
        # Add observers to file listener and start listener
        f_listener = file_listener.FileListener.getInstance()
        f_listener.addObserver(f_panel)
        if top:
            f_listener.set_root(root)
            id = root.after(100, f_listener.update_task)
            f_listener.set_after_id(id)

        #
        # Create Stripchart panel
        #
        sc_listener = stripchart_listener.StripChartListener.getInstance()
        strip_panel  = stripchart_panel.StripChartPanel(the_panel.p7(), the_panel, self.__opts)
        root.after(100, strip_panel.refresh)
        the_panel.register_refreshable(strip_panel)
        sc_listener.addObserver(strip_panel)
        the_panel.register_panel(strip_panel)

        #
        # Create Help Panel
        #

        h_panel= help_panel.HelpPanel(the_panel.p9(), the_panel)
        the_panel.register_panel(h_panel)

        #
        # Assign commands here
        #
        cmd_panel.setCmdList(cmdl.getCommandDict().keys())
        sequencer_panel.setCmdList(cmdl.getCommandDict().keys())

        #
        # Connect cmd_panel to mediator command
        #
        cmdr = commander.Commander.getInstance()
        cmdr.register(cmd_panel, cmdr.cmd_send)

        #
        # Register the root main_panel so we can get the socket object handle
        #
        if top == True:
            cmdr.register_main_panel(the_panel)

        #
        # If enabled try and connect to ThreadedTCPSocketServer here...
        #
        if self.__opts.connect == True and top == True:
            the_panel.connectTCP()

        self.__window_instance += 1
        return  the_panel

    def childClosing(self):
        """
        Window is closing.
        Remove main_panel reference so that
        main_panels is always reflective of current state.
        """
        self.__window_instance -= 1
        self.__main_panels.pop(self.__window_instance, None)

    def id(self):
        return self.__window_instance

    def get_panel_dict(self):
        return self.__main_panels


def main():
    """
    Test of main panel instancing and destroy capability here
    """
    root = Tkinter.Tk()
    #root.option_readfile('optionDB')
    root.title('F Prime Ground Support Equipment')
    Pmw.initialise()
    mpf = MainPanelFactory()
    mpf.create(root)

    # This is a quick fix since something
    # locks the main window event loop.
    w = Tkinter.Toplevel()
    w.destroy()

    # MAIN BODY #
    root.geometry("900x900+10+10")
    root.mainloop()


if __name__ == '__main__':
    main()
