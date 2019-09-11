import signal
import sys


helper_list = []

sighandler_registered = False

def sighandler(signum, frame):
	if signum == signal.SIGINT:
		print("Helper received SIGINT! Exiting helpers...")
		for helper in helper_list:
			helper.exit()
		sys.exit(1)


class Helper(object):
	def __init__(self):
		helper_list.append(self)
		
		if not sighandler_registered:
			signal.signal(signal.SIGINT,sighandler)
		