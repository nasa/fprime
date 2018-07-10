import socket
import threading
import binascii
import select

from models.serialize import u32_type

class ThreadedTCPSocketClient(object):

	def __init__(self, sock=None):
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
		self.stop_event.set()
		self.__data_recv_thread.join()
		self.sock.close()

	def send(self, data):
		"""Send data to the server

		Arguments:
			data {binary} -- The data to send
		"""
		self.sock.send(data)


	def recv(self):
		"""Method run constantly by the enclosing thread. Looks for data from the server.
		"""

		while not self.stop_event.is_set():
			print "Running distributor client recv..."
			ready = select.select([self.sock], [], [], self.__select_timeout)
			if ready[0]:
				chunk = self.sock.recv(1024)
				for d in self.__distributors:
					d.on_recv(chunk)
				print(binascii.hexlify(chunk))

                                


