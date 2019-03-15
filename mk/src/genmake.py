import sys
import os
import parsers.mod_mk_parser
import parsers.variable_list_parser

# arguments:
# 1 = name of file to generate

#os.environ["PARSER_VERBOSE"] = "true"
# Make sure that BUILD_ROOT environment variable is set

if "BUILD_ROOT" not in os.environ:
	print("You must define BUILD_ROOT first.")
	sys.exit(-1)

makefiledir = sys.argv[1]
	
print("Generating Makefiles in %s"%	makefiledir)


try:
	mod_file_parser = parsers.variable_list_parser.VariableListParser(os.environ["BUILD_ROOT"] + "/mk/configs/modules/modules.mk",":=")
except parsers.variable_list_parser.VariableParseError as ex:
	# delete the partial or old Makefile
	print(("**Error: " + ex.getErr() + "**"))
	sys.exit(-1)
	
deployment_list = mod_file_parser.getValList("DEPLOYMENTS")
	
all_module_list = []

for deployment in deployment_list:
	module_list =  mod_file_parser.getValList("%s_MODULES"%deployment)
	# search for modules in both
	for module in module_list:
		if module not in all_module_list:
			all_module_list += module,
	# write deployment targets

other_module_list = mod_file_parser.getValList("OTHER_MODULES")
		
# instantiate parsers for modules

parser_list = []

# change to root of modules
os.chdir(os.environ["BUILD_ROOT"])

all_module_list += other_module_list

for module in all_module_list:
	
	# for module name, remove slashes
	module_name = module.replace("/","").replace("\\","")
	
	if "PARSER_VERBOSE" in os.environ:
		print(("Processing module \"%s\" dir %s"%(module_name,module)))

	try:
		parser_list.append(parsers.mod_mk_parser.ModMkParser(module_name,module,True))
	except parsers.mod_mk_parser.CfgParseError as ex:
		print(("**Error: " + ex.getErr() + "**"))
		for file in ["/ac_targets.mk","/bin_targets.mk","/vars.mk"]:
			del_file = sys.argv[1] + file
			if os.path.exists(del_file):
				#print "Deleting %s" % del_file
				os.unlink(del_file)
		sys.exit(-1)


# generate variable makefile
	
file_descriptor = open("%s/vars.mk"%makefiledir,'w')

# write front of makefile to file

if "PARSER_VERBOSE" in os.environ:
	print ("Opening Makefile")

file_descriptor.write("# *** Genearated Makefile ***\n# Modifications will be overwritten.\n\n")
#file_descriptor.write("include $(BUILD_ROOT)/mk/makefiles/front.mk\n")

# write variables
if "PARSER_VERBOSE" in os.environ:
	print("Writing Makefile variables")

for parser in parser_list:
	try:
		parser.generateVariables(file_descriptor)
	except parsers.mod_mk_parser.CfgParseError as message:
		print(("**Error: " + message.getErr() + "**"))
		sys.exit(-1)
		
file_descriptor.close()

# generate ac and binary targets

ac_targets_file_descriptor = open("%s/ac_targets.mk"%makefiledir,'w')
bin_targets_file_descriptor = open("%s/bin_targets.mk"%makefiledir,'w')

#  write overall section
#file_descriptor.write("include $(BUILD_ROOT)/mk/makefiles/deployments.mk\n")
#file_descriptor.write("include $(BUILD_ROOT)/mk/makefiles/middle.mk\n")

# write targets

if "PARSER_VERBOSE" in os.environ:
	print("Writing Makefile targets")
	
for parser in parser_list:
	try:
		parser.generateTargets(ac_targets_file_descriptor,bin_targets_file_descriptor)
	except parsers.mod_mk_parser.CfgParseError as message:
		print(("**Error: " + message.getErr() + "**"))
		sys.exit(-1)

ac_targets_file_descriptor.close()
bin_targets_file_descriptor.close()

print("Makefile generation complete.")
