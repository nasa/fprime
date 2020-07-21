"""
@brief Channel Template class

Instances of this class describe a specific telemetry channel (but not a channel
reading)

@date Created July 11, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime.common.models.serialize.type_base import BaseType
from fprime.common.models.serialize.type_exceptions import TypeMismatchException

from . import data_template


class ChTemplate(data_template.DataTemplate):
    """Class for channel templates that describe specific telemetry channels"""

    def __init__(
        self,
        ch_id,
        ch_name,
        comp_name,
        ch_type_obj,
        ch_fmt_str=None,
        ch_desc=None,
        low_red=None,
        low_orange=None,
        low_yellow=None,
        high_yellow=None,
        high_orange=None,
        high_red=None,
    ):
        """
        Constructor

        Args:
            ch_id: The ID of the channel being described
            ch_name: The name of the channel
            comp_name: The name of the f-prime component that produces this ch
            ch_type_obj: The channel's type as an instance of a class derived
                         from BaseType
            ch_fmt_str: (Optional) The format string for the channel
            ch_desc: (Optional) Description of the channel
            low_red: (Optional) Below this the value will be in red alert
            low_orange: (Optional) Below this the value will be in orange alert
            low_yellow: (Optional) Below this the value will be in yellow alert
            high_yellow: (Optional) Above this the value will be in yellow alert
            high_orange: (Optional) Above this the value will be in orange alert
            high_red: (Optional) Above this the value will be in red alert
        """
        super().__init__()
        # Make sure correct types are passed
        if not isinstance(ch_id, int):
            raise TypeMismatchException(int, type(ch_id))

        if not isinstance(ch_name, str):
            raise TypeMismatchException(str, type(ch_name))

        if not isinstance(comp_name, str):
            raise TypeMismatchException(str, type(comp_name))

        if not isinstance(ch_type_obj, BaseType):
            raise TypeMismatchException(BaseType, type(ch_type_obj))

        if ch_fmt_str is not None and not isinstance(ch_fmt_str, str):
            raise TypeMismatchException(str, type(ch_fmt_str))

        if ch_desc is not None and not isinstance(ch_desc, str):
            raise TypeMismatchException(str, type(ch_desc))

        # Initialize event internal variables
        self.id = ch_id
        self.name = ch_name
        self.comp_name = comp_name
        self.ch_desc = ch_desc
        self.ch_type_obj = ch_type_obj
        self.fmt_str = ch_fmt_str
        self.low_red = low_red
        self.low_orange = low_orange
        self.low_yellow = low_yellow
        self.high_yellow = high_yellow
        self.high_orange = high_orange
        self.high_red = high_red

    def get_full_name(self):
        """
        Get the full name of this channel

        Returns:
            The full name (component.channel) for this channel
        """
        return "{}.{}".format(self.comp_name, self.name)

    def get_id(self):
        return self.id

    def get_name(self):
        return self.name

    def get_comp_name(self):
        return self.comp_name

    def get_ch_desc(self):
        return self.ch_desc

    def get_type_obj(self):
        return self.ch_type_obj

    def get_format_str(self):
        return self.fmt_str

    def get_low_red(self):
        return self.low_red

    def get_low_orange(self):
        return self.low_orange

    def get_low_yellow(self):
        return self.low_yellow

    def get_high_yellow(self):
        return self.high_yellow

    def get_high_orange(self):
        return self.high_orange

    def get_high_red(self):
        return self.high_red
