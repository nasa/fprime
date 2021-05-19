# Base class for target helpers. Actual target helpers will
# derive from this one
import scripts.helpers.helper

class SimulationHelper(scripts.helpers.helper.Helper):
	def __init__(self):
		scripts.helpers.helper.Helper.__init__(self)
	def start_simulation(self):
		print("base class function")
	def exit(self):
		print("base class function")
