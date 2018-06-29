#!/usr/bin/env python

import socket
import threading
import Queue
import SocketServer
import time
import os
import signal
import sys
import struct
import select
import multiprocessing
import errno
#import GSEAdapter
import time
import logging

import binascii

from utils import Logger
from models.serialize.type_base import *
from optparse import OptionParser

__version__ = 0.1
__date__ = '2015-04-03'
__updated__ = '2016-04-07'

# Universal server id global
SERVER = None
LOCK   = None
shutdown_event = threading.Event()

#f = "set logging file"
#LOGGER = Logger.connectOutputLogger(file=f, logger_name=__name__, logger_level=logging.INFO)

FSW_clients = []
GUI_clients = []
FSW_ids = []
GUI_ids = []

def signal_handler(signal, frame):
    print "Ctrl-C received, server shutting down."
    shutdown_event.set()


def now():
    return time.ctime(time.time())

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
    """
    Derived from original Stable demo during R&TD and adapted
    for use in new FSW gse.py applicaiton.

    TCP socket server for commands, log events, and telemetry data.
    Later this will handle other things such as sequence files and parameters.

    Handle is instanced in own thread for each client.

    Registration is done by sending the string "Register <name>".
    Sending a message to destination <name> is done as
    "A5A5 <name> <data>" Note only <data> is sent.
    Any client that sends a "List" comment makes the server display all
    registered clients.
    """
    SocketServer.BaseRequestHandler.allow_reuse_address = True

    def handle(self):                           # on each client connect
        """
        The function that is invoked upon a new client.  This function listens
        for data on the socket.  Packets for now are assumed to be separated
        by a newline.  For each packet, call processPkt.
        """

        self.partial = ''
        self.cmdQueue = []
        self.registered = False
        self.name = ''
        self.id = 0

        #print self.client_address, now()        # show this client's address
        # Read the data from the socket
        data = self.recv(13)

        # Connection was closed by the client
        if not data:
            print "Client exited."
            return

        else:
            # Process the data into the cmdQueue
            self.getCmds(data)

            # Process the cmdQueue
            self.processQueue()

            if self.registered:
                print "Registration complete waiting for message."
                self.getNewMsg()
            else:
                print "Unable to register client."
                return


        LOCK.acquire()
        del SERVER.dest_obj[self.name]
        if self.name in FSW_clients:
            FSW_clients.remove(self.name)
            FSW_ids.remove(self.id)
        elif self.name in GUI_clients:
            GUI_clients.remove(self.name)
            GUI_ids.remove(self.id)
        LOCK.release()

        print "Closed %s connection." % self.name
        self.registered = False
        self.request.close()


    def getCmds(self, inputString, end_of_command='\n'):
        """
        Build a command from partial or full socket input
        """
        commands = inputString.split(end_of_command)
        if len(self.partial):
            commands[0] = self.partial + commands[0]
            self.partial = ''
        if len(commands[-1]):
            self.partial = commands[-1]
            self.cmdQueue.extend(commands[:-1])
        else:
            self.cmdQueue.extend(commands[:-1])

    def processQueue(self):
        for cmd in self.cmdQueue:
            self.processRegistration(cmd)
        self.cmdQueue = []

    def processRegistration(self, cmd):

        params = cmd.split()
        id = 0

        if params[0] == 'Register':
            LOCK.acquire()
            name = params[1]
            if 'FSW' in name:
                if FSW_clients:
                    id = sorted(FSW_ids)[-1] + 1

                name = params[1] + '_' + str(id)
                FSW_clients.append(name)
                FSW_ids.append(id)
            elif 'GUI' in name:
                if GUI_clients:
                    id = sorted(GUI_ids)[-1] + 1

                name = params[1] + '_' + str(id)
                GUI_clients.append(name)
                GUI_ids.append(id)


            SERVER.dest_obj[name] = DestObj(name, self.request)
            LOCK.release()

            self.registered = True
            self.name = name
            self.id = id
            print "Registered client " + self.name

    #################################################
    # New Routines to process the command messages
    #################################################
    def getNewMsg(self):
        """
        After registration wait for an incoming message
        The first part must always be an "A5A5 " or a "List "
        """

        # Loop while the connected client has packets to send/receive
        while not shutdown_event.is_set():
            # Read the header data from the socket either A5A5 or List
            header = self.readHeader()

            #If the received header is an empty string, connection closed, exit loop
            if not header:
                break

            elif header == "Quit":
                LOCK.acquire()
                print "Quit received!"
                SERVER.shutdown()
                SERVER.server_close()
                shutdown_event.set()
                LOCK.release()
                break

            # Got the header data so read the data of the message here...
            data = self.readData(header)

            # Process and send the packet of the message here...
            self.processNewPkt(header, data)


    def recv(self, l):
        """
        Read l bytes from socket.
        """
        chunk =''
        msg = ""
        n = 0
        while l > n:
            try:
                chunk = self.request.recv(l-n)
                if chunk == '':
                    print "read data from socket is empty!"
                    return ''
                msg = msg + chunk
                n = len(msg)
            except socket.error, err:
                if err.errno == errno.ECONNRESET:
                    print "Socket error " + str(err.errno) + " (Connection reset by peer) occurred on recv()."
                else:
                    print "Socket error " + str(err.errno) + " occurred on recv()."
        
        print msg
        return msg


    def readHeader(self):
        """
        Read the 9 byte header (e.g. "A5A5 GUI " or "A5A5 FSW "),
        or just read the "List\n" command.
        """
        header = self.recv(5)

        if len(header) == 0:
            print "Header information is empty, client " + self.name + " exiting."
            return header
        if header == "List\n":
            return "List"
        elif header == "Quit\n":
            return "Quit"
        elif header[:-1] == "A5A5":
            header2 = self.recv(4)
            return (header + header2)
        else:
            return


    def readData(self,header):
        """
        Read the data part of the message sent to either GUI or FSW.
        GUI receives telemetry.
        FSW receives commands of various lengths.
        """
        data = ""
        if header == "List":
            return ""
        elif header == "Quit":
            return ""
        dst = header.split(" ")[1].strip(" ")
        if dst == "FSW":
            # Read variable length command data here...
            desc = self.recv(4)
            sizeb = self.recv(4)
            size = struct.unpack(">I", sizeb)[0]
            data = desc + sizeb + self.recv(size)
        elif dst == "GUI":
            # Read telemetry data here...
            tlm_packet_size = self.recv(4)
            size = struct.unpack(">I", tlm_packet_size)[0]
            data = tlm_packet_size + self.recv(size)

        else:
            raise RuntimeError("unrecognized client")
        return data


    def processNewPkt(self,header,data):
        """
        Process a single command here header and data here.
        The command must always start with A5A5 except if it is a List.
        Once the entire header string is processed send it on queue.
        If something goes wrong report and shutdown server.
        """
        dest_list = []

        if header == "List":
            print "List of registered clients: "
            LOCK.acquire()
            for d in SERVER.dest_obj.keys():
                print "\t" + SERVER.dest_obj[d].name
                reg_client_str = "List " + SERVER.dest_obj[d].name
                l = len(reg_client_str)
                reg_client_str = struct.pack("i%ds" % l, l,reg_client_str)
                self.request.send(reg_client_str)
            LOCK.release()
            return 0

        # Process data here...
        head, dst = header.strip(" ").split(" ")
        if head == 'A5A5':  # Packet Header
            #print "Received Packet: %s %s...\n" % (head,dst)
            if data == '':
                print " Data is empty, returning."
            if 'GUI' in dst:
                dest_list = GUI_clients
            elif 'FSW' in dst:
                dest_list = FSW_clients
            for dest_elem in dest_list:
                LOCK.acquire()
                if dest_elem in SERVER.dest_obj.keys():
                    # Send the message here....
                    #print "Sending msg to ", dest_elem

                    SERVER.dest_obj[dest_elem].put(data)
                LOCK.release()
        else:
            raise RuntimeError("Packet missing A5A5 header")

