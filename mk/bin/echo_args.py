import sys

arg_num = 0

for arg in sys.argv:
	print("arg %d: %s\n"%(arg_num,arg))
	arg_num += 1
