import sys
import os
import logging
import exceptions
import subprocess
import Queue
import time
from enum import Enum
from subprocess import PIPE


from fprime.common.models.serialize.u32_type import *
from fprime.common.models.serialize.filepacket import *
from fprime_gds.tkgui.utils.checksum import Checksum
from fprime_gds.tkgui.utils import Logger

from optparse import OptionParser

from fprime_gds.tkgui.controllers import client_sock

###
#  file_uplink_client.py
#
#  Handles file uplink to target application
#  via socket server.
###

# Module logger
# Defaults to an null logger.
# Uplink logger can be set via
# file_uplink_client.set_logger()
# or UplinkShell.set_logger()
LOGGER_NAME = __name__
LOGGER      = logging.getLogger("NULL")
LOGGER.addHandler(logging.NullHandler)


class UplinkStatus(Enum):
    SUCCESS    = 0
    CONNECTION_ERR = -1
    FILE_ERR       = -2

def set_logger(log_folder=None, log_name="uplink.log"):
    """
    Set the module level logger.
    @param log_folder: uplink log folder
    @param log_name: uplink log name (default="uplink.log")
    """
    global LOGGER

    if log_folder is None or log_folder == 'None':
        f = None
    else:
        f = os.path.join(log_folder, log_name)

    LOGGER = Logger.connectOutputLogger(file=f, logger_name=LOGGER_NAME, logger_level=logging.INFO)

def get_uplink_server_socket(host, port):
    """
    Register uplink process to server.
    @param host: Server Address
    @param port: Server Port
    @return: Socket object
    """
    global LOGGER

    sock = client_sock.ClientSocket(host, port)

    LOGGER.info("Registering UPLINK to server address {} on port {}".format(opts.host, opts.port))
    sock.send("Register GUI\n")

    return sock


def send_file(src_path, dest_path, sock, offset=0, data_size=64, subprocess=False):
    """
    Send a file to the FSW target application.
    @param src_path: Source path of file to be sent.
    @param dest_path: Destination path of file to be received by FSW application.
    @param offset: Byte offset into the source file (0 by default).
    @param data_size: Size of data packets (in bytes) being sent to FSW application.
    """
    global LOGGER

    # Count bytes sent
    total_sent = 0
    data_sent  = 0
    seq_idx = 0

    # Check socket connection
    if sock == None:
       LOGGER.info("File %s not sent: No socket connection" % src_path)
       return (UplinkStatus.CONNECTION_ERR.value, 0)

    # Open file for reading
    try:
        src_file = open(src_path, 'rb')
    except Exception, exc:
        LOGGER.info("Unable to open file: ", src_path, ". Exception:", exc)
        return (UplinkStatus.FILE_ERR.value, 0)

    LOGGER.info("Sending Ground File: {}".format(src_path))
    LOGGER.info("Target Destination:  {}".format(dest_path))

    # Package and send immediate command here...
    desc = U32Type( 0x5A5A5A5A )

    # Added desc. type for FSW to know it is a file (3).
    desc_type = U32Type(3)

    file_data = src_file.read()

    # Add null character to paths
    src_path = src_path + "\0"
    dest_path = dest_path + "\0"

    # Prep START packet
    start_pkt = StartPacket(seq_idx, len(file_data), src_path, dest_path)
    seq_idx += 1

    if subprocess:
        print "<s|{}|{}>\n".format(len(file_data), src_path),
        sys.stdout.flush()

    pkt_len = U32Type( start_pkt.getBufSize() + desc_type.getSize() )
    cmd = "A5A5 " + "FSW " + desc.serialize() + pkt_len.serialize() + desc_type.serialize() + start_pkt.serialize()
    total_sent += pkt_len.val

    #print "Bytes to be sent to socket:"
    #type_base.showBytes(cmd)


    # Send START packet
    sock.send(cmd)

    # Prep DATA packets
    for file_idx in range(0, len(file_data), data_size):
        file_chunk = file_data[file_idx:(file_idx + data_size)]
        data_pkt = DataPacket(seq_idx, file_idx, len(file_chunk), file_chunk)

        pkt_len = U32Type( data_pkt.getBufSize() + desc_type.getSize() )
        cmd = "A5A5 " + "FSW " + desc.serialize() + pkt_len.serialize() + desc_type.serialize() + data_pkt.serialize()

        #print "Bytes to be sent to socket:"
        #type_base.showBytes(cmd)

        # send DATA packets
        sock.send(cmd)
        seq_idx += 1

        data_sent += pkt_len.val
        total_sent += pkt_len.val

        cur_sent = float(file_idx)/len(file_data)*100
        if subprocess:
            print "<d|{}|{}>\n".format(cur_sent, data_sent),
            sys.stdout.flush()



    if subprocess:
        print "<e|x_prep>\n",

    # Prep END packet
    checksum = Checksum()

    #TODO: assuming file offset of 0 for now
    checksum.update(file_data, 0, len(file_data))

    end_pkt = EndPacket(seq_idx, checksum.xSum)
    pkt_len = U32Type( end_pkt.getBufSize() + desc_type.getSize() )
    cmd = "A5A5 " + "FSW " + desc.serialize() + pkt_len.serialize() + desc_type.serialize() + end_pkt.serialize()
    total_sent += pkt_len.val

    #print "Bytes to be sent to socket:"
    #type_base.showBytes(cmd)

    # Send END packet
    sock.send(cmd)

    LOGGER.info("Total Bytes sent: {}".format(total_sent))

    if subprocess:
        print "<e|{}>\n".format(total_sent),
        sys.stdout.flush()

    src_file.close()

    return (UplinkStatus.SUCCESS.value, total_sent)



