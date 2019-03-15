import subprocess
import os
import sys

class EnvFileReader:
	def read_file(self, filename, env_var = os.environ):
		file_lines = open(filename,'r').readlines()
		line_num = 1
		for line in file_lines:
			# get rid of comments
			line = line.split("#")[0]
			# strip whitespace from ends
			line = line.strip()
			# check if empty line
			if line == "":
				line_num += 1
				continue
			
			# check for =
			if line.find("=") == -1:
				raise Exception("Missing '=' on line %i of file %s" % (line_num,filename))
			# split into var = val pairs
			(var,val) = line.split("=",1)
			# remove whitespace from vars and values
			var = var.strip()
			val = val.strip()
			# search for variables in val
			done = False
			while True:
				var_start_index = val.find("$(")
				if var_start_index == -1:
					break
				var_end_index = val.find(")")
				if var_end_index == -1:
					raise Exception("Variable parse error on line %i of file %s" % (line_num,filename))
				# extract variable value
				sub_var = val[var_start_index+2:var_end_index]
				# look for variable in environment, if there rebuild val
				if sub_var in os.environ:
					val = val[0:var_start_index] + os.environ[sub_var] + val[var_end_index+1:]
				elif sub_var in env_var:
					val = val[0:var_start_index] + env_var[sub_var] + val[var_end_index+1:]
				else:
					raise Exception("Variable %s not found in environment" % sub_var)
					
				
#			print "%s = %s" % (var, val)
			env_var[var] = val
			line_num += 1
			
		
if __name__ == '__main__' :
	
	reader = EnvFileReader()
	reader.read_file(sys.argv[1])
#	for env in os.environ:
#		print "%s = %s" % (env,os.environ[env])
		
		
