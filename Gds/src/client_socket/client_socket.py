import socket
import threading
import binascii
import select

from pprint import pprint

from models.serialize import u32_type

class ThreadedTCPSocketClient(object):
	'''Threaded TCP client that connects to teh socket server which serves packets from the helecopter'''

	def __init__(self, sock=None):
		"""Threaded client socket constructor
		
		Keyword Arguments:
			sock {Socket} -- A socket for the client to use. Created own if None (default: {None})
		"""

		if sock is None:
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		else:
			self.sock = sock

		# NOTE can't do this b/c EINPROGRESS: self.sock.setblocking(0)

		self.__distributors = []
		self.__select_timeout = 1
		self.__data_recv_thread = threading.Thread(target=self.recv)
		self.stop_event = threading.Event()

	def register_distributor(self, distributor):
		"""Registers a distributor object with this socket
		
		Arguments:
			distributor {Distributor} -- Distributor must implement data_callback
		"""

		self.__distributors.append(distributor)

	def connect(self, host, port):
		"""Connect to host at given port and start the threaded recv method.

		Arguments:
			host {string} -- IP of the host server
			port {int} -- Port of the host server
		"""
		try:
			self.sock.connect((host, port))
			self.__data_recv_thread.start()
		except:
			print("There was a problem connecting to the TCP Server")
                        exit()

	def disconnect(self):
		"""Disconnect the socket client from the server and stop the internal thread.
		"""

		self.stop_event.set()
		self.__data_recv_thread.join()
		self.sock.close()

	def send(self, data):
		"""Send data to the server

		Arguments:
			data {binary} -- The data to send
		"""
		#pprint(data)
		self.sock.send(data)

	# TODO Find oUt why we are getting doubled packets
	def recv(self):
		"""Method run constantly by the enclosing thread. Looks for data from the server.
		"""

		while not self.stop_event.is_set():
			#print "Running distributor client recv..."
			ready = select.select([self.sock], [], [], self.__select_timeout)
			if ready[0]:
				chunk = self.sock.recv(1024)
				for d in self.__distributors:
					d.on_recv(chunk)
				#print(binascii.hexlify(chunk))

                                


