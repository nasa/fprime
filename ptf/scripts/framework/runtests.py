# *******************************************************************************
# * Copyright 2006, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged.
# *
# * regulations. By accepting this document, the user agrees to comply
# * with all applicable U.S. export laws and regulations.  User has the 
# * responsibility to obtain export licenses,
# * or other export authority as may be required before exporting such
# * information to foreign countries or providing access to foreign
# * persons.
# *

# Set up session variables from environment

import sys
import os
import optparse
import platform
import xml.sax
import subprocess
import scripts.helpers.parsers.env_file_parser
import scripts.helpers.process_helper
import time
import signal
import socket

class bcolors:
    RED = "\u001b[31m"
    ENDC = "\u001b[0m"
    GREEN = "\u001b[32m"

# check arguments

parser = optparse.OptionParser()

parser.add_option("-i",default=None, dest="session_id", help="Set test session id. If not specified, will be generated.")
parser.add_option("-o",default=None, dest="output_dir", help="Set test output directory. If not specified, will be generated.")
parser.add_option("-t",default="linux", dest="target", help="Set target environment.")
#parser.add_option("-t",default="vxsim-6.8_st", dest="target", help="Set target environment.")
parser.add_option("-c",default="gcc-debug",dest="compiler",help="Set compiler name.")
parser.add_option("-s",default="nosim",dest="simulator",help="Set simulation name.")
parser.add_option("-m",default="nomaster",dest="master",help="Set master name.")
parser.add_option("-r",action="store_true",dest="delete_output_dir",help="Delete previous output dir")
parser.add_option("-g",action="store_true",dest="gui_session",help="Use interactive gui")
parser.add_option("-a",default="nogds",dest="gds",help="Set gds program.")
parser.add_option("-d",default="FSW", dest="deployment",help="Specify a deployment")
parser.add_option("-p",default=None, dest="python_debug",help="Debug Python")
parser.add_option("-q",action="store_true", dest="quiet",help="Run with minimal output")

# stash arguments
	
(options,arg) = parser.parse_args(sys.argv)

args = "target=" + options.target + " compiler=" + options.compiler + " simulator=" + options.simulator + " master=" + options.master + " gds=" + options.gds

# only keep environment variables we want to keep
#envs_to_save = ["HOST","TERM","USER","DISPLAY","LOGNAME","BUILD_ROOT"]
#for var in os.environ.keys():
#	if not var in envs_to_save:
#		del os.environ[var]


# Generate session id if not specified

if (options.session_id == None):
	print("Generating session id.")
	session_id = os.getenv("USER") + "--" + time.strftime("%Y-%m-%d-%H-%M-%S",time.gmtime())
	print("Session id: " + session_id)
else:
	session_id = options.session_id
	
os.environ["session_id"] = session_id

# Determine host operating system
if (platform.system() == "Linux"):
#	host = platform.system() + "-" + platform.release().split('.')[0] + "." + platform.release().split('.')[1]
	host = "LINUX"
elif (platform.system().count("CYGWIN")):
	host = "CYGWIN"
elif (platform.system().count("Darwin")):
	host = "DARWIN"
else:
	print("Platform " + platform.system() + " not supported.")
	sys.exit(-1)

if options.quiet == None:
	print("Host: " + host)

# get BUILD_ROOT environment variable
os.environ["HOST"] = host
build_root = os.getenv("BUILD_ROOT")

# check to see if host file exists
host_file = build_root + "/ptf/configurations/host/" + host + ".env"
if (os.path.isfile(host_file) == False):
	print("Host configuration file " + host_file + " does not exist")
	sys.exit(-1)

# read host file
scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(host_file) 

# Check to see if session is interactive
if options.gui_session == True:
	os.environ["SESSION_INTERACTIVE"]="TRUE"
else:
	os.environ["SESSION_INTERACTIVE"]="FALSE"

# Generate output directory if not specified

if (options.output_dir == None):
	if options.quiet == None:
		print("Generating output directory.")
	output_dir = os.environ["HOST_DEFAULT_TEST_DIR"] + "/tests." + session_id
else:
	output_dir = options.output_dir
	
