import os.path



class VariableParseError:
	def __init__(self, error):
		self.error = error
	def getErr(self):
		return self.error

class VariableListParser:
	def __init__(self, file, equal_token = "="):
		self.variable_dictionary = {}
		
		# read in file
		try:
			file_lines = open(file).readlines()
		except:
			raise VariableParseError("Error opening %s"%file)
		# for each line, search for variables.
		
		line_number = 1
		
		line = ""
		
		for curr_line in file_lines:
			# chop off comments (# character to end of page)
			# print("Processing line: " + line)
			comment_loc = curr_line.find("#")
			if comment_loc != -1:
				curr_line = curr_line[0:comment_loc]
			# strip off remaining whitespace
			curr_line = curr_line.strip()
			# check to see if empty line
			#print("Line length: " + str(len(line)))
			if len(curr_line) == 0:
				#print ("skipping")
				line_number += 1
				continue
			
			# look for continuation character
			cont_loc = curr_line.find("\\")
			if cont_loc != -1:
				curr_line = curr_line[0:cont_loc]
				curr_line = curr_line.strip()
				line += curr_line + " " # the space is for white space between file names
				line_number += 1
				continue
			else:
				line += curr_line + " "
			
			# look for $(VAR) entries
			while True:
				var_loc_start = line.find("$(")
				if var_loc_start == -1:
					break
				# search for closing
				var_loc_end = line.find(")")
				if var_loc_end == -1:
					break
				subst_var = line[var_loc_start+2:var_loc_end]
				if subst_var not in self.variable_dictionary:
					break
				line = line[0:var_loc_start] + " " + self.variable_dictionary[subst_var] + " " + line[var_loc_end+1:]
					
			
			# print("Processed line: %s" % line)
			# split at first equal sign	
			# make sure remaining text has "=" in it somewhere
			# by searching for "="
			
			if line.count(equal_token) < 1:
				raise VariableParseError("Invalid entry found in %s line %i: \"%s\"" % (file,line_number,line))
			(var,value) = line.split(equal_token,1)
			var = var.strip()
			value = value.strip()
			
			self.variable_dictionary[var] = value
			
			line = ""
			line_number += 1
	
	def getVal(self,var):
			
			return self.variable_dictionary[var]
		
	def getValList(self,var):
		
			return self.variable_dictionary[var].split()
