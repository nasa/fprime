import os
import re

class FileSearcher:
	def loadFile(self, file_name):
		self.file_name = file_name
		self.file_line_list = open(file_name,'r').readlines()
	# find a particular string in the file
	def find(self, file_name, text):
		self.loadFile(file_name)
		occurrences = 0
		for line in self.file_line_list:
			occurrences += line.count(text)
		return occurrences

	# find a value of the form x = y with white space delineating
	# finds first then quits
	def get_value(self,file_name,key):
		self.loadFile(file_name)
		regex_var_pattern = re.compile("\s*" + key + "\s*=\s*(.*?)(\W|$)")
		
		for line in self.file_line_list:
			# I hate regular expressions...
			matchObj = regex_var_pattern.match(line)
			if matchObj:
				return matchObj.group(1)
		return None
			
	