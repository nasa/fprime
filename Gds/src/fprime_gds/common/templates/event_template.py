'''
@brief Event Template class

Instances of this class describe a specific event type. For example: AF_ASSERT_0
or cmdSeq_CS_CmdStarted

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
from __future__ import absolute_import

from enum import Enum

from . import data_template

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize.type_exceptions import *
from fprime_gds.common.utils.event_severity import EventSeverity


class EventTemplate(data_template.DataTemplate):
    '''Class to create event templates to describe specific event types'''

    def __init__(self, event_id, name, component, args, severity, format_str,
                 description=None):
        '''
        Constructor

        Args:
            event_id: The ID of the event being described
            name: event name as a string
            component: Component that produces the Event
            args: List of arguments in tuple form. Each tuple should be:
                  (arg name, arg description, arg obj). Where arg obj is an
                  object of a type derived from the class Base Type. Arg
                  description may be None.
            severity: event severity as an EventSeverity Enum
            format_str: Format string for the event's arguments
            description: (Optional) Event Description
        '''

        # Make sure correct types are passed
        if not type(event_id) == type(int()):
            raise TypeMismatchException(type(int()),type(event_id))

        if not type(name) == type(str()):
            raise TypeMismatchException(type(str()), type(name))

        if not type(component) == type(str()):
            raise TypeMismatchException(type(str()), type(component))

        if not type(format_str) == type(str()):
            raise TypeMismatchException(type(str()), type(format_str))

        if not type(args) == type(list()):
            raise TypeMismatchException(type(list()),type(args))

        for (arg_name, arg_desc, arg_type) in args:
            if not type(arg_name) == type(str()):
                raise TypeMismatchException(type(str()),type(arg_name))

            if arg_desc != None and not type(arg_desc) == type(str()):
                raise TypeMismatchException(type(str()),type(arg_desc))

            if not issubclass(type(arg_type), type(type_base.BaseType())):
                raise TypeMismatchException(type(type_base.BaseType()),type(arg_type))

        if description != None and not type(description) == type(str()):
            raise TypeMismatchException(type(str()), type(description))

        if not isinstance(severity, EventSeverity):
            raise TypeMismatchException("EventSeverity", type(severity))

        # Initialize event internal variables
        self.id          = event_id
        self.name        = name
        self.comp_name   = component
        self.args        = args
        self.severity    = severity
        self.format_str  = format_str
        self.description = description


    def get_full_name(self):
        '''
        Get the full name of this event

        Returns:
            The full name (component.channel) for this event
        '''
        return ("%s.%s"%(self.comp_name, self.name))

    def get_id(self):
        return self.id

    def get_name(self):
        return self.name

    def get_comp_name(self):
        return self.comp_name

    def get_severity(self):
        '''
        Returns the event's severity as an EventSeverity Enum.

        Returns:
            The event's severity as an EventSeverity Enum
        '''
        return self.severity

    def get_format_str(self):
        return self.format_str

    def get_description(self):
        self.description

    def get_args(self):
        '''
        Returns a list of argument information

        Returns:
            A list of tuples where each tuple represents an argument. Each tuple
            in the form: (arg name, arg description, arg obj). Where arg obj is
            an object of a type derived from the class Base Type. Arg
            description may be None.
        '''
        return self.args


if __name__ == '__main__':
    pass
