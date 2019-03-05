'''
Created on Feb 23, 2015

@author: tcanham
'''

import sys
import optparse

verbose = False

parser = optparse.OptionParser()

parser.add_option("-d", dest="description", help="Description of output")
parser.add_option("-m", dest="module", help="Module")
parser.add_option("-o", dest="output", help="Output format: csv or summary")

# stash arguments
	
(options,args) = parser.parse_args(sys.argv)

sloc = 0
ncsl = 0
comments = 0
semis = 0


if options.output == "summary":
	for file in args[1:]:
		if verbose:
			print("Processing %s"%file)
		for line in open(file).readlines():
			# search for total line
			if line.count("total 	[ncsl:sloc"):
				line_tuple = line.split()
				sloc_loc = int(line_tuple[0])
				ncsl_loc = int(line_tuple[1])
				comments_loc = int(line_tuple[2])
				semis_loc = int(line_tuple[3])
				if verbose:
					print("sloc: %15d ncsl: %15d comments: %15d semis: %15d"%(sloc_loc,ncsl_loc,comments_loc,semis_loc))
				sloc += sloc_loc
				ncsl += ncsl_loc
				comments += comments_loc
				semis += semis_loc

	print("%s SLOC: %10d NCSL: %10d COMMENTS: %10d SEMIS: %10d"%(options.description,sloc,ncsl,comments,semis))

if options.output == "csv_header":
	print("%-40s%10s,%10s,%10s,%10s"%("Module" + ",","HAND","AC","XML","TOTAL"))
	sys.exit(0)

if options.output == "csv":

	sloc_list = list()
	for file in args[1:]:
		for line in open(file).readlines():
			# search for total line
			if line.count("total 	[ncsl:sloc"):
				line_tuple = line.split()
				ncsl_loc = int(line_tuple[1])
				if verbose:
					print("file: %s ncsl: %15d"%(file,ncsl_loc))
				sloc += ncsl_loc
				sloc_list.append(ncsl_loc)
	
	print("%-40s%10d,%10d,%10d,%10d"%(options.description + ",",sloc_list[0],sloc_list[1],sloc_list[2],sloc))
	