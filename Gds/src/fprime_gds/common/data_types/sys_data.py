"""
@brief Base class for system data classes.

This class defines the interface for cata classes which are intended to hold
a specific data item (packet, channel, event). This data item includes the time
of the data as well as data such as channel value or argument value.

@date Created July 2, 2018
@author R. Joseph Paetz (rpaetz@jpl.nasa.gov)

@bug No known bugs
"""

import fprime_gds.common.utils.jsonable
from fprime.common.models.serialize import time_type
from fprime_gds.common.templates import data_template


class SysData:
    """
    The SysData class defines the interface for system data classes which are
    for specific data readings/events
    """

    def __init__(self):
        """
        Constructor.

        Each subclass will define new constructors with necessary arguments.
        The necessary fields are time, id, and template.

        Returns:
            An initialized SysData object
        """
        if not hasattr(self, "id"):
            self.id = 0
        if not hasattr(self, "template"):
            self.template = data_template.DataTemplate()
        if not hasattr(self, "time"):
            self.time = time_type.TimeType()

    def get_id(self):
        """
        Returns the id of the channel

        Returns:
            The id of the channel
        """
        return self.id

    def get_time(self):
        """
        Returns the time of the channel data reading

        Returns:
            Time of the reading as a TimeType
        """
        return self.time

    def get_template(self):
        """
        Returns the template class instance for the data stored

        Returns:
            An instance of a template class for this instance's data
        """
        return self.template

    def to_jsonable(self):
        """
        Converts to a JSONable object (primitives, anon-objects, lists)
        """
        return fprime_gds.common.utils.jsonable.fprime_to_jsonable(self)

    @staticmethod
    def compare(x, y):
        """
        Compares two data items.

        Returns:
            Negative, 0, or positive for t1<t2, t1==t2, t1>t2 respectively
        """
        # Compare by time first
        time_comp = time_type.TimeType.compare(x.time, y.time)

        if time_comp != 0:
            return time_comp

        # Compare by id second (just let multiple events at the same time with
        # the same id be counted as equal
        return time_type.TimeType.compare(x.id, y.id)
