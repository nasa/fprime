# Base class for target helpers. Actual target helpers will
# derive from this one

import array
import scripts.helpers.helper

class GdsHelper(scripts.helpers.helper.Helper):
	def __init__(self):
		scripts.helpers.helper.Helper.__init__(self)
	def start_gds(self, desc):
		print("base class function")
	def send_frame(self, filename):
		print("base class function")
	def send_command(self, command_text):
		print("base class function")
	def send_sse_command(self, command_text):
		print("base class function")
	def exit(self):
		print("base class function")
	def sendCLTU(self, bytes):
		file = open("cltu.bin",'w')
		cltu = array.array('B',bytes)
		cltu.tofile(file)
		file.close()
		self.send_frame("cltu.bin")
