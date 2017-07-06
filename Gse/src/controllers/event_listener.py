#!/bin/env python
#===============================================================================
# NAME: EventListener.py
#
# DESCRIPTION: This module is the event listener class that is a thread
#              which listens for incomming log event messages. When a
#              message is received first the descriptor and size are
#              decoded.  Then the event id is decoded.  The event id
#              looks up a specific Event class instance that describes
#              the event id type and can deserilize it.
#
#              The event logging panel and other panels act as
#              observers and update when new data from the listen
#              thread is avaliable.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED: Feb 9, 2014
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#
import collections
import sys
import os
import threading
import Queue
import struct
import logging
import time

#@todo Add logging to disk of log events via the Logger module.

from controllers import observer
from controllers import event_loader
from controllers import channel_listener
from controllers import status_bar_updater
from controllers import stripchart_listener

from models.serialize import type_base
from models.serialize import u32_type
from models.serialize import u16_type
from models.serialize import u8_type

#from views import main_panel
from utils import Logger


class EventListener(observer.Observed):
    """
    An event listener class that monitors incoming events,
    decodes them into strings, and updates Logger panel
    within the GUI.
    @todo: For now connect directly to the socket but in future mediate packets.
    """
    __instance = None


    def __init__(self, sock=None):
        """
        Constructor.
        WARNING: After the first instantiation setupLogging must be executed.
        """
        super(EventListener, self).__init__()
        #
        # Socket when connection established
        #
        self.__sock = sock
        #
        # Store thread here
        #
        self.__thread = threading.Thread()
        #
        # Instance a lock and make all socket read/write atomic
        #
        self.__lock = threading.Lock()
        #
        # Create Queue for events here
        #
        self.__queue = Queue.Queue()
        #
        # Instance the event loader here
        #
        self.__event_loader = event_loader.EventLoader.getInstance()
        self.__event_obj_dict = self.__event_loader.getEventsDict()
        #print self.__event_obj_dict
        #
        # Instance the channel telemetry listener here
        #
        self.__channel_listener = channel_listener.ChannelListener.getInstance()
        #
        self.__stripchart_listener = stripchart_listener.StripChartListener.getInstance()
        #
        # Current event log message string
        #
        self.__current_event_log_msg = None

        # Status bar observer
        self.__status_bar_updater  = status_bar_updater.StatusBarUpdater.getInstance()

        #
        # Store the after event id here for the current one...
        self.__after_id = None

        # Manually break from update_tasks
        self.update_break = False
        #
        # setup the bin file
        self.__binfile = None

        # store options
        self.__opt = None


    def getInstance():
        """
        Return instance of singleton.
        """
        if(EventListener.__instance is None):
            EventListener.__instance = EventListener()
        return EventListener.__instance

    #define static method
    getInstance = staticmethod(getInstance)


    def setupLogging(self, opt=None):
        """
        Set up logging once for the singleton to use.
        """

        if opt == None:
            p = os.environ['HOME'] + os.sep + 'fprime_logs' + os.sep + "event"
        else:
            p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "event"
        #
        logfile = 'Event.log'
        #
        self.__opt = opt

        if not os.path.exists(p):
            os.makedirs(p)
        f = p + os.sep + logfile
        self.__logger = Logger.connectOutputLogger(f, 'event_out')
        self.__logger.info("Created log: %s" % f)
        self.__logger.info("User: %s" % os.environ['USER'])

    def setupBinaryLogging(self, opt=None):
        """
        Set up binary data logging once for the singleton to use.
        """

        # don't log binary data if flag has been set to 0
        if Logger.BIN_LOGGER_FLAG == 0:
            return

        if opt == None:
            p = os.environ['HOME'] + os.sep + 'fprime_logs' + os.sep + "event"
        else:
            p = opt.log_file_path + os.sep + opt.log_file_prefix + os.sep + "event"

        self.__opt = opt

        logfilename = 'RAW.bin'
        outfile = p + os.sep + logfilename
        #bufsize of 0 tells python not to buffer data when writing to file, just flush
        bufsize = 0
        self.__binfile = open(outfile, "wb", bufsize)

    def get_bin_logfile(self):
        return self.__binfile

    def close_bin_logfile(self):
        self.__binfile.close()

    def set_after_id(self, id):
        self.__after_id = id

    def get_after_id(self):
        return self.__after_id

    def register_root(self, the_root):
        """
        Register the root so that the after call works in thread.
        """
        self.__root = the_root


    def register_main_panel(self, the_main_panel):
        """
        Register the main panel to get socket handle
        if one is set.  This is an indication of connection.
        """
        self.__the_main_panel = the_main_panel
        self.__sock = self.__the_main_panel.getSock()

    def register_status_bar(self, status_bar):
        self.__status_bar = status_bar

    def receive_telemetry(self, sock):
        """
        Receive telemetry by first reading 4 byte size,
        then 4 byte desc. Then recieve the rest of message.
        Send size, desc, id, and optional args for decoding.
        """
        pkt_len = sock.recv(4)
        if self.__binfile != None:
            self.__binfile.write(pkt_len)

        pkt_desc    = sock.recv(4)
        if self.__binfile != None:
            self.__binfile.write(pkt_desc)

        desc = int(struct.unpack(">I",pkt_desc)[0])
        size = int(struct.unpack(">I",pkt_len)[0])

        #
        #print "size = %d" % size
        #print  "desc = %d" % desc
        data = sock.recv(size - u32_type.U32Type().getSize())

        if self.__binfile != None:
            self.__binfile.write(data)


        #print "data: "
        #type_base.showBytes(data)
        return pkt_len + pkt_desc + data


    def enqueue_output(self, sock, queue):
        """
        Queue up socket telemetry for TK processing
        """
        while 1:
            try:
                x = self.receive_telemetry(sock)
                if x:
                    queue.put(x)
            except:
                print "Socket connection terminated"
                break
        return


    def connect(self, sock):
        """
        Start thread that is connected to sock talking to TCPThreadServer.py
        THis is called from the TCP Server menu Connect... item.
        """
        self.__sock = sock

        if self.__thread.isAlive() == True:
            print "LISTENER THREAD IS ALIVE!"
            return

        # create background listener thread
        self.__thread = threading.Thread(target=self.enqueue_output, args=(self.__sock, self.__queue))
        # thread dies with the program
        self.__thread.daemon = True
        # state listener thread here
        self.__thread.start()


    def decode_desc(self, msg):
        """
        Decode the size and descriptor and return them.
        """
        ptr = 0
        # Decode size here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, 0)
        size = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Size = 0x%x" % size

        # Decode descriptor part of message
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        desc = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Desc = 0x%x" % desc
        return (desc, size)

    def decode_desc_api(self, msg):
        """
        Decode the size and descriptor and return them.
        """
        ptr = 0
        # Decode size here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, 0)
        size = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Size = 0x%x" % size

        # Decode descriptor part of message
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        desc = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Desc = 0x%x" % desc

        # Decode log event ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
        #print "ID: %d" % i
        return (desc, size, i)

    def decode_event(self, msg):
        """
        Decode event log message using Event object dictionary.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0
        # Decode size here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, 0)
        size = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Size = 0x%x" % size

        # Decode descriptor part of message
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        desc = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Desc = 0x%x" % desc

        # Decode log event ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
        #print "ID: %d" % i

        # Decode time...
        # Base
        u32_obj = u16_type.U16Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_base = u32_obj.val
        #print "Time Base: %d" % time_base

        # Time context
        u8_obj = u8_type.U8Type()
        u8_obj.deserialize(msg,ptr)
        ptr += u8_obj.getSize()
        time_context = u8_obj.val

        # Seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_secs = u32_obj.val
        #print "Time Seconds: %d" % time_secs
        # Micro-seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_usecs = u32_obj.val
        #print "Time MicroSeconds: %d" % time_usecs

        # Decode arguments here...
        # Look up correct Event event log message instance object for decoding
        if i in self.__event_obj_dict:
            event_obj = self.__event_obj_dict[i]
            #
            # Deserialize to a tuple to stringify
            event_args = event_obj.deserialize(msg[ptr:])
            # Stringify to formatted message string for GUI updates
            fmt = event_obj.stringify(event_args)
            #
            # Package NAME, ID, SEVERITY, and formatted string into tuple for views update.
            # 'Name (id) Severity : formatted string'
            name = event_obj.getName()
            i    = event_obj.getId()
            s    = event_obj.getSeverity()
            # Send severity to stripchart listener
            self.__stripchart_listener.update_severity(s)
            self.__stripchart_listener.update_events(name)

            # See if set to workstation time
            if (time_base == 2):
                if self.__opt.log_time == "local":
                    t = time.strftime("%m/%d-%H:%M:%S",time.localtime(time_secs))
                else:
                    t = time.strftime("%m/%d-%H:%M:%S",time.gmtime(time_secs))

            else:
                t = "%f" % (float(time_secs) + (float(time_usecs)/1000000))
            # Create tuple of log msg string and log msg object
            msg_tup = (t, name, i, s, fmt)
            self.__current_event_log_msg = ("%s: %s (%d) %s : %s" % msg_tup, msg_tup)
            #print self.__current_event_log_msg
        else:
            self.__current_event_log_msg = "EVR ID Received but could not be decoded: %d" % i

    def decode_event_api(self, msg):
        """
        Decode event log message using Event object dictionary.
        """
        #type_base.showBytes(msg)
        #
        ptr = 0
        # Decode size here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, 0)
        size = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Size = 0x%x" % size

        # Decode descriptor part of message
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        desc = u32_obj.val
        ptr += u32_obj.getSize()
        #print "Desc = 0x%x" % desc

        # Decode log event ID here...
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        i = u32_obj.val
        #print "ID: %d" % i

        # Decode time...
        # Base
        u32_obj = u16_type.U16Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_base = u32_obj.val
        #print "Time Base: %d" % time_base

        # Time context
        u8_obj = u8_type.U8Type()
        u8_obj.deserialize(msg,ptr)
        ptr += u8_obj.getSize()
        time_context = u8_obj.val

        # Seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_secs = u32_obj.val
        #print "Time Seconds: %d" % time_secs
        # Micro-seconds
        u32_obj = u32_type.U32Type()
        u32_obj.deserialize(msg, ptr)
        ptr += u32_obj.getSize()
        time_usecs = u32_obj.val
        #print "Time MicroSeconds: %d" % time_usecs

        event_args = None
        if i in self.__event_obj_dict:
            event_obj = self.__event_obj_dict[i]
            event_args = event_obj.deserialize(msg[ptr:])[1:]
        return event_args

    def getCurrentEventLogMsg(self):
        """
        Used by logger panel update() to get the string to display.
        """
        return self.__current_event_log_msg


    def __log_info(self, s):
        """
        Update log message with time tag.
        """
        if self.__opt.log_time == "local":
            t = time.strftime("%Y-%m-%d %H:%M:%S:", time.localtime())
        else:
            t = time.strftime("%Y-%m-%d %H:%M:%S:", time.gmtime())

        self.__logger.info("%s %s" % (t,s[0]))


    def update_task_start(self):
        """
        Start the update task here...
        """
        time_period = 200
        self.__after_id = self.__root.after(time_period, self.update_task)


    def update_task_cancel(self):
        """
        Cancel the update task here...
        """
        self.__root.after_cancel(self.__after_id)


    def update_task(self):
        """
        Update event log message here...
        1. Receive message from thread.
        2. Decode message into object.
        3. Generate updates for all log event panel instances.
        """
        time_period = 2
        max_msgs = 100

        # read line without blocking
        while max_msgs > 0:
            msg = None
            try:
                msg = self.__queue.get_nowait()
                #print msg
                #type_base.showBytes(msg)
                if len(msg) > 0:
                    pass
            except Queue.Empty:
                #print('no output yet')
                self.__status_bar_updater.update_data(0,0, data_incoming=False)
                self.__after_id = self.__root.after(time_period, self.update_task)
                return
                # ... do something with line
            #
            # Observer update will be generated here...
            #
            if len(msg) > 0:
                # If window is exiting we need to stop updating
                # Set in main_panel.__handle_exit()
                if self.update_break:
                    self.update_break = False
                    return

                (desc, size) = self.decode_desc(msg)
                if desc == 2:
                    # Update event messages here...
                    self.decode_event(msg)
                    self.observers_notify()
                    self.__log_info(self.getCurrentEventLogMsg())
                elif desc == 1:
                    # Update channel telemetry here...
                    self.__channel_listener.update(msg)
                    tlm = self.__channel_listener.getCurrentChannelTelemetryItem()
                    # Send to stripchart
                    self.__stripchart_listener.update_telem(tlm)
                else:
                    print ("Unknown descriptor %d"%desc)

                # Update status bar
                self.__status_bar_updater.update_data(num_recv=size, num_sent=0, data_incoming=True)

            max_msgs = max_msgs - 1

        self.__after_id = self.__root.after(time_period, self.update_task)
        return msg

    def update_task_api(self):
        """
        Update event log message here...
        1. Receive message from thread.
        2. Decode message into object.
        """
        if self.__sending_file:
            self.__monitor_uplink()

        time_period = 200
        # read line without blocking
        msg = ""
        try:
            msg = self.__queue.get_nowait()
            #print msg
            #type_base.showBytes(msg)
            if len(msg) > 0:
                pass
        except Queue.Empty:
            #print('no output yet')
            self.__current_event_log_msg = None
            return (None, None, None)
            # ... do something with line
        #
        # Observer update will be generated here...
        #
        args = None
        if len(msg) > 0:
            (desc, size, evr_id) = self.decode_desc_api(msg)
            if desc == 2:
                # Update event messages here...
                args = self.decode_event_api(msg)
            elif desc == 1:
                # Update channel telemetry here...
                args = self.__channel_listener.decode_ch_api(msg)
        return (desc, evr_id, args)

if __name__ == "__main__":
    pass
