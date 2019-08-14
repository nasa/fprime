'''
@brief file writer to write decoded objects into files

This file writer takes in decoded data and writes the data
to the correct log files and destination filse

@date Created August 8, 2019
@author Blake A. Harriman

@bug No known bugs
'''
from __future__ import print_function
import copy
from struct import *
from enum import Enum
from fprime_gds.common.decoders import decoder
from fprime_gds.common.data_types import file_data
from fprime_gds.common.decoders import file_decoder
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize import time_type
from fprime.common.models.serialize.type_exceptions import *
import multiprocessing as mp
from multiprocessing import Process
import threading
import time
import traceback
import datetime
import os
import random

#Main FileWriter class
class FileWriter(decoder.Decoder):
    '''File writer class for decoded packets'''
    def __init__(self, timeout_duration = 120.0, verbose = 2):
        '''
        FileWriter class constructor

        Args:
            timeout_duration: This is used to keep track of how long the program should wait before throwing
            a timeout exception:

            verbose: Determines what is written to the log files.  Can be equal to 2, 1, or 0

        Returns:
            An initialized FileWriter object.
        '''
        
        self.timeout_duration = timeout_duration
        self.verbose = verbose
        self._file_dest_data = '' #The file that will be written to by the program
        self._source_path = ''  #The source path of the file that the user inputs to be downlinked
        self._dest_path = ''    #The name and path of the file that will be written to
        self._log_file = ''     #The log file
        self.first_time = True  #The first packet that runs through requires a check to avoid errors so this boolean is used
        self.timer = threading.Timer(self.timeout_duration, self.check_timeout, args=()) #Timer to check for timeouts
        self.state = 'IDLE' #The state machine of the program.  Can be either IDLE or ACTIVE
        self.dest_is_open = False  #Boolean for checking whether or not the destination file is open or closed
        self.log_is_open = False  #Boolean for checking whether or not the log file is open or closed

        super(FileWriter, self).__init__()


    #Getter for the destination file
    def get_file(self):
        return self._file_dest_data

    #Setter for the destination file
    def set_file(self, x):
        self._file_dest_data = x

    #Getter for the log file
    def get_log(self):
        return self._log_file

    #Setter for the log file
    def set_log(self, x):
        self._log_file = x

    #Setter for the timeout_duration
    def set_timeout_duration(self, x):
        self.timeout_duration = x

    def set_verbose(self, x):
        self.verbose = x


    def data_callback(self, data):
        '''
        Function called to pass data to the writer class

        Args:
            data: Binary data that has been decoded and passed to the correct consumer
        '''

        #The first time through the loop requires a special case for the timeout to not fail
        if (self.first_time):
            self.timer = threading.Timer(self.timeout_duration, self.check_timeout, args=())
            self.timer.start()
            self.first_time = False

        self.write_to_file(data)

    #Write to the file
    def write_to_file(self, data):
        '''
        Writes the given data to the correct file.  This function creates the
        directories for the log files and the destination files.  After creating
        the destination file this function then writes all of the provided data
        to it.

        Args:
            data: data that has been decoded and is ready to be written to a file

        Returns:
            None
        '''
        #If the program gets to the next packet before reaching the timeout duration, then cancel the timer
        self.timer.cancel()

        #Start a timer to check for a timeout error
        self.timer = threading.Timer(self.timeout_duration, self.check_timeout, args=())
        self.timer.start()

        #Write to the file here
        packetType = data.packetType

        #Packet Type determines the variables following the seqID
        if (packetType == 'START'):
            #Initialize all relavent START packet attributes into variables from file_data
            size = data.size
            lengthSP = data.lengthSP
            sourcePath = data.sourcePath
            lengthDP = data.lengthDP
            destPath = data.destPath

            if (self.state == 'IDLE'):
                #Create the log file where the soucePath and lengthDP will be placed
                self.create_log_file()
                log_file = self.get_log()
                if (self.log_is_open and self.verbose > 0):
                    log_file.write(self.get_datetime() + " Received START packet\n")
                    log_file.write("START packet meta data:\n")
                    log_file.write("\tSize: " + str(size) + "\n")
                    log_file.write("\tLength of Source Path: " + str(lengthSP) + "\n")
                    log_file.write("\tSource Path: " + str(sourcePath) + "\n")
                    log_file.write("\tLength of Destination Path: " + str(lengthDP) + "\n")
                    log_file.write("\tDestination Path: " + str(destPath) + "\n")


                #Create the destination file where the DATA packet data will be stored
                self.create_dest_file(destPath)
                
                self.state = 'ACTIVE'
            elif (self.state == 'ACTIVE'):
                #A new start packet was found in the middle of the downlink.  Close current file and open a new one
                log_file = self.get_log()
                file_dest = self.get_file()
                if (self.dest_is_open):
                    file_dest.close()
                    self.dest_is_open = False

                if (self.log_is_open):
                    log_file.write(self.get_datetime() + " ERROR: Received a START packet out of order")

                #Create the destination file where the DATA packet data will be stored
                self.create_dest_file(destPath)
        elif (packetType == 'DATA'):
            #Initialize all relevant DATA packet attributes into variables from file_data
            offset = data.offset
            dataVar = data.dataVar
            log_file = self.get_log()
            file_dest = self.get_file()

            if (self.state == 'IDLE'):
                if (self.log_is_open):
                    log_file.write(self.get_datetime() + " ERROR: Recieved DATA packet out of order at offset: " + str(offset) + "\n")
            elif (self.state == 'ACTIVE'):
                if (self.dest_is_open):
                    #Write the data information to the file
                    file_dest.seek(offset, 0)
                    file_dest.write(dataVar)    #write the data information to the destination file
                    file_dest.flush()
                if(self.log_is_open and self.verbose == 2):
                    log_file.write(self.get_datetime() + " Received DATA packet at offset: " + str(offset) + "\n")
                    log_file.flush()

        elif (packetType == 'END'):
            #Initialize all relevant END packet attributes into varibles from file_data
            #hashValue attribute is not relevent right now, but might be in the future
            log_file = self.get_log()
            file_dest = self.get_file()

            if (self.state == 'IDLE'):
                if (self.log_is_open):
                   log_file.write(self.get_datetime() + " ERROR: Received END packet out of order\n")
                   log_file.flush()
                   log_file.close()
            elif (self.state == 'ACTIVE'):
                if (self.dest_is_open):
                    file_dest.flush()
                    file_dest.close()
                    self.dest_is_open = False

                if (self.log_is_open and self.verbose > 0):
                    log_file.write(self.get_datetime() + " Received END packet\n")
                    log_file.write(self.get_datetime() + " DONE: Successfully finished downlink\n")
                    log_file.flush()
                    log_file.close()
                    self.log_is_open = False

            self.first_time = True
            self.state = 'IDLE'
            self.timer.cancel()
        elif (packetType == 'CANCEL'):
            #CANCEL Packets have no data
            file_dest = self.get_file()
            log_file = self.get_log()

            if (self.dest_is_open):
                file_dest.flush()
                file_dest.close()
                self.dest_is_open = False

            if (self.log_is_open and self.verbose > 0):
                log_file.write(self.get_datetime + " Received a CANCEL packet\n")
                log_file.flush()
                log_file.close()
                self.log_is_open = False
                
            self.first_time = True
            self.state = 'IDLE'
        else:
            log_file = self.get_log()
            if (self.log_is_open):
                log_file.write(self.get_datetime() + " Invalid file detected\n")


    #Create the log file for this downlink
    def create_log_file(self):
        '''
        Creates the log file which contains information about the source
        path and length

        This function allows for the write_to_file method to use its START
        packets to create log files that are available to be viewed by
        the user.  The log contains the source path the user entered
        as well as the length of the destination path.

        Args:
            sourcePath: the source path that the user has entered into
            the program

        Returns:
            None
        '''

        #Create the file_decoder_logs folder if it does not already exist
        log_path = os.getcwd() + '/file_decoder_logs'
        if not (os.path.exists(log_path)):
            try:  
                os.makedirs(log_path)
            except OSError:  
                pass
            else:  
                pass

        #Get the current date time to create a new log file folder
        #Use the dates and time as the folder name
        time = datetime.datetime.now()
        log_path = log_path + '/' + str(time.year) + '_' + str(time.month) + '_' + str(time.day) + '_' + str(time.hour) + '_' + str(time.minute) + '_' + str(time.second)
        try:  
            os.makedirs(log_path)
        except OSError:  
            pass
        else:  
            pass

        #Create a new file to write to
        new_log = log_path + '/send_file'
        new_file = open(new_log, 'a')
        temp_str = self.get_datetime()[1:len(self.get_datetime()) - 1]
        new_file.write("Log File Datetime: " + temp_str + "\n")
        self.set_log(new_file)
        self.log_is_open = True

    
    #Create the destination file for this downlink
    def create_dest_file(self, destPath):
        '''
        Creates the destination file that contains the contents of
        the data varaible
        
        This function creates the file that the write_to_file function
        can write to.  The method also creates the folder and path
        that the file will be put into.  It also creates and opens
        the appendable file to be used

        Args:
            destPath: the destination path the user inputs which is
            used to create the folder path and file

        Returns:
            None
        '''

        #First, create the file_downlink folder if it does not already exist
        log_path = os.getcwd() + '/file_downlink/'
        if not (os.path.exists(log_path)):
            try:
                os.makedirs(log_path)
            except OSError:
                pass
            else:
                pass

        #Get rid of all the leading '/'
        for x in str(destPath[0:]):
            if (str(destPath[0]) == '/'):
                destPath = destPath[1:]

        #Figure out where the last '/' is in the file path (if any)
        location = 0
        x = 0
        new_dest_path = log_path + destPath
        for x in range(len(str(new_dest_path[0:]))):
            if str(new_dest_path[x]) == '/':
                location = x

        #We need to create the directory first before we create/open the file so make a temp string
        #without the file at the end of it
        temp_dest_path = new_dest_path[0:location + 1]

        #Create the directory if it does not already exist and then create/open the file
        if not (os.path.exists(temp_dest_path)):
            try:
                os.makedirs(temp_dest_path)
            except OSError:
                pass
            else:
                pass
        
        #Create/open the new file
        data_file = open(new_dest_path, 'w+')

        #Set the file using the setter for other functions to be able to use the new file
        self.set_file(data_file)
        self.dest_is_open = True
    

    #Small function that is called to check if the file writer has timed out
    def check_timeout(self):
        file_dest = self.get_file()
        log_file = self.get_log()
        if(self.log_is_open):
            log_file.write("ERROR: Downlink has timed out. State machine has been reset to IDLE\n")
            log_file.flush()
            log_file.close()
            self.log_is_open = False
        if(self.dest_is_open):
            file_dest.flush()
            file_dest.close()
            self.dest_is_open = False
        self.state = 'IDLE'


    #Gets current datetime in form of: [month/day/year | hour:minute:second]
    def get_datetime(self):
        time = datetime.datetime.now()
        temp = "[" + str(time.month) + "/" + str(time.day) + "/" + str(time.year) + " | " + str(time.hour) + ":" + str(time.minute) + ":" + str(time.second) + "]"
        return str(temp)


if __name__ == "__main__":
    pass