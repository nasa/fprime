"""
@brief Event Template class

Instances of this class describe a specific event type. For example: AF_ASSERT_0
or cmdSeq_CS_CmdStarted

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime.common.models.serialize import type_base
from fprime.common.models.serialize.type_exceptions import TypeMismatchException
from fprime_gds.common.utils.event_severity import EventSeverity

from . import data_template


class EventTemplate(data_template.DataTemplate):
    """Class to create event templates to describe specific event types"""

    def __init__(
        self, event_id, name, component, args, severity, format_str, description=None
    ):
        """
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
        """
        super().__init__()
        # Make sure correct types are passed
        if not isinstance(event_id, int):
            raise TypeMismatchException(int, type(event_id))

        if not isinstance(name, str):
            raise TypeMismatchException(str, type(name))

        if not isinstance(component, str):
            raise TypeMismatchException(str, type(component))

        if not isinstance(format_str, str):
            raise TypeMismatchException(str, type(format_str))

        if not isinstance(args, list):
            raise TypeMismatchException(list, type(args))

        for (arg_name, arg_desc, arg_type) in args:
            if not isinstance(arg_name, str):
                raise TypeMismatchException(str, type(arg_name))

            if arg_desc is not None and not isinstance(arg_desc, str):
                raise TypeMismatchException(str, type(arg_desc))

            if not isinstance(arg_type, type_base.BaseType):
                raise TypeMismatchException(type_base.BaseType, type(arg_type))

        if description is not None and not isinstance(description, str):
            raise TypeMismatchException(str, type(description))

        if not isinstance(severity, EventSeverity):
            raise TypeMismatchException("EventSeverity", type(severity))

        # Initialize event internal variables
        self.id = event_id
        self.name = name
        self.comp_name = component
        self.args = args
        self.severity = severity
        self.format_str = format_str
        self.description = description

    def get_full_name(self):
        """
        Get the full name of this event

        Returns:
            The full name (component.channel) for this event
        """
        return "{}.{}".format(self.comp_name, self.name)

    def get_id(self):
        return self.id

    def get_name(self):
        return self.name

    def get_comp_name(self):
        return self.comp_name

    def get_severity(self):
        """
        Returns the event's severity as an EventSeverity Enum.

        Returns:
            The event's severity as an EventSeverity Enum
        """
        return self.severity

    def get_format_str(self):
        return self.format_str

    def get_description(self):
        return self.description

    def get_args(self):
        """
        Returns a list of argument information

        Returns:
            A list of tuples where each tuple represents an argument. Each tuple
            in the form: (arg name, arg description, arg obj). Where arg obj is
            an object of a type derived from the class Base Type. Arg
            description may be None.
        """
        return self.args


if __name__ == "__main__":
    pass