if options.quiet == None:	
	print("Test output to: " + output_dir)

# Remove old directory
if (options.delete_output_dir == True):
	if (os.path.isdir(output_dir)):
		# rm dir doesn't seem to do recursive
		(rmstat,rmstr) = scripts.helpers.process_helper.getstatusoutput(os.environ["HOST_RM_DIR"] + " " + output_dir)
		if (rmstat != 0):
			print("Could not delete" + output_dir + ":" + rmstr)
else:
	if (os.path.isdir(output_dir)):
		print("Destination output directory already exists!")
		sys.exit(-1)
	

# get target name
os.environ["TARGET"] = options.target
# check to see if target file exists
target_file = build_root + "/ptf/configurations/target/" + options.target + ".env"
if (os.path.isfile(target_file) == False):
	print("Target configuration file " + target_file + " does not exist")
	sys.exit(-1)

# read target file 
scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(target_file)  

# get compiler name
os.environ["COMPILER"] = options.compiler
# check to see if compiler file exists
compiler_file = build_root + "/ptf/configurations/compiler/" + options.compiler + ".env"
if (os.path.isfile(compiler_file) == False):
	print("Compiler configuration file " + target_file + " does not exist")
	sys.exit(-1)

# read compiler file 
scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(compiler_file)  

# get master name
os.environ["MASTER"] = options.master
# get master configuration file
master_file = build_root + "/ptf/configurations/master/" + options.master + ".env"
if (os.path.isfile(master_file) == False):
	print("Master configuration file " + master_file + " does not exist")
	sys.exit(-1)
scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(master_file)  

# get GDS
os.environ["GDS"] = options.gds
# read simulator file 
# check to see if gds file exists
gds_file = build_root + "/ptf/configurations/gds/" + options.gds + ".env"
if (os.path.isfile(gds_file) == False):
	print("GDS configuration file " + gds_file + " does not exist")
	sys.exit(-1)
	
scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(gds_file)  

# get simulator

os.environ["SIMULATION"] = options.simulator
# check to see if simulator file exists
simulator_file = build_root + "/ptf/configurations/simulator/" + options.simulator + ".env"
if (os.path.isfile(simulator_file) == False):
	print("Simulator configuration file " + simulator_file + " does not exist")
	sys.exit(-1)

# read simulator file 
scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(simulator_file)  


# get list of tests. Tests are arguments with .py or .sh extensions

test_list = []

for arg in sys.argv:
	if arg != sys.argv[0]:
		if arg.find(".sh") != -1 or arg.find(".py") != -1:
			# check to see if file exists
			if os.path.isfile(arg) == False:
				print("Test file " + arg + " not found.")
				sys.exit(-1)

			# print "Single adding %s" % arg
			test_list.append((os.path.splitext(os.path.basename(arg))[0],os.path.abspath(arg),None,None))
		# Look for test suites
		if arg.find(".suite") != -1:
			# verify that it is a real file
			if os.path.isfile(arg) == False:
				print("Suite file " + arg + " not found.")
				sys.exit(-1)
				
			suite_test_list = open(os.path.abspath(arg),'r').readlines()
			
			suite_line = 1
			
			for test in suite_test_list:
				# remove comments
				test = test.split("#")[0]
				# strip carriage return and any leading white space
				test = test.strip()
				# if non-empty line
				if (len(test)):
					# check for environment variables
					var_start_index = test.find('$(')
					while (var_start_index != -1):
						var_end_index = test.find(")")
	
						if var_end_index == -1:
							raise Exception("Environment variable parse error on line %i of file %s" % (suite_line,arg))
						# extract variable value
						sub_var = test[var_start_index+2:var_end_index]
						# look for variable in environment, if there rebuild val
						if sub_var in os.environ:
							test = test[0:var_start_index] + os.environ[sub_var] + test[var_end_index+1:]
						else:
							raise Exception("Variable %s not found in environment in test suite file %s, line %d" % (sub_var,arg,suite_line))
	
						var_start_index = test.find('$(')
						
					desc = ""
					# get test name
					subline = test.split("\"")
					# if name present
					if len(subline) == 3:
						# element 1 would be name
						desc = subline[1]
						# split test line into script and environment file
						file_list = subline[2].split()
					elif len(subline) == 1:
						file_list = test.split()
					else:
					    print("Badly formed description in test suite file %s, line %d" %(arg,suite_line))
					    sys.exit(-1)
						
					script = file_list[0]
					env_list = []
					env_val_list = []
					if (len(file_list) > 1):
						for entry in file_list[1:]:
							# check for ENV keyword
							if entry.count("ENV("):
								if entry.count("=") == 0:
									print("Badly formed ENV entry in test suite file %s, line %d. Should be ENV(var=val)" %(arg,suite_line))
									sys.exit(-1)
								# extract environment variable to set
								(envvar,envval) = entry[4:-1].split("=")
								env_val_list.append((envvar,envval))
							else:
								env_list.append(os.path.abspath(entry))
					
					# add test
					# print "Suite adding %s" % test
					if (desc == ""):
						desc = os.path.splitext(os.path.basename(script))[0]
						
					test_list.append((desc,os.path.abspath(script),env_list,env_val_list))

				suite_line += 1
				
		if arg.find(".env") != -1:
			
			if os.path.isfile(arg) == False:
				print("ENV config file " + arg + " not found.")
				sys.exit(-1)
			# read env file into environment
			scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(arg)  
			
			
		