class ThreadedUDPRequestHandler(SocketServer.BaseRequestHandler):
    """
    Derived from original Stable demo during R&TD and adapted
    for use in new FSW gse.py applicaiton.

    TCP socket server for commands, log events, and telemetry data.
    Later this will handle other things such as sequence files and parameters.

    Handle is instanced in own thread for each client.

    Registration is done by sending the string "Register <name>".
    Sending a message to destination <name> is done as
    "A5A5 <name> <data>" Note only <data> is sent.
    Any client that sends a "List" comment makes the server display all
    registered clients.
    """
    SocketServer.BaseRequestHandler.allow_reuse_address = True

    def handle(self):                           # on each packet
        """
        The function that is invoked when a packet is received.  This function listens
        for data on the socket.  Packets for now are assumed to be separated
        by a newline.  For each packet, call processPkt.
        """

        self.getNewMsg(self.request[0])
        

    #################################################
    # New Routines to process the command messages
    #################################################
    def getNewMsg(self,packet):
        """
        After registration wait for an incoming message
        The first part must always be an "A5A5 " or a "List "
        """

        # Read the header data from the socket either A5A5 or List
        (header,packet) = self.readHeader(packet)

        #If the received header is an empty string, connection closed, exit loop
        if not header:
            return

        # Got the header data so read the data of the message here...
        data = self.readData(header,packet)

        # Process and send the packet of the message here...
        self.processNewPkt(header, data)


    def readHeader(self, packet):
        """
        Read the 9 byte header (e.g. "A5A5 GUI " or "A5A5 FSW "),
        or just read the "List\n" command.
        """
        header = packet[:4]
        header2 = packet[4:9]
        packet = packet[9:]
        return (header + header2,packet)

    def readData(self,header,packet):
        """
        Read the data part of the message sent to either GUI or FSW.
        GUI receives telemetry.
        FSW receives commands of various lengths.
        """
        data = ""
        dst = header.split(" ")[1].strip(" ")
        # Read telemetry data here...
        tlm_packet_size = packet[:4]
        size = struct.unpack(">I", tlm_packet_size)[0]
        data = tlm_packet_size + packet[4:4+size]

        return data


    def processNewPkt(self,header,data):
        """
        Process a single command here header and data here.
        The command must always start with A5A5 except if it is a List.
        Once the entire header string is processed send it on queue.
        If something goes wrong report and shutdown server.
        """
        dest_list = []
        print (binascii.hexlify(header))
        print(binascii.hexlify(data))
        # Process data here...
        head, dst = header.strip(" ").split(" ")
        if head == 'A5A5':  # Packet Header
            print "Received Packet: %s %s...\n" % (head,dst)
            if data == '':
                print " Data is empty, returning."

            if 'GUI' in dst:
                dest_list = GUI_clients
            else:
                print "dest? %s"%dst

            for dest_elem in dest_list:
                LOCK.acquire()
                if dest_elem in SERVER.dest_obj.keys():
                    # Send the message here....
                    #print "Sending msg to ", dest_elem
                    SERVER.dest_obj[dest_elem].put(data)
                LOCK.release()
        else:
            raise RuntimeError("Telemetry missing A5A5 header")

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    """
    TCP Socket server.

    Keep a dictionary of destination objects containing queues and
    socket id's for writting to destinations.
    """
    dest_obj = dict()
    lock_obj = threading.Lock()

