# Base class for target helpers. Actual target helpers will
# derive from this one
import scripts.helpers.helper

class TargetHelper(scripts.helpers.helper.Helper):
	def __init__(self,deployment,instance_id):
		scripts.helpers.helper.Helper.__init__(self)
	def start_target(self):
		print("base class function")
	def load_fsw(self, fsw_file):
		print("base class function")
	def run_cmd(self, cmd):
		print("base class function")
	def reset(self):
		print("base class function")
	def exit(self):
		print("base class function")
	def wait(self):
		print("base class function")
	def do_test(self, unit_test):
		print("base class function")
