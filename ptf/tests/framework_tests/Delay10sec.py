#!/dsw/Python-2.4.3r1_32/bin/python

import time
import signal
import sys

def sighandler(signum, frame):
	if signum == signal.SIGINT:
		print("Received SIGINT! Exiting.")
		sys.exit(-1)
			
signal.signal(signal.SIGINT,sighandler)

time.sleep(10)