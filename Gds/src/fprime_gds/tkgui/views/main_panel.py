#!/bin/env python
#===============================================================================
# NAME: TopPanel.py
#
# DESCRIPTION: This module contains classes for the toplevel window
#              creation and instantiation of the GSE GUI.
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
import os
import sys
import time
import socket
import logging
import Tkinter
import tkFileDialog
import Pmw
import exceptions
import subprocess
import signal

import log_event_panel
import command_panel
import telemetry_panel
import status_panel
import status_bar
import file_panel

from fprime_gds.tkgui.controllers import client_sock
from fprime_gds.tkgui.controllers import socket_listener
from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import channel_listener
from fprime_gds.tkgui.controllers import packetize_channel_listener
from fprime_gds.tkgui.controllers import exceptions
from fprime_gds.tkgui.controllers import status_updater
from fprime_gds.tkgui.controllers import status_bar_updater
from fprime_gds.tkgui.controllers import file_listener

from fprime_gds.tkgui.utils import ConfigManager
from fprime_gds.tkgui.utils.tkGui_persist import WindowMementoOriginator, MementoCaretaker

from Canvas import Line

@WindowMementoOriginator
class TopPanel(object):
    '''
    Instances the top-level menu bar, notebook, status bar
    '''
    def __init__(self, parent, opts, wid, top=False):
        '''
        Constructor
        '''
        self._parent = parent
        self.__main_panel_factory = None
        self.__display = None
        self.__opts = opts
        self.__top = top
        self.__config = ConfigManager.ConfigManager()
        self.__memento_caretaker = MementoCaretaker.getInstance()
        if top:
            self.__memento_caretaker.set_root(parent)
        #
        # The notebook widget
        self.__nb = None
        #
        if self._parent == None:
            return
        #
        self.build_menu(parent)
        self.build_notebook(parent)
        #
        # Window id (0 is root)
        self.__id = wid

        # Constrain size on window so geometry works.
        min_x = self.__config.get("gui", "window_min_x")
        min_y = self.__config.get("gui", "window_min_y")
        parent.minsize(min_x, min_y)

        self.__sock = None
        #
        # List of sub-panels for destruction
        self.__panels = []
        #
        self.__refreshable_panels = []

        # Socket listener
        self.__socket_listen = socket_listener.SocketListener.getInstance()

        # Event listener singleton with observer
        self.__event_listen = event_listener.EventListener.getInstance()
        # Start listener thread
        self.__event_listen.start_thread()

        # Channel listener singleton with observer
        self.__ch_listen = channel_listener.ChannelListener.getInstance()
        # Start listener thread
        self.__ch_listen.start_thread()

        # Channel listener singleton with observer
        self.__pkt_ch_listen = packetize_channel_listener.PacketizeChannelListener.getInstance()
        # Start listener thread
        self.__pkt_ch_listen.start_thread()

        # Status updater singleton
        self.__stat_bar_updater = status_bar_updater.StatusBarUpdater.getInstance()
        # Start updater thread
        self.__stat_bar_updater.start_thread()

        self.__status_update = status_updater.StatusUpdater.getInstance()

        # File listener singleton
        self.__file_listener = file_listener.FileListener.getInstance()

        # When window is closed handle the event with exit stuff
        self._parent.protocol("WM_DELETE_WINDOW", self.exit)
        #
        # The sequencer panel is registered so menu can access functions.
        self.__seq_panel = None

    def root(self):
        return self._parent

    def register_panel(self,p):
        self.__panels.append(p)

    def register_refreshable(self, p):
        self.__refreshable_panels.append(p)

    def configureStyle(self, options=None):
        """
        Configure title and geometry
        """
        if options != None: # use options title
            base = "%s Ground System"%options.title
        else:
            base = 'F Prime Ground Software'

        if self.__id > 0:
            base = base + '(%d)' % self.__id

        self._parent.title(base)
        g = "900x900+%d+%d" % (self.__id * 10, self.__id * 10) #900,900
        self._parent.geometry(g)
        self._parent.update()

    def register_seq_panel(self, seq_panel):
        """
        This registers a seq panel for menu operations.
        """
        self.__seq_panel = seq_panel

    def build_menu(self, parent):
        '''
        Create menu bar and items
        '''
        balloon = Pmw.Balloon(parent)
        #@todo: Look at replacing with Pmw.MainMenuBar
        menuBar = Pmw.MenuBar(parent, hull_relief=Tkinter.RAISED, hull_borderwidth=1, balloon=balloon)
        menuBar.pack(fill=Tkinter.X)
        #menuBar.grid(sticky=Tkinter.N)

        m=menuBar.addmenu('File', 'New..., Save..., Load..., Save Windows..., Restore Windows..., About...')
        menuBar.addmenuitem('File', 'command', 'Create a new main window.',
                    font=('StingerLight', 14), label='New...', command=self.new)
        menuBar.addmenuitem('File', 'command', 'Save to file cooresponding to selected notebook pane.',
                    font=('StingerLight', 14), label='Save...', command=self.save)
        menuBar.addmenuitem('File', 'command', 'Load to file cooresponding to selected notebook pane.',
                    font=('StingerLight', 14), label='Load...', command=self.load)
        menuBar.addmenuitem('File', 'command', 'Program Information.',
                    font=('StingerLight', 14), label='About...', command=self.about)
        menuBar.addmenuitem('File', 'command', 'Save Window Configuration...',
                label='Save Window Configuration', font=('StingerLight', 14), command=self.save_window_configuration)
        menuBar.addmenuitem('File', 'command', 'Restore Windows...',
                    label='Restore Windows', font=('StingerLight', 14), command=self.restore_windows)
        menuBar.addmenuitem('File', 'separator')
        menuBar.addmenuitem('File', 'command',
                    'Exit the application', label='Exit', command=self.exit)

        if self.__top == True:
            m2=menuBar.addmenu('TCP Server', 'Launch..., Connect..., Kill..')
            menuBar.addmenuitem('TCP Server', 'command', 'Start a TCP Threaded Socket Server in terminal',
                                font=('StingerLight', 14), label='Launch...', command=self.startTCP)
            menuBar.addmenuitem('TCP Server', 'command', 'Connect To Socket Server',
                                font=('StingerLight', 14), label='Connect...', command=self.connectTCP)
            menuBar.addmenuitem('TCP Server', 'command', 'Kill TCP Threaded Socket Server and terminal',
                                font=('StingerLight', 14), label='Kill...', command=self.killTCP)

    # Menu callbacks
    def new(self):
        """
        Create new window.
        - Break from event listener
        - Let remaining tasks finish then create new window
        """
        self.break_update_tasks()
        self._parent.after(100, self.__handle_new)

    def __handle_new(self):
        """
        Generate a new instance of the panel.
        """
        top = Tkinter.Toplevel()
        #id = self.__main_panel_factory.id()

        self.__main_panel_factory.create(top, False)

        # Start update tasks again
        self.restart_update_tasks()

    def about(self):

        a = Pmw.AboutDialog(self._parent, applicationname=self.__config.get('about_info', 'app_name'))
        a.show()

    def restore_windows(self):
        """
        Tell caretaker to restore windows.
        """
        self.__memento_caretaker.load_from_file()
        self.__memento_caretaker.restoreWindows(self.__main_panel_factory)

    def save_window_configuration(self):
        """
        Iterate through main panel references and save their state.
        """
        for main_panel in self.__main_panel_factory.get_panel_dict().values():
            main_panel.create_memento()

            #for panel in main_panel.__panels:
            #    try:
            #        panel.create_memento()
            #    except AttributeError, e:
            #        pass


        self.__memento_caretaker.save_to_file()

    def save(self):
        """
        Save various things depending on active panel implemented.
        """
        # Get base default path
        p = os.environ['HOME'] + os.sep + 'fprime' + os.sep + 'sequences'
        if not os.path.exists(p):
            os.makedirs(p)
        #
        # Various file save as dialogs and save method calls.
        pname = self.notebook().getcurselection()
        if pname == "Sequences":
            f = tkFileDialog.asksaveasfilename(defaultextension =".txt", \
                                           filetypes =[(".txt","*"), (".bin","*")], \
                                           initialdir = p, \
                                           initialfile = "sequence", \
                                           title = "Save a sequence file (ether .txt, or .bin supported)")
            # f is None if user cancelled save
            if not f:
                return


            e = os.path.splitext(f)[1][1:].strip()
            if e == 'txt':
                t = "ascii"
            elif e == "bin":
                t = "binary"
            else:
                self.__status_update.update("Unrecognized file type: %s" % e, 'red')
            #
            self.__seq_panel.save(f, t)
            #
            self.__status_update.update("Saved sequencer file: %s" % f)
        else:
            self.__status_update.update("Save functionality not yet implemented for panel: %s" % pname, 'red')


    def load(self):
        print "Load..."
        self.__status_update.update("Load...")

    def break_update_tasks(self):
        """
        - Break and cancel file update_task
        """
        self.__file_listener.update_task_cancel()

    def restart_update_tasks(self):
        """
        Start new update_task call
        """
        after_id = self._parent.after(100, self.__file_listener.update_task)
        self.__file_listener.set_after_id(after_id)

    def exit(self):
        """
        Exit window.
        - Break from event listener's update_task
        - Cancel the next update_task call
        - Let remaining tasks finish then proceed to exit
        """

        for panel in self.__refreshable_panels:
            panel.refresh_cancel()

        self.break_update_tasks()

        # If top level thread kill consumers
        if self.__top:
          self.__stat_bar_updater.stop_thread()
          self.__event_listen.stop_thread()
          self.__ch_listen.stop_thread()
          self.__pkt_ch_listen.stop_thread()

        self._parent.after(500, self.__handle_exit)


    def __handle_exit(self):


        print "Exit..."
        user = os.environ['USER']
        op_sys = os.uname()[0]

        if self.__opts.exec_app != None:
            if op_sys == "Darwin":
               process_str = ["ps", "-ef", "|", "grep " + self.__opts.exec_app]
               p = subprocess.check_output(process_str, shell=True)
               p = p.split('\n')
               pid = ''

               for line in p:
                  if((self.__opts.exec_app) in line) and not ('gse.py' in line):
                     pid = line
               pid = pid.split()[0]
               os.kill(int(pid), signal.SIGINT)
            else:
               process_str = ["ps","-ef"]
               p = subprocess.Popen(process_str, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
               o, e = p.communicate()
               pid = ''
               lines = o.split('\n')
               for l in lines:
                   if (user in l) and ('Ref' in l) and ('tee' in l):
                      pid = l.split()[1]
               os.kill(int(pid), signal.SIGINT)

        # Only close bin logfile and quit socket connection if top
        if self.__top:
            self.__socket_listen.close_bin_logfile()
            # Cancel next update task
            self.break_update_tasks()

            if self.__opts.connect == True or self.__opts.exec_app != None:
                if self.__sock != None:
                    self.__sock.send("Quit\n")

            self.__file_listener.exit()


        for p in self.__panels:
            if isinstance(p, log_event_panel.LogEventPanel):
                self.__event_listen.deleteObserver(p)
            elif isinstance(p, command_panel.CommandPanel):
                #@todo: mediator clean up needed here.
                pass
                #p.create_memento("command_panel")
            elif isinstance(p, telemetry_panel.TelemetryPanel):
                self.__ch_listen.deleteObserver(p)
            elif isinstance(p, status_panel.StatusPanel):
                self.__status_update.deleteObserver(p)
            elif isinstance(p, status_bar.StatusBar):
                self.__stat_bar_updater.notify_exit()
            elif isinstance(p, file_panel.FilePanel):
                self.__file_listener.deleteObserver(p)
                self.__file_listener.exit()
            else:
                p.__del__()


        #self.create_memento(self.__id)

        #
        self.__main_panel_factory.childClosing()
        #
        self._parent.destroy()

        # If window is not root restart event listener update task
        if not self.__top:
            self.restart_update_tasks()

    # TCP Server menu callbacks
    def startTCP(self):
        """
        Execute the TCP Socket Server in a shell...
        """
        return
        #
        # Setup log file name here...
        #
        if self.__opts == None:
            p = os.environ['HOME'] + os.sep + 'logs' + os.sep + "threaded_tcp_server"
        else:
            p = self.__opts.log_file_path + os.sep + "threaded_tcp_server"
        #
        if not os.path.exists(p):
            os.makedirs(p)
        #
        logfile = p + os.sep + time.strftime("%y%m%d%H%M%S", time.gmtime()) + '_SockectServer.log'
        #
        op_sys = os.uname()[0]
        if 'BUILD_ROOT' in os.environ:
            root_path = os.environ['BUILD_ROOT']
        else:
            print "EXCEPTIONS:  BUILD_ROOT Environment Variable not found!"
            raise exceptions.EnvironmentError

        if op_sys == 'Darwin':
            app_script = '/usr/bin/osascript '
            app_args   = '-e \'tell application \"Terminal" to do script '
            app_cmd    = "\"cd %s/Gse/bin; ./ThreadedTCPServer.py -p %s | tee -a %s; exit\"" % (root_path, self.__opts.port, logfile)
            # Note the exit is so Mac Terminal.app window will exit when process killed...
            cmd = app_script + app_args + app_cmd + '\''
            os.system(cmd)
        else:
            app_script = "/usr/bin/gnome-terminal"
            app_args   = " -x bash -c "
            app_cmd    = "\"source ~/.bashrc; cd %s/Gse/bin; ./ThreadedTCPServer.py -p %s | tee -a %s; exit\"" % (root_path, self.__opts.port, logfile)
            # Note the .bashrc must be sourced.
            cmd = app_script + app_args + app_cmd
            my_env = os.environ
            print cmd
            #@todo:  The environment is not being imported???
            #subprocess.call(cmd, shell=True, env=my_env)
            subprocess.Popen(cmd, shell=True, env=my_env)
        #
        if self._parent != None:
            self.__status_update.update(cmd, "red")
            self.__status_update.update("ThreadedTCPServer.py Started...", color='red')


    def connectTCP(self):
        """
        Connect to a running ThreadedTCPServer here...
        """
        # connect to server
        try:
            port = self.__opts.port
            server = self.__opts.addr
            s = "Connected to server (host addr %s, port %d)" % (server, port)
            self.__sock=client_sock.ClientSocket(server,port)
            self.__status_update.update(s, 'red')
            #
            # Register the GUI with the server
            #self.lock.acquire()
            self.__sock.send("Register GUI\n")
            #self.lock.release()
            #
            # Register the socket with the socket_listener and
            # Spawn the listener thread here....
            self.__socket_listen.connect(self.__sock)
        except IOError:
            del(self.__sock)
            self.__sock = None
            s = "EXCEPTION: Could not connect to socket at host addr %s, port %s" % (server, port)
            print s
            self.__status_update.update(s,color='red')


    def getSock(self):
        """
        Return socket client object handle.
        """
        return self.__sock


    def killTCP(self):
        """
        Kill the TCP Socket Server.
        """
        #@todo: This does not quite work yet.  Make it shutdown and stop listener thread.
        self.__sock.send("Quit\n")
        #
        if self.__sock != None:
            del(self.__sock)
            self.__sock = None


    def build_notebook(self,parent):
        '''
        Create notebook tabs and panes
        '''
        self.__nb = Pmw.NoteBook(parent)
        nb = self.__nb
        self.__p1 = nb.add('Commands')
        self.__p2 = nb.add('Log Events')
        self.__p3 = nb.add('Channel Telemetry')
        self.__p4 = nb.add('Sequences')
        self.__p7 = nb.add('Stripchart')
        self.__p10= nb.add('File Management')
        self.__p8 = nb.add('Status')
        self.__p9 = nb.add('Help')
        nb.pack(side=Tkinter.TOP, padx=5, pady=5, expand=Tkinter.YES, fill=Tkinter.BOTH)
        #nb.grid(row=1, rowspan=2)
        nb.config(bg='lightblue')
        return

    def notebook(self):
        return self.__nb

    def p1(self):
        return self.__p1

    def p2(self):
        return self.__p2

    def p3(self):
        return self.__p3

    def p4(self):
        return self.__p4

    def p7(self):
        return self.__p7

    def p8(self):
        return self.__p8

    def p9(self):
        return self.__p9

    def p10(self):
        return self.__p10

    def register(self, main_panel_factory):
        """
        """
        self.__main_panel_factory = main_panel_factory
    def get_panels(self):
        return self.__panels

    def id(self):
        return self.__id

def main_window_start():
    """
    """
    root = Tkinter.Tk()
    #root.option_readfile('optionDB')
    root.title('F Prime Ground Support Equipment')
    Pmw.initialise()
    t=TopPanel(root)
    c=command_panel.CommandPanel(t.p1())
    return root


if __name__ == "__main__":
    root = main_window_start()
    root.geometry("900x900+10+10")
    root.mainloop()
