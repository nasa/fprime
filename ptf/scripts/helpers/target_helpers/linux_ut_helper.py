import scripts.helpers.target_helper
import scripts.helpers.process_helper
import subprocess
import os
import signal
import time
import scripts.helpers.make_helper

class LinuxUtHelper(scripts.helpers.target_helper.TargetHelper):
	def __init__(self):
		scripts.helpers.target_helper.TargetHelper.__init__(self)
		self.module = ""
		self.test_status = 0
	def set_module(self, module):
		self.module = module
	def make(self):
		self.test_status = scripts.helpers.make_helper.MakeHelper().make_module(self.module,"ut_linux")
	def run(self):
		self.test_status = scripts.helpers.make_helper.MakeHelper().make_module(self.module,"run_ut_linux")
	def do_test(self, module):
		self.set_module(module)
		self.make()
		self.run()
		return self.test_status
