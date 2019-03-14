# Base class for target helpers. Actual target helpers will
# derive from this one

import scripts.helpers.helper

class MasterHelper(scripts.helpers.helper.Helper):
	def __init__(self):
		scripts.helpers.helper.Helper.__init__(self)
	def start_master(self):
		print("base class function")
	def run(self, ticks):
		print("base class function")
	def exit(self):
		print("base class function")
	def wait(self):
		print("base class function")
		
