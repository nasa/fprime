'''
@brief Command Data class

Instances of this class define a specific instance of a command with specific
argument values.

@data Created July 3, 2018
@author Josef Biberstein

@bug No known bugs
'''
from __future__ import print_function

from enum import Enum

from fprime_gds.common.data_types import sys_data

from fprime.common.models.serialize.time_type import *

from fprime.common.models.serialize.bool_type import *
from fprime.common.models.serialize.enum_type import *
from fprime.common.models.serialize.f32_type import *
from fprime.common.models.serialize.f64_type import *

from fprime.common.models.serialize.u8_type import *
from fprime.common.models.serialize.u16_type import *
from fprime.common.models.serialize.u32_type import *
from fprime.common.models.serialize.u64_type import *

from fprime.common.models.serialize.i8_type import *
from fprime.common.models.serialize.i16_type import *
from fprime.common.models.serialize.i32_type import *
from fprime.common.models.serialize.i64_type import *

from fprime.common.models.serialize.string_type import *
from fprime.common.models.serialize.serializable_type import *

from copy import deepcopy


class CmdData(sys_data.SysData):
    '''The CmdData class stores a specific command'''

    def __init__(self, cmd_args, cmd_temp, cmd_time=None):
        '''
        Constructor.

        Args:
            cmd_args: The arguments for the event. Should match the types of the
                      arguments in the cmd_temp object. Should be a tuple.
            cmd_temp: Command Template instance for this command (this provides
                      the opcode and argument types are stored)
            cmd_time: The time the event should occur. This is for sequences.
                      Should be a TimeType object with time base=TB_DONT_CARE

        Returns:
            An initialized CmdData object
        '''
        self.id = cmd_temp.get_id()
        self.template = cmd_temp
        self.arg_vals = cmd_args

        self.args = [deepcopy(typ) for (_, _, typ) in self.template.arguments]
        self.arg_names = [name for (name, _, _) in self.template.arguments]

        if (cmd_time):
            self.time = cmd_time
        else:
            self.time = TimeType(TimeBase["TB_DONT_CARE"].value)

        for val, typ in zip(self.arg_vals, self.args):
            self.convert_arg_value(val, typ)

    def get_template(self):
        """Get the template class associate with this specific data object

        Returns:
            Template -- The template class for this data object
        """

        return self.template

    def get_id(self):
        """Get the ID associate with the template of this data object

        Returns:
            An ID number
        """
        
        return self.id

    def get_arg_vals(self):
        """ Get the values for each argument in a command.

        Returns:
            list -- a list of value objects that were used in this data object.
        """
        
        return self.arg_vals

    def get_args(self):
        """Get the arguments associate with the template of this data object

        Returns:
            list -- A list of type objects representing the arguments of the template of this data object (in order)
        """

        return self.args


    def get_str(self, time_zone=None, verbose=False, csv=False):
        '''
        Convert the command data to a string

        Args:
            time_zone: (tzinfo, default=None) Timezone to print time in. If
                      time_zone=None, use local time.
            verbose: (boolean, default=False) Prints extra fields if True
            csv: (boolean, default=False) Prints each field with commas between
                                          if true

        Returns:
            String version of the command data
        '''
        time_str = self.time.to_readable(time_zone)
        raw_time_str = str(self.time)
        name = self.template.get_full_name()


        if self.args == None:
            arg_str = "EMPTY COMMAND OBJ"
        else:
            # The arguments are currently serializable objects which cannot be
            # used to fill in a format string. Convert them to values that can be
            arg_val_list = [arg_obj.val for arg_obj in self.args]

            arg_str = " ".join(arg_val_list)

        if verbose and csv:
            return ("%s,%s,%s,%d,%s"%(time_str, raw_time_str, name, self.id, arg_str))
        elif verbose and not csv:
            return ("%s: %s (%d) %s : %s"%(time_str, name, self.id,
                                              raw_time_str, arg_str))
        elif not verbose and csv:
            return ("%s,%s,%s"%(time_str, name, arg_str))
        else:
            return ("%s: %s : %s"%(time_str, name, arg_str))

    def convert_arg_value(self, arg_val, arg_type):
        if "0x" in arg_val:
            arg_val = int(arg_val, 16)

        if type(arg_type) == type(BoolType()):
            if arg_val == "False":
                av = False
            else:
                av = True
            arg_type.val = av
        elif type(arg_type) == type(EnumType()):
            arg_type.val = arg_val
        elif type(arg_type) == type(F64Type()):
            arg_type.val = float(arg_val)

        elif type(arg_type) == type(F32Type()):
            arg_type.val = float(arg_val)

        elif type(arg_type) == type(I64Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(I32Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(I16Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(I8Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(U64Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(U32Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(U16Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(U8Type()):
            arg_type.val = int(arg_val)

        elif type(arg_type) == type(StringType()):
            arg_type.val = arg_val

        elif type(arg_type) == type(SerializableType()):
            pass
        else:
            raise Exception('Argument value could not be converted to type object')


    def __str__(self):
        arg_str = ''
        for name, typ in zip(self.arg_names, self.args):
            arg_str += ('%s : %s |')%(name, str(typ.val))
        arg_str = 'w/ args | ' + arg_str


        arg_info = '%s '% self.template.mnemonic

        if len(self.args) > 0:
            return arg_info + arg_str
        else:
            return arg_info


