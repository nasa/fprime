'''
Created on Feb. 9, 2015

@author: reder
'''
from __future__ import print_function

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

import struct
from enum import Enum
import traceback

Severity = Enum('Severity', 'COMMAND ACTIVITY_LO ACTIVITY_HI WARNING_LO WARNING_HI DIAGNOSTIC FATAL')

class Event(object):
    '''
    Event class is for deserializing log event messages.
    THis is essentially the equivalent of EVR's in MSL, SMAP, etc.
    '''
    def __init__(self, name, id, severity, format_string, event_description, arguments):
        '''
        Constructor
        '''
        
        ## Make sure correct types are passed
        
        if not type(name) == type(str()):
            raise TypeMismatchException(type(str()), type(name))
        
        if not type(id) == type(int()):
            raise TypeMismatchException(type(int()),type(id))
        
        if not type(format_string) == type(str()):
            raise TypeMismatchException(type(str()), type(format_string))
        
        if not type(event_description) == type(str()):
            raise TypeMismatchException(type(str()), type(event_description))
        
        if not type(arguments) == type(list()):
            raise TypeMismatchException(type(list()),type(arguments))

        for (argname,argdesc,argtype) in arguments:
            #
            if not type(argname) == type(str()):
                raise TypeMismatchException(type(str()),type(argname))
            #
            if not type(argdesc) == type(str()):
                raise TypeMismatchException(type(str()),type(argdesc))
            #
            if not issubclass(type(argtype), type(BaseType())):
                raise TypeMismatchException(type(BaseType()),type(argtype))

        # Initialize event internal variables
        self.__name     = name
        self.__id       = id
        self.__severity = severity
        self.__format_string = format_string
        self.__event_description = event_description
        self.__arguments = arguments


    def deserialize(self, ser_data):
        '''
        Deserialize event arguments
        @param ser_data: Binary input data of id followed by args
        '''
        vals = []
        #
        # Next parse the arguments.
        # 
        args = list(self.__arguments)
        offset = 0
        for arg in args:
            arg_obj = arg[2]
            try:
                #print arg_obj
                arg_obj.deserialize(ser_data, offset)
                vals.append(arg_obj.val)
            except TypeException as e:
                print("Event deserialize exception %s"%(e.getMsg()))
                traceback.print_exc()  
                vals.append("ERR")
            offset = offset + arg_obj.getSize()
        
        vals = [0] + vals
        return vals


    def stringify(self, event_args_list):
        """
        Return a formated string of event args.
        """
        #print event_args_list
        return self.__format_string % tuple(event_args_list[1:])

    def getName(self):
        return self.__name

    def getId(self):
        return self.__id

    def getSeverity(self):
        return self.__severity
    
    def getFormatString(self):
        return self.__format_string
    
    def getEventDescription(self):
        self.__event_description
    
    def getArgs(self):
        return self.__arguments


if __name__ == '__main__':
    pass
