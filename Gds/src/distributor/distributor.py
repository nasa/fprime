import threading
import socket
import select
import binascii

from serializable import u32_type
from utils import data_desc_type

# NOTE decoder function to call is called data_callback(data)
# TODO close the socket when you're done
class Distributor(object):

	def __init__(self):
		self.__decoders = dict()
		self.__threaded_socket_client = ThreadedTCPSocketClient(self)
		self.__data_recv_thread = None
		
	
	def disconnect(self):
		self.__threaded_socket_client.stop_event.set()
		self.__data_recv_thread.join()

	def connect(self, host, port):
		"""Connect the internal socket client to the server
		
		Arguments:
			host {string} -- host IP address
			port {int} -- host port
		"""

		self.__threaded_socket_client.connect(host, port)
		self.__data_recv_thread = threading.Thread(target=self.__threaded_socket_client.recv)
		self.__data_recv_thread.start()

	# NOTE we could use either the type of the object or an enum as the type argument. It should indicate what the decoder decodes.
	# TODO implement as an ENUM
	def register(self, typeof, obj):
		self.__decoders[typeof].append(obj)

	def send(self, data):
		self.__threaded_socket_client.send(data)
	
	def on_recv(self, data):
		# NOTE make these selectable with a configuration later

		offset = 0 # Represents our offset into the packet so far

		# 4 byte length header for sanity
		pkt_len = u32_type.U32Type()
		pkt_len.deserialize(data, offset)
		offset += pkt_len.getSize()
		
		if len(data) - offset != pkt_len.val:
			#TODO the packet length is not right
			pass

		data_desc = u32_type.U32Type()
		data_desc.deserialize(data, offset)
		offset += data_desc.getSize()

		data_pass_thru = data[offset:] # This will get passed up to decoders

		if data_desc.value == data_desc_type.DataDescType.FW_PACKET_COMMAND:
			pass
		elif data_desc.value == data_desc_type.DataDescType.FW_PACKET_TELEM:
			pass		
		elif data_desc.value == data_desc_type.DataDescType.FW_PACKET_LOG:
			pass		
		elif data_desc.value == data_desc_type.DataDescType.FW_PACKET_FILE:
			pass
		elif data_desc.value == data_desc_type.DataDescType.FW_PACKET_PACKETIZED_TLM:
			pass
		elif data_desc.value == data_desc_type.DataDescType.FW_PACKET_IDLE:
			pass
		elif data_desc.value == data_desc_type.DataDescType.FW_PACKET_UNKNOWN:
			pass	
		
		
class ThreadedTCPSocketClient(object):
	
	def __init__(self, distributor, sock=None):
		if sock is None: 
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		else:
			self.sock = sock
		
		# NOTE can't do this b/c EINPROGRESS: self.sock.setblocking(0)

		self.__distributor = distributor
		self.__select_timeout = 1
		self.stop_event = threading.Event()

	def connect(self, host, port):
		self.sock.connect((host, port))

	def send(self, data):
		self.sock.send(data)
		pass

	def recv(self):

		while not self.stop_event.is_set():
			print "Running distributor client recv..."
			ready = select.select([self.sock], [], [], self.__select_timeout)
			if ready[0]:
				chunk = self.sock.recv(1024)
				self.__distributor.on_recv(chunk)
				print(binascii.hexlify(chunk))

	

