#!/bin/env python
#===============================================================================
# NAME: CommandArgsFactory
#
# DESCRIPTION: This is a factory class maps the command Arguments
#              into the proper UI widget set on a single frame and
#              returns that frame for packing so it will appear.
#              This class is instanced for each UI panel
#              can utilize the same create method.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Jan 14, 2015
#
# Copyright 2015, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
#===============================================================================
#
# Python standard modules
#

import os
import sys
import time
import logging
import exceptions
import Tkinter
import Pmw

from controllers import command_loader
from controllers import commander

# Import the types this way so they do not need prefixing for execution.
from models.serialize.type_exceptions import *
from models.serialize.type_base import *

from models.serialize.bool_type import *
from models.serialize.enum_type import *
from models.serialize.f32_type import *
from models.serialize.f64_type import *

from models.serialize.u8_type import *
from models.serialize.u16_type import *
from models.serialize.u32_type import *
from models.serialize.u64_type import *

from models.serialize.i8_type import *
from models.serialize.i16_type import *
from models.serialize.i32_type import *
from models.serialize.i64_type import *

from models.serialize.string_type import *
from models.serialize.serializable_type import *

from utils import gse_misc

# Global logger init. below.
PRINT = logging.getLogger('output')
DEBUG = logging.getLogger('debug')

class CommandArgsFactory:
    """
    This is a concrete factory method object used to create the
    main UI panel instances for sets of arguments contained
    for a specific command mnemonic.
    """
    __parsed   = None
    __instance = None

    def __init__(self):
        """
        Private Constructor (singleton pattern)
        """
        self.__parsed   = None
        self.__instance = None

        # Setup command dictionary of objects here.
        self.__cmds = command_loader.CommandLoader().getInstance()
        self.__cmds_dict = self.__cmds.getCommandDict()
        # Scrolled frame widget initialize (never actually used)
        self.__sf = Tkinter.Frame() # TKC - overwritten in create(), doesn't allow text running of command scripts


    def create(self, mnemonic, parent):
        """
        Create the command args panel here.
        """
        # Always create fresh top level scrolled frame
        self.__sf = Pmw.ScrolledFrame(parent, usehullsize=1, \
                               hull_width=400, hull_height=60)
        self.__sf.pack(fill="both", expand=1)
        frame = self.__sf.interior()
        entry_fields = []
        # Entry fields dict holds arg_name as key and EntryWidget instance as value.
        entry_fields_dict = dict()
        #
        #self.__cmds_dict = self.__cmds.getCommandDict()
        args = self.__cmds.getCommandObj(mnemonic).getArgs()
        #
        # Iterate over args to create entries
        #for arg in self.__cmds_dict[mnemonic].getArgs():
        for arg in args:
            arg_name, arg_desc, arg_type = arg

            # print arg_name, repr(arg_type), arg_type.val
            #
            #
            if type(arg_type) == type(BoolType()):
                def choose1(entry):
                    pass
                    #print entry
                l = ['True','False']
                e = Pmw.ComboBox(frame, listbox_width=24, dropdown=1,
                                selectioncommand=choose1,
                                scrolledlist_items=l,label_text = arg_name, labelpos='nw')
                if arg_type.val == True:
                    s = "True"
                else:
                    s = "False"
                e.selectitem(s)
            elif type(arg_type) == type(EnumType()):
                def choose2(entry):
                    pass
                    #print entry
                l = arg_type.keys()
                e = Pmw.ComboBox(frame, listbox_width=24, dropdown=1,
                                 selectioncommand=choose2,
                                 scrolledlist_items=l,label_text = arg_name, labelpos='nw')
                e.selectitem(arg_type.val)
            elif type(arg_type) == type(F64Type()):
                e = Pmw.EntryField(frame, validate={'validator': 'real'},label_text = arg_name, labelpos='nw',value = "0.0")
            elif type(arg_type) == type(F32Type()):
                e = Pmw.EntryField(frame, validate={'validator': 'real'},label_text = arg_name, labelpos='nw',value = "0.0")
            elif type(arg_type) == type(I64Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : -9223372036854775808L,
                                                    'max' : 9223372036854775807L},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(I32Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : -2147483648,
                                                    'max' : 2147483647},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(I16Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : -32768,
                                                    'max' : 32767},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(I8Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : -128,
                                                    'max' : 127},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(U64Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : 0,
                                                    'max' : 18446744073709551615L},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(U32Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : 0,
                                                    'max' : 4294967295},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(U16Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : 0,
                                                    'max' : 65535},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(U8Type()):
                e = Pmw.EntryField(frame, validate={'validator': gse_misc.hex_integer_validate, 'stringtovalue':gse_misc.hex_integer_stringtovalue,
                                                    'min' : 0,
                                                    'max' : 255},label_text = arg_name, labelpos='nw',value = "0")
            elif type(arg_type) == type(StringType()):
                # @todo: The alphanumeric validator seems to dissable input
                e = Pmw.EntryField(frame, validate=None)
            elif type(arg_type) == type(SerializableType()):
                pass

            if arg_type.val != None:
                e.setvalue( str(arg_type.val) )

            entry_fields.append( e )
            entry_fields_dict[arg_name] = e

        # Pack here...
        for e in entry_fields:
            # If ComboBox, rebind the key events to prevent exception
            if isinstance(e, Pmw.ComboBox):
                gse_misc.rebind_comboBox(e)

            e.pack(side=Tkinter.LEFT)
        #
        return entry_fields_dict


    def destroy(self):
        """
        Destroy the scrolling frame here for reset.
        """
        self.__sf.destroy()
        self.__sf = Tkinter.Frame()


    def create_arg_type(self, arg_name, arg_type, arg_value):
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