# store current directory
current_dir = os.path.abspath(os.curdir)

# run tests

# register signal handler for when user issues SIGINT (i.e. hits Ctrl-C)

current_test_process = None

def sighandler(signum, frame):
	if signum == signal.SIGINT:
		if current_test_process:
			print("Runtests Received SIGINT! Killing current test " + testname)
			current_test_process.kill()
			current_test_process.wait()
			raise Exception("Exiting due to SIGINT.")
		
	
signal.signal(signal.SIGINT, sighandler)

a_test_failed = False

test_entry = 0

os.makedirs(output_dir,0o755)
summary_file_path = output_dir + "/Summary-%s"%time.strftime("%Y-%m-%d-%H-%M-%S",time.gmtime())
summary_file = open(summary_file_path,'w')

if "TEST_ARCHIVE_DIR" in os.environ:
	# make archive directory root
	archive_dir =  os.environ["TEST_ARCHIVE_DIR"] + "/" + os.path.split(output_dir)[1]
	os.makedirs(archive_dir,0o755)

for test in test_list:
	(testdesc, script, envlist,envValList) = test
	testname = os.path.splitext(os.path.basename(script))[0]
	os.environ["TSTDIR"] = os.path.split(script)[0]
	dirname = "%s/%s.results_%d" % (output_dir,testname,test_entry)
		
	os.environ["TSTOUTDIR"] = dirname
	if options.quiet == None:
		print("Making directory " + dirname)
	os.makedirs(dirname)
	# make fake home directory
	#os.makedirs(dirname + "/FAKE_HOME")
	# set home directory to location
	#os.environ["HOME"]  = dirname + "/FAKE_HOME"
	# change to output directory and run
	os.chdir(dirname)
	if options.quiet == None:
		print("******************\n" + testdesc + "\n******************")
		print("Test standard output in: " + dirname + "/" + testname + ".stdout") 
		print("Test standard error in: " + dirname + "/" + testname + ".stderr") 

	summary_file.write("******************\n" + testdesc + "\n******************\n")
	summary_file.write("Script: %s\n"%script)	
	summary_file.write("Test standard output in: " + dirname + "/" + testname + ".stdout\n")	
	summary_file.write("Test standard error in: " + dirname + "/" + testname + ".stderr\n")
	
	open("test_desc.txt",'w').write("Test Description: %s\n"%testdesc)
	# Check for Python files
	(base,ext) = os.path.splitext(script)
	
	test_args = ()
	
	if ext == ".py":
		test_args = (script,)
		if options.python_debug == None:
			# run normal python
			script = os.environ["HOST_PYTHON"]
		else:
			# run idle
			script = os.environ["HOST_PYTHON_IDLE"]
	
	start_time = time.strftime("%Y-%m-%d-%H-%M-%S",time.gmtime())
	summary_file.write("Test start time: %s\n"%start_time)
	current_test_process = scripts.helpers.process_helper.BackgroundProcess()
	# read in environment file, if specified
	new_env = {}
	if envlist != None:
		summary_file.write("ENV files: %s\n"%" ".join(envlist))
		for envfile in envlist:
			if os.path.isfile(envfile) == False:
				print("Suite ENV config file %s for script %s not found."%(envfile,base + ext))
				sys.exit(-1)
			# read env file into environment
			scripts.helpers.parsers.env_file_parser.EnvFileReader().read_file(envfile,new_env)
	if envValList != None:
		for (var,value) in envValList:
			new_env[var] = value
	
	summary_file.flush()	
	current_test_process.start(script,test_args,testname + ".stdout",testname + ".stderr",new_env)
	status = current_test_process.wait()
	end_time = time.strftime("%Y-%m-%d-%H-%M-%S",time.gmtime())
	if status == None:
		outcome = "killed"
		print("[%d] %-60.60s%30.30s"%(test_entry,testdesc,"KILLED"))
		summary_file.write("%-60.60s%30.30s\n"%(testdesc,"KILLED"))
		a_test_failed = True;
		break
	elif status == 0:
		outcome = "passed"
		print("[%d] %-60.60s%30.30s"%(test_entry,testdesc,bcolors.GREEN + "PASSED" + bcolors.ENDC))
		summary_file.write("%-60.60s%30.30s\n"%(testdesc,"PASSED"))
	else:
		outcome = "failed"
		# search for description of failure
		if os.path.isfile("status.txt"):
			status_text  = open("status.txt",'r').readline().strip()
		else:
			status_text = bcolors.RED + "FAILED" + bcolors.ENDC
			
		if options.quiet == None:
