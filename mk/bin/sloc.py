import sys
import subprocess
import os
import os.path

# Make sure that BUILD_ROOT environment variable is set

file_stats = {}

def c_sloc_lines(file):
	
	LINE_COUNTER = os.environ["LINE_COUNT"]
	cmdText = "%s %s 2>&1" %(LINE_COUNTER,file)
	numRealLines = 0;
	sloc = 0
	ncsl = 0
	comments = 0
	semis = 0
	
	try:
		realLineData = os.popen(cmdText).readlines() 
		sloc = int(realLineData[1].split()[0])
		ncsl = int(realLineData[1].split()[1])
		comments = int(realLineData[1].split()[2])
		semis = int(realLineData[1].split()[3])
	except:
		sys.stderr.write("command \"%s\" exception processing file %s\n" % (cmd,file))
		sys.exit(-1)
		
	return (sloc,ncsl,comments,semis)

def count_non_empty_lines(file_lines,comment):
	non_empty_count = 0
	comments = 0
	raw_count = 0
	
	for line in file_lines:
		if line.count(comment):
			comments += 1
		line = line.split(comment)[0].strip()
		if line != "":
			non_empty_count += 1
		raw_count += 1
			
	return (raw_count,non_empty_count,comments,0)
	
# entries are module, file, stats
	
def get_stats(file):

	file_short = file.replace(os.environ["BUILD_ROOT"]+"/","")

	if not os.path.isfile(file):
		sys.stderr.write("File %s not found!\n" % file_short)
		file_stats[file_short] = (0,0,0,0)
		return
	
	file_lines = open(file,"r").readlines()

	ext = os.path.splitext(file)[1]
	
	if ext == ".c" or ext == ".cc" or ext == ".cpp" or ext == ".h" or ext == ".hpp":
		file_stats[file_short] = c_sloc_lines(file)
	elif ext == ".xml":
		file_stats[file_short] = count_non_empty_lines(file_lines,"<?")
	elif ext == ".hsm":
		file_stats[file_short] = count_non_empty_lines(file_lines,"#")
	elif ext == ".txt":
		file_stats[file_short] = count_non_empty_lines(file_lines,"#")
	else:
		sys.stderr.write("Unrecognized extension %s on %s\n"%(ext,file_short))
		return
	
	if sloc_verbose:
		sloc_log.write("File %s: sloc: %d ncsl: %d comments: %d semis: %d\n"%(file_stats[file_short][0],file_stats[file_short][1],file_stats[file_short][2],file_stats[file_short][3]))
 
if "BUILD_ROOT" not in os.environ:
	print("You must define BUILD_ROOT first.")
	sys.exit(-1)

if "SLOC_VERBOSE" in os.environ:
	sloc_verbose = True
	sloc_log = open("sloc.log_%s"%sys.argv[1],'w')
else:
	sloc_verbose = False
		
if sys.argv[1] == "summarize":
	file_lines = open(sys.argv[2],"r").readlines()
	type_stats = {}
	for line in file_lines:
		(type,file_short,sloc_lines,ncsl_lines,comments,semis) = line.split(",")
		if type not in type_stats:
			type_stats[type] = [0,0,0,0]
			
		type_stats[type][0] += int(sloc_lines)
		type_stats[type][1] += int(ncsl_lines)
		type_stats[type][2] += int(comments)
		type_stats[type][3] += int(semis)
		
	print("*sum*")
	for type in list(type_stats.keys()):
		print(("%s,%d,%d,%d,%d"%(type,type_stats[type][0],type_stats[type][1],type_stats[type][2],type_stats[type][3])))
	sys.exit(0)
	
if sys.argv[1] == "collate":
	file_listing = open("file_sloc.txt","w")
	sloc_sum_file = open("sloc_sum.txt","w")
	type_stats = {}
	for file in sys.argv[2:]:
		curr_file_lines = open(file,"r").readlines()
		sum_found = False
		for line in curr_file_lines:
			if line.count("*sum*"):
				sum_found = True
				continue
			if sum_found: 
				(type,sloc_lines,ncsl_lines,comments,semis) = line.split(",")
				if type not in type_stats:
					type_stats[type] = [0,0,0,0]
					
				type_stats[type][0] += int(sloc_lines)
				type_stats[type][1] += int(ncsl_lines)
				type_stats[type][2] += int(comments)
				type_stats[type][3] += int(semis)
			else:
				file_listing.write(",".join(line.split(",")[1:]))
	
	if not sum_found:
		sys.stderr.write("File %s doesn't have *sum*!\n" % file_short)
	else:
		total_sloc_lines = total_ncsl_lines = total_comment_lines = total_semi_lines = 0
		for type in list(type_stats.keys()):
			sloc_sum_file.write("Type: %s sloc: %d nscl: %d comments: %d semis %d\n"%(type,type_stats[type][0],type_stats[type][1],type_stats[type][2],type_stats[type][3]))
			total_sloc_lines += type_stats[type][0]		
			total_ncsl_lines += type_stats[type][1]		
			total_comment_lines += type_stats[type][2]
			total_semi_lines += type_stats[type][3]
			
		sloc_sum_file.write("Totals: sloc: %d ncsl: %d comments: %d semis: %d\n"%(total_sloc_lines,total_ncsl_lines,total_comment_lines,total_semi_lines))		

	sys.exit(0)

for file in sys.argv[2:]:
	file_short = file.replace(os.environ["BUILD_ROOT"]+"/","")
	file_stats[file] = []
	if sloc_verbose:
		sloc_log.write("Examining file %s\n"%file)
	get_stats(file)
	(sloc_lines,ncsl_lines,comments,semis) = file_stats[file_short]
	print(("%s,%s,%s,%s,%s,%s"%(sys.argv[1],file_short,sloc_lines,ncsl_lines,comments,semis)))


