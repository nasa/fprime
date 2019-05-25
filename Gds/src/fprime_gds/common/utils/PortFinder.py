#!/usr/bin/env python
# encoding: utf-8
from __future__ import print_function

import socket
import sys
import subprocess

from fprime.constants import DATA_ENCODING
'''
This module is used to find an available socket port.

@author:     Leonard J. Reder
            
@copyright:  2014 California Institute of Technology. All rights reserved.
            
@license:    license

@contact:    reder@jpl.nasa.gov
'''
def getstatusoutput(cmd):
    '''
    Replaces `commands.getstatusoutput` for use in python 2/3 code. This is a wrapper layer to virtualize old python 2
    code with new python 3 syntax and libraries.
    @param cmd: command to run
    '''
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    (out, err) = process.communicate()
    if sys.version_info >= (3, 0):
        out = out.decode(DATA_ENCODING)
    assert err is None, "Failed to force standard error to standard out"
    return (process.returncode, out)


def IsPortUsed(port):
    s = ''
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind(('',port))
        s.close()

        try:
           s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
           s.bind((socket.gethostname(),port))
           s.close()
        except: 
           print('error: no valid address-to-host mapping for the host %s.' % socket.gethostname())
       
        portUsed = False
    except:
        #print 'error:', sys.exc_info()[0]
        portUsed = True                
        s.close()
       
    return portUsed

def getport(port, port_pool):
   # Recursively call getport() if specified port is used or in the port pool
   if IsPortUsed(port) or port in set(port_pool):
      port = getport(port+1, port_pool)
   return port 


def old_getport(startport,newport):
   portnum = startport
   # Search /etc/services for specified socket port
   cmd = "grep " + str(startport) + " /etc/services"
   (status, portused1) = getstatusoutput(cmd)
   found = portused1.find(str(startport))

   # Query netstat for the specified socket port
   if found == -1:
      cmd = "netstat -a | grep " + str(startport)
      (status, portused2) = getstatusoutput(cmd)
      found = portused2.find(str(startport))

   # Recursively call getport() if specified port is used
   if found != -1:
      startport = startport + 1
      portnum = getport(startport, newport)

   # Recursively call getport() if specified port is in supplied list
   for port in newport:
      if startport == port:
         startport = startport +1
         portnum = getport(startport, newport)

   return portnum

if __name__ == "__main__":
   print(old_getport(150,[]))

   #for port in range(50000,50100):
   #   val = IsPortUsed(port)
   #   if not val:
   #      print 'found valid port number = ', port
   #      break
