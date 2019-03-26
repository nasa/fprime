#!/bin/env python
#===============================================================================
# NAME: GseApi.py
#
# DESCRIPTION: A basic API of command and telemetry monitoring capabilities.
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: June 17, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
from __future__ import print_function
import sys
import os
import logging
import time
import glob
import signal
import subprocess
from subprocess import PIPE, STDOUT

from fprime_gds.tkgui.utils import Logger
from fprime_gds.tkgui.utils import command_args
from checksum import *
from fprime.common.models.serialize.u32_type import *

from fprime_gds.tkgui.controllers import command_loader
from fprime_gds.tkgui.controllers import commander
from fprime_gds.tkgui.controllers import event_loader
from fprime_gds.tkgui.controllers import event_listener
from fprime_gds.tkgui.controllers import channel_loader
from fprime_gds.tkgui.controllers import packetize_channel_loader
from fprime_gds.tkgui.controllers import channel_listener
from fprime_gds.tkgui.controllers import packetize_channel_listener
from fprime_gds.tkgui.controllers import client_sock
from fprime_gds.tkgui.controllers import socket_listener
from fprime_gds.tkgui.controllers import file_uplink_client
from fprime_gds.tkgui.controllers import file_downlink_client
from fprime_gds.tkgui.controllers.file_downlink_client import DownlinkStatus
from fprime_gds.tkgui.controllers.file_uplink_client import UplinkStatus

from ConfigManager import ConfigManager
from ConfigParser import NoOptionError

