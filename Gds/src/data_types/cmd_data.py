'''
@brief Command Data class

Instances of this class define a specific instance of a command with specific argument values

@data Created July 3, 2018
@author Josef Biberstein

@bug No known bugs
'''

import sys_data
from enum import Enum


Descriptor = Enum(value='Descriptor', names='ABSOLUTE RELATIVE')
class CommandData(sys_data.SysData):

	def __init__(self, command_template, command_args, seconds=0, useconds=0, descriptor=Descriptor.RELATIVE):
		self.command_template = command_template
		self.command_args = command_args

		if not type(seconds) == type(int()):
			raise TypeMismatchException(type(int()),type(seconds))
		if not type(useconds) == type(int()):
			raise TypeMismatchException(type(int()),type(useconds))
		if not type(descriptor) == type(Descriptor.ABSOLUTE):
			raise TypeMismatchException(type(Descriptor.ABSOLUTE), type(descriptor))

		self.secs = seconds
		self.usecs = useconds
		self.desc = descriptor

	def get_args(self):
		return self.command_args

	def __str__(self):
		arg_str = ''
		for (arg, _, arg_value) in self.command_args:
			arg_str += ('	Arg %s with value %s\n')% arg, str(arg_value.val)
		
		arg_info = 'Command mneumonic %s\n'% self.command_template.mneumonic

		return arg_info + arg_str


