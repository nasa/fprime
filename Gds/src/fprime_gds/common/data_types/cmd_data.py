"""
@brief Command Data class

Instances of this class define a specific instance of a command with specific
argument values.

@data Created July 3, 2018
@author Josef Biberstein

@bug No known bugs
"""

from copy import deepcopy

from fprime.common.models.serialize.array_type import ArrayType
from fprime.common.models.serialize.bool_type import BoolType
from fprime.common.models.serialize.enum_type import EnumType
from fprime.common.models.serialize.numerical_types import (
    I8Type,
    I16Type,
    I32Type,
    I64Type,
    U8Type,
    U16Type,
    U32Type,
    U64Type,
    F32Type,
    F64Type,
)
from fprime.common.models.serialize.serializable_type import SerializableType
from fprime.common.models.serialize.string_type import StringType
from fprime.common.models.serialize.time_type import TimeBase, TimeType
from fprime_gds.common.data_types import sys_data


class CmdData(sys_data.SysData):
    """The CmdData class stores a specific command"""

    def __init__(self, cmd_args, cmd_temp, cmd_time=None):
        """
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
        """
        super().__init__()
        self.id = cmd_temp.get_id()
        self.template = cmd_temp
        self.arg_vals = cmd_args

        self.args = [deepcopy(typ) for (_, _, typ) in self.template.arguments]
        self.arg_names = [name for (name, _, _) in self.template.arguments]

        if cmd_time:
            self.time = cmd_time
        else:
            self.time = TimeType(TimeBase["TB_DONT_CARE"].value)

        errors = []
        for val, typ in zip(self.arg_vals, self.args):
            try:
                self.convert_arg_value(val, typ)
                errors.append("")
            except Exception as exc:
                errors.append(str(exc))
        # If any errors occur, then raise a aggregated error
        if [error for error in errors if error != ""]:
            raise CommandArgumentsException(errors)

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
        """Get the values for each argument in a command.

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
        """
        Convert the command data to a string

        Args:
            time_zone: (tzinfo, default=None) Timezone to print time in. If
                      time_zone=None, use local time.
            verbose: (boolean, default=False) Prints extra fields if True
            csv: (boolean, default=False) Prints each field with commas between
                                          if true

        Returns:
            String version of the command data
        """
        time_str = self.time.to_readable(time_zone)
        raw_time_str = str(self.time)
        name = self.template.get_full_name()

        if self.args is None:
            arg_str = "EMPTY COMMAND OBJ"
        else:
            # The arguments are currently serializable objects which cannot be
            # used to fill in a format string. Convert them to values that can be
            arg_val_list = [arg_obj.val for arg_obj in self.args]

            arg_str = " ".join(str(arg_val_list))

        if verbose and csv:
            return "%s,%s,%s,%d,%s" % (time_str, raw_time_str, name, self.id, arg_str)
        elif verbose and not csv:
            return "%s: %s (%d) %s : %s" % (
                time_str,
                name,
                self.id,
                raw_time_str,
                arg_str,
            )
        elif not verbose and csv:
            return "{},{},{}".format(time_str, name, arg_str)
        else:
            return "{}: {} : {}".format(time_str, name, arg_str)

    @staticmethod
    def convert_arg_value(arg_val, arg_type):
        if arg_val is None:
            raise CommandArgumentException(
                "Argument value could not be converted to type object"
            )
        if isinstance(arg_type, BoolType):
            value = str(arg_val).lower().strip()
            if value in ("true", "yes"):
                av = True
            elif value in ("false", "no"):
                av = False
            else:
                raise CommandArgumentException(
                    "Argument value is not a valid boolean"
                )
            arg_type.val = av
        elif isinstance(arg_type, EnumType):
            arg_type.val = arg_val
        elif isinstance(arg_type, (F64Type, F32Type)):
            arg_type.val = float(arg_val)
        elif isinstance(
            arg_type,
            (I64Type, U64Type, I32Type, U32Type, I16Type, U16Type, I8Type, U8Type),
        ):
            arg_type.val = int(arg_val, 0)
        elif isinstance(arg_type, StringType):
            arg_type.val = arg_val
        # Cannot handle serializable or array argument inputs
        elif isinstance(arg_type, (SerializableType, ArrayType)):
            pass
        else:
            raise CommandArgumentException(
                "Argument value could not be converted to type object"
            )

    def __str__(self):
        arg_str = ""
        for name, typ in zip(self.arg_names, self.args):
            arg_str += "%s : %s |" % (name, str(typ.val))
        arg_str = "w/ args | " + arg_str

        arg_info = "%s " % self.template.mnemonic

        if len(self.args) > 0:
            return arg_info + arg_str
        else:
            return arg_info


class CommandArgumentException(Exception):
    pass


class CommandArgumentsException(Exception):
    def __init__(self, errors):
        """
        Handle a list of errors as an exception.
        """
        super().__init__(" ".join(errors))
        self.errors = errors
