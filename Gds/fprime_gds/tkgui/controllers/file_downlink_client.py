#!/bin/env python

import sys
import os
import logging
import subprocess
from enum import Enum
from subprocess import PIPE
from optparse import OptionParser

from fprime_gds.tkgui.controllers import client_sock
from fprime.common.models.serialize.u32_type import *
from fprime.common.models.serialize.u8_type import *
from fprime_gds.tkgui.utils.checksum import *
from fprime_gds.tkgui.utils import Logger

from fprime.common.models.serialize.filepacket import PacketType
from fprime.common.models.serialize import filepacket


# Module logger
# Defaults to an null logger.
# Uplink logger can be set via
# file_uplink_client.set_logger()
# or UplinkShell.set_logger()
LOGGER_NAME = __name__
LOGGER      = logging.getLogger("NULL")
LOGGER.addHandler(logging.NullHandler)


class DownlinkStatus(Enum):
	CANNOT_OPEN_FILE      = -3
	CANNOT_WRITE_TO_FILE  = -2
	X_SUM_DIFFERENCE      = -1
	SUCCESS               = 0


def set_logger(log_folder=None, log_name="downlink.log"):
    """
    Set the module level logger.
    @param log_folder: downlink log folder
    @param log_name: downlink log name (default="downlink.log")
    """
    global LOGGER

    if log_folder is None or log_folder == 'None':
        f = None
    else:
        f = os.path.join(log_folder, log_name)

    LOGGER = Logger.connectOutputLogger(file=f, logger_name=LOGGER_NAME, logger_level=logging.INFO)



def startListen(sock, subprocess=False):
	"""
	Start listening for incoming file packets.
	Ignore anything that is not a file packet.
	"""
	LOGGER.info("STARTING LISTEN\n")
	check_sum = Checksum()

	# Subprocess variables
	filesize  = 0
	data_recv = 0

	while(1):
		try:
			# Get pkt_len, and message type (event, tlm, or file)
			msg = sock.recv(8)

			# 4 Bytes before start packet
			desc_size = 4

			desc = 0
			size = 0

			try:
			  size, desc = __processHeader(msg)
			except IOError:
			  LOGGER.info("Header data incorrect\n")
			  continue

			if desc != 3:
			  #LOGGER.info("Not a file, ignoring\n")
			  # read rest of packet but don't process it
			  sock.recv(size - desc_size)
			  continue
			LOGGER.info("File Packet Received")
			LOGGER.info("Size: {}".format(size))


			#print "Processing."
			ptr = 0
			file_msg = sock.recv(size-4)


			pkt_type = U8Type()
			pkt_type.deserialize(file_msg, ptr)
			ptr += pkt_type.getSize()
			#print "pkt_type = 0x%x" % pkt_type.val

			seq_idx = U32Type()
			seq_idx.deserialize(file_msg, ptr)
			ptr += seq_idx.getSize()
			#LOGGER.info("seq_idx = 0x%x" % seq_idx.val)


			LOGGER.info("PKT TYPE: {}".format(pkt_type.val))



			##
			# Process Packets
			##
			if PacketType(pkt_type.val) == PacketType.START:
			  LOGGER.info("START PACKET\n")

			  # Open destination file
			  start_packet = filepacket.StartPacket()
			  start_packet.deserialize(file_msg)
			  filesize  = start_packet.getFileSize().val
			  dest_path = start_packet.getDestPath()


			  if subprocess:
			  	print "<s|{}|{}>\n".format(filesize , dest_path),
			  	sys.stdout.flush()

			  #LOGGER.info("DEST_LEN: {}".format(start_packet.getDestLen().val))
			  #LOGGER.info("DEST: {}".format(dest_path))
			  #LOGGER.info("SRC : {}".format(start_packet.getSrcPath()))

			  # Open file for writing
			  try:
			     dest_file = open(dest_path, 'a')
			  except Exception, exc:
			     LOGGER.info("Unable to open file: {}. Exception: {}\n".format(dest_path, exc))
			     return DownlinkStatus.CANNOT_OPEN_FILE.value

			elif PacketType(pkt_type.val) == PacketType.DATA:
			  LOGGER.info("DATA PACKET\n")

			  data_packet = filepacket.DataPacket()
			  data_packet.deserialize(file_msg)
			  data        = data_packet.getData()
			  data_len    = data_packet.getDataSize()

			  data_recv += data_len.val
			  progress = data_recv/float(filesize)*100
			  if subprocess:
			  	print "<d|{}|{}>\n".format(progress, data_recv),
			  	sys.stdout.flush()

			  # Write data to file
			  try:
			     dest_file.write(data)
			  except Exception, exc:
			     LOGGER.info("Unable to write data packet to file. Exception: {}\n".format(exc))
			     return DownlinkStatus.CANNOT_WRITE_TO_FILE.value

			  # Update checksum
			  check_sum.update(data, 0, data_len)

			elif PacketType(pkt_type.val) == PacketType.END:
			  LOGGER.info("END PACKET\n")

			  end_packet = filepacket.EndPacket()
			  end_packet.deserialize(file_msg)

			  # Checksum
			  if subprocess:
			  	print "<e|x_prep>\n",
			  	sys.stdout.flush()

			  msg_xSum   = end_packet.getChecksum()
			  local_xSum = check_sum.xSum
			  LOGGER.info("msg   xSUM: {}".format(msg_xSum.val))
			  LOGGER.info("local xSUM: {}".format(local_xSum))

			  # Close file
			  try:
			     dest_file.close()
			  except Exception, exc:
			     LOGGER.info("Unable to close file. Exception: {}\n".format(exc))

			  # Reset Checksum
			  #check_sum.xSum(0)
			  LOGGER.info("Filedownlink Finished\n")

			  if subprocess:
			  	print "<e|{}>\n".format(data_recv),
			  	sys.stdout.flush()

			  # Subprocess will continue to listen
			  if not subprocess:
				  if msg_xSum.val - local_xSum != 0:
				  	return DownlinkStatus.X_SUM_DIFFERENCE.value
				  else:
				  	return DownlinkStatus.SUCCESS.value
			  else:
			      # Reset subprocess variables
			      filesize  = 0
			      data_recv = 0


			elif PacketType(pkt_type.val) == PacketType.CANCEL:
			  LOGGER.info("^ CANCEL PACKET\n")

			# bad file packet type
			else:
			  continue

		except Exception, e:
			LOGGER.info(e)
			continue