#			print testname + " **FAILED**. Status = " + str(status)
			print("%-60.60s%30.30s"%(testdesc,status_text))
		else: 
			print("[%d] %-60.60s%30.30s"%(test_entry,testdesc,status_text))
		summary_file.write("%-60.60s%30.30s\n"%(testdesc,status_text))
		
		a_test_failed = True;
	if options.quiet == None:
		print("******************\n\n")

	# Write stats file if accessible
	if os.path.isfile(os.environ["HOST_STAT_FILE"]):
		# append to current file
		try:
			stat_file = open(os.environ["HOST_STAT_FILE"],'a')
			stat_file.write(testname + "\t" + outcome + "\t" + start_time + "\t" + end_time + "\t" + os.environ['USER'] + "\t" + socket.gethostname() + "\t\"" + args + "\"\n")
			stat_file.close()
		except:
			print("Unable to write stats file " + os.environ["HOST_STAT_FILE"])
			
	if "TEST_ARCHIVE_DIR" in os.environ:
		os.chdir(output_dir)
		# copy test data to archive directory
		cmd = "rsync -aHx %s %s"%(dirname,archive_dir)
		# print cmd
		(status,output) = scripts.helpers.process_helper.getstatusoutput(cmd)
		if status != 0:
			print("Unable to archive test directory! %s"%output)
		else:
			summary_file.write("Archived to %s\n"%archive_dir)
			
		
		if "TEST_ARCHIVE_REMOVE_SOURCE" in os.environ:
			# Only remove source if arhcive worked
			if os.environ["TEST_ARCHIVE_REMOVE_SOURCE"] and status == 0:
				rm_cmd = "rm -rf %s"%dirname
				(status,output) = scripts.helpers.process_helper.getstatusoutput(rm_cmd)
				if status != 0:
					print("Error deleting source directory %s: %s"%(dirname,output))
		
	summary_file.write("******************\n\n")
	summary_file.flush()	
			
	test_entry += 1

summary_file.close()

if "TEST_ARCHIVE_DIR" in os.environ:
	# copy summary file
	cmd = "cp %s %s"%(summary_file_path,archive_dir)
	(status,output) = scripts.helpers.process_helper.getstatusoutput(cmd)
	if status != 0:
		print("Error copying summary file to %s: %s"%(dirname,output))

current_test_process = None

os.chdir(current_dir)

if a_test_failed:
	sys.exit(1)


