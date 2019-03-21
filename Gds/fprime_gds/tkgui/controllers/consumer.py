#!/bin/env python
#===============================================================================
# NAME: consumer.py
#
# DESCRIPTION: This module is the consumer class. Class contains a thread
#              which blocks on a queue for messages. The thread then
#              executes conext specific data processing, and notifies
#              observers with the processed data.
#
# AUTHOR: Brian Campuzano
# DATE CREATED: July 10, 2017
#
# Copyright 2017, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

import Queue
import threading

from fprime_gds.tkgui.controllers import observer

# Make class an abstract base class
class Consumer(observer.Observable):
    '''
    Thread class that services a queue
    '''

    def __init__(self):

      super(Consumer, self).__init__()

      # Create Queue for events here
      self._queue = Queue.Queue()

      # Run flag
      self.__run = False

      # Store thread handle
      self.__thread = threading.Thread()

    def queue_size(self):
      return self._queue.qsize()


    def put_data(self, data):
      self._queue.put(data)

    def start_thread(self):
        if self.__run == False:
            # Set run flag
            self.__run = True

            # create background event thread
            self.__thread = threading.Thread(target=self.service_queue, args=[self._queue])

            # state event thread here
            self.__thread.start()


    def stop_thread(self):
      # Stop thread and wait for exit
      self.__run = False

    def process_data(self, data):
      '''
      Override function for data specific processing
      '''
      pass

    def service_queue(self, queue):
        '''
        Queue up socket events for observer processing
        '''
        while self.__run:
          try:
            # Block on queue for 1 second in order to check exit condition
            data = queue.get(True, 1)
          except Queue.Empty:
            continue
          else:
            self.process_data(data)
            self.notifyObservers(data)

if __name__ == '__main__':
    pass
