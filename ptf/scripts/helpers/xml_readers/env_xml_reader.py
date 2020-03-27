# This reader takes each tag and appends a suffix for each property in the tag, and places it in a dictionary.
# A subsequent call puts those values into the environment for use by, among other things, helpers. 

import xml.sax
import os
import sys

class EnvXMLHandler(xml.sax.ContentHandler):
	def __init__(self):
		self.cfg_dictionary = {}

	def startElement(self, tag, attrs):
		
		#print "TAG: %s" % (tag)
		#print attrs.keys()

		for param in list(attrs.keys()):
			val = attrs[param]

			# look for embedded environment variables
			while 1:
				var_index = val.find("$(")
				if var_index == -1:
					break
				else:
					var_end_index = val.find(")")
					if var_end_index == -1:
						raise str("Mismatched environment variable parens")
					else:
						# slice out variable
						val_pre = val[0:var_index]
						val_post = val[var_end_index+1:]
						var = val[var_index+2:var_end_index]
						# check first for variable already read in
						# print "Looking for " +  var
						if var in self.cfg_dictionary:
							sub_val = self.cfg_dictionary[var]
						else:
							try:
								sub_val = os.environ[var]
							except:
								for key in list(self.cfg_dictionary.keys()):
									print("%s = %s" % (key,self.cfg_dictionary[key]))
								raise str("%s not found in environment or in xml file" % var)
							
						val = val_pre + sub_val + val_post
			
			
			#print "Submitting %s as %s" % (tag + "_" + param,val)
			self.cfg_dictionary[tag + "_" + param] =  val
			
	def toEnvs(self):

		for env_var in list(self.cfg_dictionary.keys()):
			# the dictionary key is the variable
			val = self.cfg_dictionary[env_var]
			# print "Setting " + env_var + " to " + val
			os.environ[env_var] = val
			
	def getDict(self):
			return self.cfg_dictionary
	def clear(self):
			self.cfg_dictionary = {}
		
		 