class UplinkShell(object):
    """
    Singlton shell to hold uplink options
    and user api. Only one instance available
    to prevent multiple uploads.
    """

    __instance = None

    def __init__(self):
        """
        Constructor
        @param build_root: Top level fsw directory
        @param log_path: Path to uplink log folder
        @param log_name: Name of log file

        - If log_path is None process will start without a logger.
        - If an uplink is in progress add to uplink queue
        """

        #Uplink process
        self.__UPLK       = None
        self.__build_root = None
        self.__log_folder = None
        self.__log_name   = None

        self.__queue      = Queue.Queue()

    def config(self, build_root, log_folder, log_name="uplink.log"):
        """
        Configure the singleton.
        """
        self.__build_root = build_root
        self.__log_name   = log_name
        self.__log_folder = log_folder


    def get_queue(self):
        return self.__queue

    def __uplink_available(self):
        """
        Check if an uplink slot
        is available on server.
        """

        # TODO: Implement
        return True
    def spawn(self, host, port, src_path, dest_path):
        """
        Create uplink process command and place in queue.

        @param host: TCP Server address
        @param port: TCP Server port
        @param src_path: Filepath to upload
        @param dest_path: File destination path on target system
        @param build_root: Top level fsw directory
        """

        offset = 0
        data_size = 256

        uplink_path = self.__build_root +os.sep+ 'Gds' + os.sep + 'fprime_gds' + os.sep + 'tkgui' + os.sep + 'controllers' + os.sep + 'file_uplink_client.py'

        # We need to know the python base
        # before we begin the subprocess.
        cmd         = "python " + uplink_path + " -p {port} -i {host} -f {src} -d {dest} -o {off} -s {size} -l {log_folder}".format( \
                                  port=port, host=host, src=src_path, dest=dest_path, \
                                  off=offset, size=data_size, log_folder=self.__log_folder)

        self.__queue.put(cmd)


    def __quickStart(self,cmd):
        env_cpy = os.environ.copy()
        return subprocess.Popen(cmd, stdin=PIPE, stderr=PIPE, stdout=PIPE, preexec_fn=os.setsid, universal_newlines=True, env=env_cpy, shell=True)

    def startNext(self):
        """
        Start next uplink cmd in queue.
        """
        try:
            # Wait for server to be available.
            while not self.__uplink_available():
                pass

            cmd = self.__queue.get_nowait()
            self.__UPLK = self.__quickStart(cmd)
            return self.__UPLK
        except Queue.Empty:
            return None

    def getInstance():
        """
        Return instance of singleton.
        """
        if(UplinkShell.__instance is None):
            UplinkShell.__instance = UplinkShell()
        return UplinkShell.__instance

    #define static method
    getInstance = staticmethod(getInstance)



if __name__ == '__main__':

    parser = OptionParser(description="File Uplink Client")
    parser.add_option("-p", "--port", dest="port", action="store", type="int", help="Set threaded tcp socket server port [default: %default]", \
                     default=50007)
    parser.add_option("-i", "--host", dest="host", action="store", type="string", help="Set threaded tcp socket server ip [default: %default]", \
                     default="127.0.0.1")
    parser.add_option("-f", "--filepath", dest="filepath", action="store", type="string", help="File source path", default=None)
    parser.add_option("-d", "--destpath", dest="destpath", action="store", type="string", help="Destination path", default=None)
    parser.add_option("-o", "--offset", dest="offset", action="store", type="int", help="Byte offset into the source file (0 by default).", \
                      default=0)
    parser.add_option("-s", "--data_size", dest="data_size", action="store", type="int", \
                      help="Size of data packets (in bytes) being sent to FSW application (default = 512).", default=512)
    parser.add_option("-l", "--logfolder", dest="logfolder", action="store", type="string", help="Log folder path", default=None)

    # process options
    (opts, args) = parser.parse_args(sys.argv[1:])


    LOGGER.info("----- UPLOAD START -----")

    set_logger(opts.logfolder)
    sock = get_uplink_server_socket(opts.host, opts.port)
    status, bytes_sent = send_file(opts.filepath, opts.destpath, sock, opts.offset, opts.data_size, subprocess=True)

    if status == UplinkStatus.UPLINK_OKAY:
        LOGGER.info("----- UPLOAD COMPLETE -----\n\n")
        sys.exit(0)
    else:
        LOGGER.info("----- UPLOAD ERROR: {} -----\n\n\n".format(status))
        sys.exit(-1)
