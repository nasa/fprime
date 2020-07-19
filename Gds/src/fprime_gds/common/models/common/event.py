"""
Created on Feb. 9, 2015

@author: reder
"""

import traceback
from enum import Enum

from fprime.common.models.serialize.type_base import BaseType

# Import the types this way so they do not need prefixing for execution.
from fprime.common.models.serialize.type_exceptions import (
    TypeException,
    TypeMismatchException,
)

Severity = Enum(
    "Severity", "COMMAND ACTIVITY_LO ACTIVITY_HI WARNING_LO WARNING_HI DIAGNOSTIC FATAL"
)


class Event:
    """
    Event class is for deserializing log event messages.
    THis is essentially the equivalent of EVR's in MSL, SMAP, etc.
    """

    def __init__(
        self, name, event_id, severity, format_string, event_description, arguments
    ):
        """
        Constructor
        """

        ## Make sure correct types are passed

        if not type(name) == type(""):
            raise TypeMismatchException(type(""), type(name))

        if not type(event_id) == type(int()):
            raise TypeMismatchException(type(int()), type(event_id))

        if not type(format_string) == type(""):
            raise TypeMismatchException(type(""), type(format_string))

        if not type(event_description) == type(""):
            raise TypeMismatchException(type(""), type(event_description))

        if not type(arguments) == type(list()):
            raise TypeMismatchException(type(list()), type(arguments))

        for (argname, argdesc, argtype) in arguments:
            #
            if not type(argname) == type(""):
                raise TypeMismatchException(type(""), type(argname))
            #
            if not type(argdesc) == type(""):
                raise TypeMismatchException(type(""), type(argdesc))
            #
            if not issubclass(type(argtype), type(BaseType())):
                raise TypeMismatchException(type(BaseType()), type(argtype))

        # Initialize event internal variables
        self.__name = name
        self.__id = event_id
        self.__severity = severity
        self.__format_string = format_string
        self.__event_description = event_description
        self.__arguments = arguments

    def deserialize(self, ser_data):
        """
        Deserialize event arguments
        @param ser_data: Binary input data of id followed by args
        """
        vals = []
        #
        # Next parse the arguments.
        #
        args = list(self.__arguments)
        offset = 0
        for arg in args:
            arg_obj = arg[2]
            try:
                # print arg_obj
                arg_obj.deserialize(ser_data, offset)
                vals.append(arg_obj.val)
            except TypeException as e:
                print("Event deserialize exception %s" % (e.getMsg()))
                traceback.print_exc()
                vals.append("ERR")
            offset = offset + arg_obj.getSize()

        vals = [0] + vals
        return vals

    def stringify(self, event_args_list):
        """
        Return a formated string of event args.
        """
        # print event_args_list
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
        return self.__event_description

    def getArgs(self):
        return self.__arguments
