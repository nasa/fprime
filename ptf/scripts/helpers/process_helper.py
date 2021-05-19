# This module has a number of functions related to backgrounding processes
import readline
import signal
import os
import sys
import errno
import subprocess

import scripts.helpers.helper

def getstatusoutput(cmd):
    '''
    Replaces `commands.getstatusoutput` for use in python 2/3 code. This is a wrapper layer to virtualize old python 2
    code with new python 3 syntax and libraries.
    @param cmd: command to run
    '''
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    (out, err) = process.communicate()
    if sys.version_info >= (3, 0):
        out = out.decode("utf-8")
    assert err is None, "Failed to force standard error to standard out"
    return (process.returncode, out)

class BackgroundProcess(object):

	def __init__(self):
		self.pid = 0

	def start(self, executable, arguments, stdout_file, stderr_file, env = None):

		cmd = (executable,) + arguments
		
		new_env = {}
		# copy current environment
		for entry in list(os.environ.keys()):
			new_env[entry] = os.environ[entry]
		 
		# copy additional environment variables
		if (env != None):
			for entry in list(env.keys()):
				new_env[entry] = env[entry]
				
		stdoutfile = open(stdout_file,'w')
		stderrfile = open(stderr_file,'w')
		self.pid = subprocess.Popen(args=cmd, stdout=stdoutfile, stderr=stderrfile, env=new_env).pid
			
	def kill(self, sig=signal.SIGINT):
		try:
			os.killpg(self.pid,sig)
		except OSError as e :
			if e.errno != errno.ESRCH and e.errno != errno.ECHILD :
			    raise

	def getpid(self):
		return self.pid

	def wait(self):
		try:
			(pid, rawstatus) = os.waitpid(self.pid,0)
			assert pid == self.pid,"Process Helper: Returned PID != expected PID"
			# mask and shift real status
			status = (rawstatus & 0xFF00) >> 8
			return status
		except:
			print("Process %i already exited or interrupted." % self.pid)
			return None
	
class BackgroundProcessWithPipe(object):
	def __init__(self):
		self.pid = 0
	def start(self, executable, arguments, stdouterr_file = None):

		cmd = (executable,) + arguments
		# print "BackgroundProcessWithPipe Command line: " + " ".join(cmd)
		
		# open pipes for child i/o
		if (stdouterr_file == None):
			self.outputPipe = os.pipe()
		else:
			self.outputPipe = None

		self.inputPipe = os.pipe()
		# fork to start child process
		self.pid = os.fork()
		if self.pid == 0:
			# in child
			# map stdout and stderr to file
			if (stdouterr_file != None):
				stdoutfile = open(stdouterr_file,'w')
				stdoutfile.write("Command line: " + " ".join(cmd))
				os.dup2(stdoutfile.fileno(),1)
			else:
				os.dup2(self.outputPipe[1],1)
				os.close(self.outputPipe[0])
				
			# send stderr to stdout
			os.dup2(1,2)

			# execute program
			os.dup2(self.inputPipe[0],0)
			os.close(self.inputPipe[1])
			os.execv(executable, cmd) 
		else:
			# close parent end of pipes 
			if (stdouterr_file == None):
				os.close(self.outputPipe[1])
			os.close(self.inputPipe[0])
			
	def kill(self, sig=signal.SIGINT):
		try:
			os.kill(self.pid,sig)
		except OSError as e :
			if e.errno != errno.ESRCH :
			    raise
	def send(self, input):
		os.write(self.inputPipe[1],input)
	def read(self,num_chars,flush_first = True):
		return os.read(self.outputPipe[0],num_chars)
	def getpid(self):
		return self.pid
	def wait(self):
		try:
			(pid, rawstatus) = os.waitpid(self.pid,0)
			assert pid == self.pid,"Process Helper: Returned PID != expected PID"
			# mask and shift real status
			status = (rawstatus & 0xFF00) >> 8
			return status
		except:
			print("Process %i already exited or interrupted." % self.pid)
			return None
			
class BackgroundProcessHelper(scripts.helpers.helper.Helper):
	def __init__(self):
		scripts.helpers.helper.Helper.__init__(self)
		self.process_helper = None
	def start(self, executable, arguments, stdout_file, stderr_file, env = None):
		self.process_helper = scripts.helpers.process_helper.BackgroundProcess()
		self.process_helper.start(executable, arguments, stdout_file, stderr_file, env)
	def exit(self):
		if (self.process_helper != None):
			self.process_helper.kill()
	def wait(self):
		if (self.process_helper != None):
			self.process_helper.wait()
		