class ThreadedUDPServer(SocketServer.ThreadingMixIn, SocketServer.UDPServer):
    """
    UDP Socket server.
    """


class DestObj:
    """
    Destination object for all clients registered.
    """
    def __init__(self, name, request):
        """
        Constructor
        """
        self.name = name
        self.socket = request
        self.packet = ""

    def put(self, msg):
        """
        Write out the message to the destination socket
        """
        print "Message to client %s"% msg
        try:
           #print "about to send data to " + self.name
           self.socket.send(msg)
        except socket.error, err:
           print "Socket error " + str(err.errno) + " occurred on send()."


    def fileno(self):
        """
        """
        return self.socket

def main(argv=None):
        global SERVER, LOCK

        program_name = os.path.basename(sys.argv[0])
        program_license = "Copyright 2015 user_name (California Institute of Technology)                                            \
                ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged."
        program_version = "v0.1"
        program_build_date = "%s" % __updated__
        program_version_string = '%%prog %s (%s)' % (program_version, program_build_date)
        program_longdesc = '''''' # optional - give further explanation about what the program does

        if argv is None:
            argv = sys.argv[1:]

        try:
            parser = OptionParser(version=program_version_string, epilog=program_longdesc, description=program_license)
            parser.add_option("-p", "--port", dest="port", action="store", type="int", help="Set threaded tcp socket server port [default: %default]", \
                             default=50007)
            parser.add_option("-i", "--host", dest="host", action="store", type="string", help="Set threaded tcp socket server ip [default: %default]", \
                             default="127.0.0.1")

            # process options
            (opts, args) = parser.parse_args(argv)

            HOST = opts.host
            PORT = opts.port
            server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
            udp_server = ThreadedUDPServer((HOST, PORT), ThreadedUDPRequestHandler)
            # Hopefully this will allow address reuse and server to restart immediately
            server.allow_reuse_address = True
            SERVER = server
            LOCK   = server.lock_obj
            ip, port = server.server_address

            print "TCP Socket Server listening on host addr %s, port %s" % (HOST, PORT)
            # Start a thread with the server -- that thread will then start one
            # more thread for each request
            server_thread = threading.Thread(target=server.serve_forever)
            udp_server_thread = threading.Thread(target=udp_server.serve_forever)
            signal.signal(signal.SIGINT, signal_handler)
            server_thread.daemon = False
            server_thread.start()
            udp_server_thread.daemon = False
            udp_server_thread.start()
            p = os.getpid()
            #print "Process ID: %s" % p

            while not shutdown_event.is_set():
               server_thread.join(timeout = 1.0)
               udp_server_thread.join(timeout = 1.0)

            SERVER.shutdown()
            SERVER.server_close()
            udp_server.shutdown()
            udp_server.server_close()
            
            time.sleep(1)

        except Exception, e:
            indent = len(program_name) * " "
            sys.stderr.write(program_name + ": " + repr(e) + "\n")
            sys.stderr.write(indent + "  for help use --help\n")
            return 2

if __name__ == "__main__":
    sys.exit(main())
