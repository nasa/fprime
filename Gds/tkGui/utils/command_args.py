#!/bin/env python
#===============================================================================
# NAME: CommandArgs
#
# DESCRIPTION: This is a utility class containing helper fucntions for
#   manipulating command arguments.
#
# AUTHOR: Brian Campuzano
# DATE CREATED  : August 7, 2015
#
# Copyright 2017, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================

# Import the types this way so they do not need prefixing for execution.
from fprime.common.models.serialize.type_exceptions import *
from fprime.common.models.serialize.type_base import *

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

def create_arg_type(arg_name, arg_type, arg_value):
    """
    Returns a new instance of a specific type object
    with value set in it.
    """
    if type(arg_type) == type(BoolType()):
        if arg_value == "False":
            arg_value = False
        else:
            arg_value = True
        new_obj = BoolType(arg_value)
    elif type(arg_type) == type(EnumType()):
        new_obj = EnumType(arg_type.typename(), arg_type.enum_dict(), arg_value)
    elif type(arg_type) == type(F64Type()):
        new_obj = F64Type(float(arg_value))
    elif type(arg_type) == type(F32Type()):
        new_obj = F32Type(float(arg_value))
    elif type(arg_type) == type(I64Type()):
        new_obj = I64Type(int(arg_value))
    elif type(arg_type) == type(I32Type()):
        new_obj = I32Type(int(arg_value))
    elif type(arg_type) == type(I16Type()):
        new_obj = I16Type(int(arg_value))
    elif type(arg_type) == type(I8Type()):
        new_obj = I8Type(int(arg_value))
    elif type(arg_type) == type(U64Type()):
        new_obj = U64Type(int(arg_value))
    elif type(arg_type) == type(U32Type()):
        new_obj = U32Type(int(arg_value))
    elif type(arg_type) == type(U16Type()):
        new_obj = U16Type(int(arg_value))
    elif type(arg_type) == type(U8Type()):
        new_obj = U8Type(int(arg_value))
    elif type(arg_type) == type(StringType()):
        new_obj = StringType(arg_value)
    elif type(arg_type) == type(SerializableType()):
        pass
    else:
        raise exceptions.TypeError
    return new_obj
