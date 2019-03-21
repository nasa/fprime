##########################################
#
# Quick client sockets example.
# reder@jpl.nasa.gov
#
##########################################
#
from Tkinter import *

import Tkinter
import socket
import struct
import time
import select

class ClientSocket:
    """
    Class to perform client side socket connection
    """

    def __init__(self, host_addr, port):
        """
        Connect up the socket here.
        """
        self.exit_flag = False
        try:
            #print "Connecting to host addr %s, port %d\n" % (host_addr, port)
            # Create the socket
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

            # Set the size of the socket send and receive buffers to blen
            #socket.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, len(msg))
            #socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, len(msg))
            # Connect to server
            self.sock.connect((host_addr, port))

        except IOError as e:
            raise IOError("EXCEPTION: Could not connect to socket at host addr %s, port %s" % (host_addr, port))


    def __del__(self):
        self.disconnect()

    def disconnect(self):
        """
        Exit a potentially hanged receive aand close the socket
        """
        self.exit_flag = True
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
            self.sock.close()
        except:
            pass

    def receive(self):
        """
        Just return a msg if less them 512 bytes, otherwise
        return 1024 at a time.
        """
        l=1024
        msg = ''

        msg = self.sock.recv(l/2)
        if len(msg) < l/2:
            return msg

        while len(msg) < l:
            chunk = self.sock.recv(l-len(msg))
            #print chunk
            if chunk == '':
                raise RuntimeError("socket connection broken")
            msg = msg + chunk
        return msg

    def recv(self, l):
        """
        Read l bytes from socket.
        """
        chunk =""
        msg = ""
        n = 0
        while l > n:
            if self.exit_flag:
                raise Exception("Exiting receive loop")

            # Check if the socket is ready to read
            fd = select.select([self.sock], [], [], .25)
            if fd[0]:
                chunk = self.sock.recv(l-n)
                if chunk == '':
                    return ''
                    #raise RuntimeError("socket connection broken")
                msg = msg + chunk
                n = len(msg)
        return msg

    def send(self, msg):
        try:
            self.sock.sendall(msg)
        except IOError:
            raise IOError("EXCEPTION: Could not send message (%s) to socket" % msg)

#
# Main loop
#
def main():
    port = 60002

    s = ClientSocket("192.168.1.100", port)
    i = 0
    while 1:
        try:
            x_est = s.receive()
            print x_est
        except RuntimeError:
            raise IOError("Socket connection terminated")
            break


if __name__ == "__main__":
    main()