class GseApi(object):
    """
    This class is a general API into the gse.py graphical functionality
    but is indepent of any GUI package.  It is used to build test applicaitons
    for commanding and listening to event and channel telemetry.

    This class will be used to build three command line applicaitons which
    are:

    gse_send(...) to send a command.
    gse_send_and_wait(...) to send command and wait for log event or channel telemetry value.
    gse_monitor(...) to monitor for log event messages or channel telemetry values.
    If blocking then block and update console and log file, if not blocking then
    poll and if message then return it and update console and log file.
    """

    def __init__(self, server_addr='127.0.0.1', port=50000, generated_path='', packet_spec_path='', build_root='', log_file_cmds=None, log_file_events=None, log_file_channel=None, log_file_path=None, log_file_updown=None, listener_enabled=False, verbose=False, quiet=False):
        """
        Constructor.
        @param server_addr: Socket server addr
        @param port: Socket server port number
        @param packet_spec_path: path to packet definition XML
        @param log_file_cmds: Name of command log file
        @param log_file_events: Name of log event log file
        @param log_file_channel: Name of channel telemetry log file
        @param log_file_path: Name of common directory path to save log files into
        @param listener_enabled: If True then event/channel telemetry listener thread enabled, else dissabled
        @param verbose: Enable diagonistic display of information
        """
        # 1. Connect to socket server using controllers.client_sock.py
        # 2. Create log files and loggers
        # 3. Load configuration for commands, events, channels, using controllers.XXXXX_loader.py
        # 3. Start listener thread controllers.event_listern.py
        self.quiet = quiet
        # For every console output log to a default file and stdout.
        logfile = time.strftime("%y-%m-%d-%H-%M-%S", time.gmtime()) + '_GSEStdOut.log'
        #p = opts.log_file_path + os.sep + "stdout"
        p = '.' + os.sep + "stdout"
        if not os.path.exists(p):
            os.makedirs(p)
        f = p + os.sep + logfile

        # setup default for generated python command / telemetry descriptor files.
        # these are autocoded descriptions of all.

        config = ConfigManager.getInstance()
        self.print_msg("Searching for standard configuration")
        #Updated unified configuration settings
        self.build_root = self.get_configured_setting(build_root, config, "filepaths", "build_root")
        self.generated_path = self.get_configured_setting(generated_path, config, "filepaths", "generated_path")
        #Packet spec path not strictly required, thus removing it
        try:
            self.packet_spec_path = self.get_configured_setting(packet_spec_path, config, "filepaths", "packet_path")
        except NoOptionError:
            self.packet_spec_path = None
        # display configuration before starting GUI here...
        sep_line = 80*"="
        if verbose:
            logger = Logger.connectOutputLogger(f,'stdout', logging.INFO)
        else:
            logger = Logger.connectOutputLogger(f,'stdout', logging.WARNING)
        logger.info("Created log: %s" % f)
        logger.info("User: %s" % os.environ['USER'])
        (sysname, nodename, release, version, machine) = os.uname()
        logger.info("OS Name: %s" % sysname)
        logger.info("Machine Network Host Name: %s" % nodename)
        logger.info("Release: %s" % release)
        logger.info("Version: %s" % version)
        logger.info("Machine: %s" % machine)
        logger.info(sep_line)

        # load commands, events, channelized telemetry, and event listener
        sys.path.append(generated_path)
        # add path for serializables
        sys.path.append(generated_path + os.sep + "serializable")

        self._cmds = command_loader.CommandLoader.getInstance()
        self._cmds.create(generated_path + os.sep + "commands")
        self._events = event_loader.EventLoader.getInstance()
        self._events.create(generated_path + os.sep + "events")
        self._channels = channel_loader.ChannelLoader.getInstance()
        self._channels.create(generated_path + os.sep + "channels")
        if not self.packet_spec_path is None and self.packet_spec_path != "":
            self._pkt_channels = packetize_channel_loader.PacketizeChannelLoader.getInstance()
            self._pkt_channels.create(self.packet_spec_path)
        self._ev_listener = event_listener.EventListener.getInstance()
        self._ev_listener.setupLogging()
        self._ch_listener = channel_listener.ChannelListener.getInstance()
        self._ch_listener.setupLogging()
        self._pk_listener = packetize_channel_listener.PacketizeChannelListener.getInstance()
        #self._pk_listener.setupLogging()
        self.__sock_listener = socket_listener.SocketListener.getInstance()
        self.__logger = logger

        self.__server_addr = server_addr
        self.__port        = port

        # Uplink and downlink clients will log to null handler if none is specified
        file_uplink_client.set_logger(log_folder=log_file_updown)
        file_downlink_client.set_logger(log_folder=log_file_updown)

        # connect to TCP server
        try:
          self.__sock = client_sock.ClientSocket(server_addr, port)
          self.__sock.send("Register GUI\n")
          self.__sock_listener.connect(self.__sock)
        except IOError:
          self.__sock = None

        super(GseApi, self).__init__()

    def get_configured_setting(self, cmdline, config, group, key, environment=None):
        '''
        Get a configuration setting from the gse.ini file or from the environment.
        @param cmdline: value passed in from command line
        @param config: configuration dictionary passed in
        @param group: group in configuration file
        @param key: configuration key
        @param environment: environment varable, if not key.upper
        '''
        #Default the environment key
        if environment is None:
            environment = key.upper()
        #Get configuration in order of precedence
        # 1. Command line
        # 2. Environment variable (most)
        # 2. gse.ini
        try:
            if not cmdline is None and cmdline != "":
                return cmdline
            elif environment in os.environ:
                return os.environ[environment]
            value = config.get(group, key)
            if value != "" and not value is None:
                return config.get(group, key)
        except NoOptionError:
            self.print_msg("WARNING: {0} not set. Specify on the command line,".format(key) +
                           "in the environment, or in gse.ini.",
                           file=sys.stderr)
            #Re throw the problem
            raise

    def print_msg(self, message, file=sys.stdout):
        '''
        Print a message subject to the quietness level
        '''
        if not self.quiet or file == sys.stderr:
            print(message, file=file)
    
    def disconnect(self):
        '''
        Disconnect form the socket
        '''
        self.__sock.disconnect()

    class TimeoutException(Exception):
        pass

    def _timeout_sig_handler(self, signum, frame):
        raise self.TimeoutException()

    def __ctrl_c_sig_handler(self, signum, frame):
        raise Exception('Ctrl-C Received, Exiting.')

    def __loop_queue(self, id, type, timeout=None):
        """
        Grabs all telemetry and data in event listener's queue until the queried event / tlm id is found.
        Returns a tuple with two lists (tlm_list,evr_list)
        """
        
        tlm_list = []
        evr_list = []
        recv_id= ''
        if timeout:
          signal.signal(signal.SIGALRM, self._timeout_sig_handler)
          #Note: signal.alarm only works on seconds, interval timers
          #      can use a float number of seconds, consistent with
          #      python's time.time()
          signal.setitimer(signal.ITIMER_REAL, timeout)
          self.print_msg("Waiting for {0} ID {1}".format(type, id))
        
        try:
          notFound = True
          while notFound:
            tlm, evr = self._pop_queue()
            if tlm is None and evr is None and timeout == 0:
                raise self.TimeoutException()
            elif tlm is None and evr is None:
              time.sleep(0.1)
            else:
              if tlm:
                tlm_list.append(tlm)
                (recv_id, _) = tlm
                if type == "ch" and id == recv_id:
                  notFound = False
              if evr:
                evr_list.append(evr)
                (recv_id, _) = evr
                if type == "evr" and id == recv_id:
                  notFound = False
        except self.TimeoutException:
          self.print_msg("Timeout reached, unable to find {0} ID {1}".format(type, id))
        if timeout:
          signal.alarm(0)
        return tlm_list, evr_list

    def _pop_queue(self):
        """
        Grabs one event/telemetry from queue
        """
        #If the queue is empty attempt to find channels in packetized data
        if self._ch_listener.queue_size() == 0:
            #Note: return paket not used, packets routed to ch listener
            self._pk_listener.get_packet() 
        evr = self._ev_listener.get_event()
        tlm = self._ch_listener.get_channel()
        
        return tlm, evr

    def receive(self):
        """
        Grabs all telemetry and data in event listener's queue until the queue is emptied.
        Return a list of telemetry and events found.
        """
        
        tlm_list = []
        evr_list = []
        recv_id= ''
        
        notFound = True
        while notFound:
          tlm, evr = self._pop_queue()
          if tlm is None and evr is None:
            break
          else:
            if tlm:
              tlm_list.append(tlm)
              (recv_id, _) = tlm
              if id == recv_id and type == "ch":
                notFound = False
            if evr:
              evr_list.append(evr)
              (recv_id, _) = evr
              if id == recv_id and type == "evr":
                notFound = False
        return tlm_list, evr_list

    def flush(self):
        """
        Clears the telemetry/event queue and drops all data within it.
        """
        self.receive()

    def list(self, kind="cmds", ids=False):
        """
        Return a list of available commands, EVRs, or Channels.
        @param kind: kind of list desired: cmds, evrs, channels
        @param ids: if True return id numbers, else nnmonics
        @return: list of items
        """
        queryList = []

        if kind is "cmds":
            #NOTE: the dict values are ints, but they represent opcode hex values
            #TODO: see if another dict has a similar pattern to evrs and channels
            queryList = self._cmds.getOpCodeDict().values() if ids else self._cmds.getOpCodeDict().keys()
        elif kind is "evrs":
            queryList = self._events.getNameDict().keys() if ids else self._events.getNameDict().values()
        elif kind is "chans":
            queryList = self._channels.getNameDict().keys() if ids else self._channels.getNameDict().values()
        else:
            self.print_msg("Requested type is invalid.", file=sys.stderr)
        return queryList

    def send(self, cmd_name, args=None):
        """
        Send a command to the target applicaiton.
        @param cmd_name: Valid command mnemonic.
        @param args: Optional argument list for the command.
        """

        try:
           cmd_obj = self._cmds.getCommandObj(cmd_name)
        except KeyError:
           self.print_msg("%s is not a valid command mnemonic. Unable to send command." % cmd_name, file=sys.stderr)
           return -1

        if cmd_name:
            if args:
                self.__logger.info(cmd_name + "," + ",".join(str(a) for a in args))
            else:
                self.__logger.info(cmd_name)

        if args is not None:
           for i in range(len(args)):
               arg_name, arg_desc, arg_type = cmd_obj.getArgs()[i]
               arg_obj = command_args.create_arg_type(arg_name, arg_type, args[i])
               cmd_obj.setArg(arg_name, arg_obj)
        #print "Command serialized: %s (0x%x)" % (cmd_obj.getMnemonic(), cmd_obj.getOpCode())
        data = cmd_obj.serialize()
        #type_base.showBytes(data)

        # Package and send immediate command here...
        desc = U32Type( 0x5A5A5A5A )

        # Added desc. type for to know it is a command (0).
        desc_type = U32Type(0)

        data_len = U32Type( len(data) + desc_type.getSize() )
        cmd = "A5A5 " + "FSW " + desc.serialize() + data_len.serialize() + desc_type.serialize() + data
        #type_base.showBytes(cmd)
        if self.__sock == None:
            self.print_msg("Command %s not sent: No socket connection" % cmd_name, file=sys.stderr)
            return -1

        self.__sock.send(cmd)
        if args is None:
            self.print_msg('Sent command {0}'.format(cmd_name))
        else:
            self.print_msg('Sent command {0} with arguments: {1}'.format(cmd_name, args))
        return 0


    def send_file(self, src_path, dest_path, offset=0, data_size=512):
        """
        Send a file to the target application.
        If subprocess is True: starts a subprocess to handle the file upload.
        Else: Send file over current socket connection.
        @param src_path: Source path of file to be sent.
        @param dest_path: Destination path of file to be received by target application.
        @param offset: Byte offset into the source file (0 by default).
        @param data_size: Size of data packets (in bytes) being sent to application (default = 512).
        @param subprocess: Spawn new process
        @return: The subprocess if subprocess is True. UplinkStatus if subprocess is False.
        """
        ## Sends file  through current socket
        status, bytes_sent = file_uplink_client.send_file(src_path=src_path, dest_path=dest_path, sock=self.__sock, offset=offset, data_size=data_size, subprocess=False)
        self.print_msg("File bytes sent: {}".format(bytes_sent))
        if UplinkStatus(status) == UplinkStatus.SUCCESS:
            self.print_msg("File Uplink complete")
        else:
            self.print_msg("File Uplink error: {}".format(status), file=sys.stderr)
            raise Exception("File Uplink error: {}".format(status))

    def recieve_file(self, src, dest):
        """
        Request a file from target application.
        @param src: Source path
        @param dest: Destination path
        @param subprocess: Spawn new process
        @return: DownlinkStatus
        """
        # Get new socket
        sock = file_downlink_client.get_downlink_server_socket(self.__server_addr, self.__port)
        #Send downlink command
        self.send(cmd_name="FileDownlink_SendFile", args=[src,dest])
        # Catch incoming file packets
        status = file_downlink_client.startListen(sock=sock)
        if DownlinkStatus(status) == DownlinkStatus.SUCCESS:
            self.print_msg("File downlink successful")
        else:
            self.print_msg("File downlink error: {}".format(status), file=sys.stderr)
            raise Exception


    def create_downlink_subprocess(self):
        """
        Start new process to listen for incoming files.
        @return: Downlink Process
        """
        
        # Handle up/downlink
        self.__downlink_shell = file_downlink_client.DownlinkShell.getInstance()
        
        self.__downlink_shell.config(self.build_root)
        self.__downlink_shell.spawn(host=self.__server_addr, port=self.__port)
        DNLK = self.__downlink_shell.start()
        return DNLK

    def create_uplink_suprocess(self, src_path, dest_path):
        """
        Creates an uplink subprocess.
        @param src_path: Source path of file to be sent
        @param dest_path: Destination path of file to be recieved by target application
        @return: Uplink Process
        """
        self.__uplink_shell = file_uplink_client.UplinkShell.getInstance()
        
        self.__uplink_shell.config(self.build_root)
        self.__uplink_shell.spawn(host=self.__server_addr, port=self.__port, src_path=src_path, dest_path=dest_path)
        UPLK = self.__uplink_shell.startNext()
        return UPLK

    def send_wait_evr(self, cmd_name, evr_name, args=None, timeout=5):
        """
        Send a command and wait (block) until a timeout for an event response.
        @param cmd_name: Valid command mnemonic.
        @param evr_name: the name of a specific EVR to wait for.
        @param args: Optional argument list for the command.
        @param timeout: Optional timeout in seconds (default is 5 seconds).
        @return: A tuple with two lists (tlm_list, evr_list) of data collected while waiting
        """
        # Use code in controllers.commander.Commander.cmd_send(...)
        # Wait by blocking on queue if listener enabled.  Or you block on
        # socket directly.  Reimplement the event_listern.update_task method
        # in this class.
        #TODO:what should return be if timeout
        status = self.send(cmd_name,args)
        if status == -1:
           return [], []
        return self.wait_evr(evr_name, timeout)

    def wait_evr(self, evr_name, timeout=5):
        """
        Wait (block) until a timeout for an event response
        @param evr_name: the name of a specific EVR to wait for.
        @param timeout: Optional timeout in seconds (default is 5 seconds).
        @return: A tuple with two lists (tlm_list, evr_list) of data collected while waiting
        """
        evrNameDict = self._events.getEventsDictByName()
        evr_id = evrNameDict[evr_name].getId()
        return self.__loop_queue(evr_id, 'evr', timeout)

    def send_wait_tlm(self, cmd_name, tlm_name, args=None, timeout=5):
        """
        Send a command and wait (block) until a timeout for an channel response.
        @param cmd_name: Valid command mnemonic.
        @param tlm_name: the name of a specific tlm to wait for.
        @param args: Optional argument list for the command.
        @param timeout: Optional timeout in seconds (default is 5 seconds).
        @return: A tuple with two lists (tlm_list, evr_list) of data collected while waiting
        """
        # Use code in controllers.commander.Commander.cmd_send(...)
        # Wait by blocking on same queue if listener enabled.  This
        # reimplement the channel_listener.update() not to have the
        # gui notify(..) call.
        status = self.send(cmd_name,args)
        if status == -1:
           return [], []
        return self.wait_tlm(tlm_name, timeout)

    def wait_tlm(self, tlm_name, timeout=5):
        """
        Wait (block) until a timeout for an tlm response
        @param tlm_name: the name of a specific tlm to wait for.
        @param timeout: Optional timeout in seconds (default is 5 seconds).
        @return: A tuple with two lists (tlm_list, evr_list) of data collected while waiting
        """
        channelNameDict = self._channels.getChDictByName()
        channel_id = channelNameDict[tlm_name].getId()
        return self.__loop_queue(channel_id, 'ch', timeout)

    def monitor_evr(self, id=None, blocking=True):
        """
        Monitors for log event messages from a listener thread
        connected to the Threaded TCP Socket Server.  The routine
        uses the python logging module to display to stdout and
        to a log file.
        @param id: This is ether a None for displaying any event log message,
        or a list of id integers for the messages desired to be displayed,
        or a list of string names of the mnemonic for each message to be displayed.
        @param blocking: If True the routine blocks and waits for each messge,
        False it will poll for a message and display if one is present otherwise
        return.
        """
        signal.signal(signal.SIGINT, self.__ctrl_c_sig_handler)
        evr_dict = self._events.getNameDict()
        try:
            while(blocking):
                desc, tlm_id, args = self._ev_listener.update_task_api()
                # only interested in events
                if desc is 0x1 or desc is None:
                    continue
                if id is None or tlm_id in id:
                    output = '(' + str(tlm_id) + ':' + evr_dict[tlm_id] + ')'
                    self.__logger.info(output)
        except Exception as exc:
            self.print_msg("[ERROR] Exception while monitoring EVR: {}".format(exc), file=sys.stderr)

    def monitor_tlm(self, id=None, blocking=True):
        """
        Monitors for channel telemetry from a listener thread
        connected to the Threaded TCP Socket Server.  The routine
        uses the python logging module to display to stdout and
        to a log file.
        @param id: This is ether a None for displaying any channel telemetry,
        or a list of id integers for the channels desired to be displayed,
        or a list of string names of the mnemonic for each channel to be displayed.
        @param blocking: If True the routine blocks and waits for each channel update,
        False it will poll for a channel value and display if one is present otherwise
        return.
        """
        signal.signal(signal.SIGINT, self.__ctrl_c_sig_handler)
        ch_dict = self._channels.getNameDict()
        try:
            while(blocking):
                desc, tlm_id, args = self._ev_listener.update_task_api()
                # only interested in channelzed telemetry
                if desc is 0x2 or desc is None:
                    continue
                if id is None or tlm_id in id:
                    output = '(' + str(tlm_id) + ':' + ch_dict[tlm_id] + ')'
                    self.__logger.info(output)
        except Exception, exc:
            self.print_msg("[ERROR] Exception while monitoring TLM: {}".format(exc), file=sys.stderr)

    def get_evr_id(self, evr_name):
        """
        Given an evr name, return the corresponding evr id
        @param evr_name: the name of a specific evr
        @return: the id of evr_name
        """
        return self._events.getEventsDictByName()[evr_name].getId()

    def get_tlm_id(self, tlm_name):
        """
        Given a tlm name, return the corresponding tlm id
        @param tlm_name: the name of a specific tlm
        @return: the id of tlm_name
        """
        return self._channels.getChDictByName()[tlm_name].getId()

    def get_cmd_id(self, command_name):
        """
        Given a command_name (mnemonic), return the corresponding command op code id
        @param command_name: the name of a specific command (mnemonic)
        @return: the id (op code) of command_name
        """
        return self._cmds.getCommandObj(command_name).getOpCode()

    def get_evr_name(self, evr_id):
        """
        Given an evr id, return the corresponding evr name
        @param evr_id: the id of a specific id
        @return: the name of evr_id
        """
        return self._events.getEventsDict()[evr_id].getName()

    def get_tlm_name(self, tlm_id):
        """
        Given a tlm id, return the corresponding tlm name
        @param tlm_id: the id of a specific tlm
        @return: the name of tlm_id
        """
        return self._channels.getChDict()[tlm_id].getName()

    def get_cmd_name(self, command_id):
        """
        Given a command_id (opcode), return the corresponding command name (mnemonic)
        @param command_id: the id of a specific command (opcode)
        @return: the name (mnemonic) of command_id
        """
        return self._cmds.getCommandObjById(command_id).getMnemonic()