def get_downlink_server_socket(host, port):
	"""
	Register downlink process to server.
	@param host: Server Address
	@param port: Server Port
	@return: Socket object
	"""
	global LOGGER

	LOGGER.info("Connecting to server socket addr {} port {} ".format(host,port))
	try:
	  sock = client_sock.ClientSocket(host, port)
	  sock.send("Register GUI\n")
	except IOError:
		LOGGER.info("Unable to conenct.")
		print "Unable to connect with the socket server. Exiting.\n"

	LOGGER.info("Successful connection.")
	return sock



def __processHeader(msg):
	"""
	"""
	global LOGGER
	ptr = 0

	#u32_obj.deserialize(msg, ptr)


	#delim = u32_obj.val
	#ptr += u32_obj.getSize()

	#LOGGER.info("delim = 0x%x" % delim)
	#if hex(delim) != '0x5a5a5a5a':
	#   raise IOError
	u32_obj = U32Type()
	u32_obj.deserialize(msg, ptr)
	size = u32_obj.val
	ptr += u32_obj.getSize()

	#LOGGER.info("size = 0x%x" % size)

	u32_obj.deserialize(msg, ptr)
	desc = u32_obj.val
	ptr += u32_obj.getSize()

	#LOGGER.info("desc = 0x%x" % desc)

	return (size, desc)




class DownlinkShell(object):
	"""
	Handles incoming file packets from socket server.
	Spawn must be called before start.
	"""

	__instance = None

	def __init__(self):
		self.__build_root = None
		self.__log_folder = None
		self.__check_sum  = Checksum()

		self.__start_cmd  = None

	def config(self, log_folder, build_root):
		"""
		Configure shell.
		@param sock: Socket object
		@oaram build_root: Top level directory
		"""
		self.__build_root = build_root
		self.__log_folder = log_folder

	def start(self):
	 	"""
	 	Start the client if spawn has been called.
	 	"""
	 	if self.__start_cmd:
	 		env_cpy = os.environ.copy()
	 		return subprocess.Popen(self.__start_cmd, stdin=PIPE, stderr=PIPE, stdout=PIPE, preexec_fn=os.setsid, universal_newlines=True, env=env_cpy, shell=True)
	 	else:
	 		return None

	def spawn(self, host, port):
		"""
		Create a start command.
		Let file listener start the process.
		"""

		# Dont allow multiple downlink instances on one machine
		if self.__start_cmd:
			return

		downlink_path = self.__build_root +os.sep+ 'Gds' + os.sep + 'fprime_gds' + os.sep + 'tkgui' + os.sep + 'controllers' + os.sep + 'file_downlink_client.py'

		# We need to know the python base
		# before we begin the subprocess.
		# Set start command. file listener will call if needed
		self.__start_cmd = "python " + downlink_path + " -p {port} -i {host} -l {log_folder}".format( \
		                          port=port, host=host, log_folder=self.__log_folder)


	def getInstance():
		"""
		Return instance of singleton.
		"""
		if(DownlinkShell.__instance is None):
		    DownlinkShell.__instance = DownlinkShell()
		return DownlinkShell.__instance

	#define static method
	getInstance = staticmethod(getInstance)


def main():
   description = "File Downlink Client"
   parser = OptionParser(description=description)
   parser.add_option("-p", "--port", dest="port", action="store", type="int", help="Set threaded tcp socket server port [default: %default]", \
                    default=50007)
   parser.add_option("-i", "--host", dest="host", action="store", type="string", help="Set threaded tcp socket server ip [default: %default]", \
                    default="127.0.0.1")
   parser.add_option("-l", "--logfolder", dest="logfolder", action="store", type="string", help="Log folder path", default=None)

   (opts, args) = parser.parse_args(sys.argv[1:])

   LOGGER.info("---- START ----")

   set_logger(opts.logfolder)
   sock = get_downlink_server_socket(opts.host, opts.port)
   startListen(sock, subprocess=True)

   LOGGER.info("---- END ----")



if __name__ == "__main__":
   sys.exit(main())
