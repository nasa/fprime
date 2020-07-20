"""
@file time_tag.py
@brief Class used to parse and store time tags sent with serialized data

This Class is used to parse, store, and create human readable strings for the
time tags sent with serialized data in the fprime architecture.

@date Created Dec 16, 2015
@author: dinkel

@date Updated June 18, 2018
@author R. Joseph Paetz (rpaetz@jpl.nasa.gov)

@date Updated July 22, 2019
@author Kevin C Oran (kevin.c.oran@jpl.nasa.gov)

@bug No known bugs
"""
from __future__ import print_function
from __future__ import absolute_import

import datetime

from enum import Enum
import math

# Custom Python Modules
import fprime.common.models.serialize.numerical_types
from fprime.common.models.serialize.type_exceptions import TypeException
from fprime.common.models.serialize.type_exceptions import TypeRangeException
from fprime.common.models.serialize import type_base

TimeBase = Enum(
    "TimeBase",
    # No time base has been established
    {
        "TB_NONE": 0,
        # Processor cycle time. Not tried to external time
        "TB_PROC_TIME": 1,
        # Time on workstation where software is running. For testing.
        "TB_WORKSTATION_TIME": 2,
        # Time as reported by the SCLK.
        "TB_SC_TIME": 3,
        # Time as reported by the FPGA clock
        "TB_FPGA_TIME": 4,
        # Don't care value for sequences
        "TB_DONT_CARE": 0xFFFF,
    },
)


class TimeType(type_base.BaseType):
    """
    Representation of the time type

    Used to parse, store, and create human readable versions of the time tags
    included in serialized output from fprime_gds systems
    """

    def __init__(self, time_base=0, time_context=0, seconds=0, useconds=0):
        """
        Constructor

        Args
            time_base (int): Time base index for the time tag. Must be a valid
                             integer for a TimeBase Enum value.
            time_context (int): Time context for the time tag
            seconds (int): Seconds elapsed since specified time base
            useconds (int): Microseconds since start of current second. Must
                            be in range [0, 999999] inclusive

        Returns:
            An initialized TimeType object
        """
        # Layout of time tag:
        #
        # START (LSB)
        # |  2 bytes  |    1 byte    | 4 bytes |   4 bytes    |
        # |-----------|--------------|---------|--------------|
        # | Time Base | Time Context | Seconds | Microseconds |
        super().__init__()
        self._check_time_base(time_base)
        self._check_useconds(useconds)

        self.__timeBase = fprime.common.models.serialize.numerical_types.U16Type(time_base)
        self.__timeContext = fprime.common.models.serialize.numerical_types.U8Type(time_context)
        self.__secs = fprime.common.models.serialize.numerical_types.U32Type(seconds)
        self.__usecs = fprime.common.models.serialize.numerical_types.U32Type(useconds)

    def _check_useconds(self, useconds):
        """
        Checks if a given microsecond value is valid.

        Args:
            usecs (int): The value to check

        Returns:
            None if valid, raises TypeRangeException if not valid.
        """
        if (useconds < 0) or (useconds > 999999):
            raise TypeRangeException(useconds)

    def _check_time_base(self, time_base):
        """
        Checks if a given TimeBase value is valid.

        Args:
            time_base (int): The value to check

        Returns:
            Returns if valid, raises TypeRangeException if not valid.
        """
        # Construct list of possible values for TimeBase enum
        valid_vals = [member.value for member in list(TimeBase)]

        if time_base not in valid_vals:
            raise TypeRangeException(time_base)

    def to_jsonable(self):
        """
        JSONable object format
        """
        return {
            "type": self.__repr__(),
            "base": self.__timeBase,
            "context": self.__timeContext,
            "seconds": self.seconds,
            "microseconds": self.useconds,
        }

    @property
    def timeBase(self):
        return TimeBase(self.__timeBase.val)

    @timeBase.setter
    def timeBase(self, val):
        self._check_time_base(val)
        self.__timeBase = fprime.common.models.serialize.numerical_types.U16Type(val)

    @property
    def timeContext(self):
        return self.__timeContext.val

    @timeContext.setter
    def timeContext(self, val):
        self.__timeContext = fprime.common.models.serialize.numerical_types.U8Type(val)

    @property
    def seconds(self):
        return self.__secs.val

    @seconds.setter
    def seconds(self, val):
        self.__secs = fprime.common.models.serialize.numerical_types.U32Type(val)

    @property
    def useconds(self):
        return self.__usecs.val

    @useconds.setter
    def useconds(self, val):
        self._check_useconds(val)
        self.__usecs = fprime.common.models.serialize.numerical_types.U32Type(val)

    def serialize(self):
        """
        Serializes the time type

        Returns:
            Byte array containing serialized time type
        """
        buf = b""
        buf += self.__timeBase.serialize()
        buf += self.__timeContext.serialize()
        buf += self.__secs.serialize()
        buf += self.__usecs.serialize()
        return buf

    def deserialize(self, data, offset):
        """
        Deserializes a serialized time type in data starting at offset.

        Internal values to the object are updated.

        Args:
            data: binary data containing the time tag (type = bytearray)
            offset: Index in data where time tag starts
        """

        # Decode Time Base
        self.__timeBase.deserialize(data, offset)
        offset += self.__timeBase.getSize()

        # Decode Time Context
        self.__timeContext.deserialize(data, offset)
        offset += self.__timeContext.getSize()

        # Decode Seconds
        self.__secs.deserialize(data, offset)
        offset += self.__secs.getSize()

        # Decode Microseconds
        self.__usecs.deserialize(data, offset)
        offset += self.__usecs.getSize()

    def getSize(self):
        """
        Return the size of the time type object when serialized

        Returns:
            The size of the time type object when serialized
        """
        return (
            self.__timeBase.getSize()
            + self.__timeContext.getSize()
            + self.__secs.getSize()
            + self.__usecs.getSize()
        )

    @staticmethod
    def compare(t1, t2):
        """
        Compares two TimeType objects

        This function sorts times in the order of: timeBase, secs, usecs, and
        then timeContext.

        Returns:
            Negative, 0, or positive for t1<t2, t1==t2, t1>t2 respectively
        """

        def cmp(x, y):
            return (x > y) - (x < y)  # added to support Python 2/3

        # Compare Base
        base_cmp = cmp(t1.timeBase.value, t2.timeBase.value)
        if base_cmp != 0:
            return base_cmp

        # Compare seconds
        sec_cmp = cmp(t1.seconds, t2.seconds)
        if sec_cmp != 0:
            return sec_cmp

        # Compare usecs
        usec_cmp = cmp(t1.useconds, t2.useconds)
        if usec_cmp != 0:
            return usec_cmp

        # Compare contexts
        return cmp(t1.timeContext, t2.timeContext)

    def __str__(self):
        """
        Formats the time type object for printing

        Returns:
            A string representing the time type object
        """
        return "(%d(%d)-%d:%d)" % (
            self.__timeBase.val,
            self.__timeContext.val,
            self.__secs.val,
            self.__usecs.val,
        )

    def to_readable(self, time_zone=None):
        """
        Returns a string of the time object in a human readable format

        Args:
            time_zone (tzinfo, default=None): Time zone to convert the TimeType
                      object to before printing. Timezone also displayed.
                      If time_zone=None, local timezone is used.

        Returns:
            A human readable string reperesenting the time type object
        """
        dt = self.get_datetime(time_zone)

        # If we could convert to a valid datetime, use that, otherwise, format
        # TODO use time_zone arg
        if dt:
            return dt.strftime("%Y-%m-%d %H:%M:%S%z")
        else:
            return "%s: %d.%06ds, context=%d" % (
                TimeBase(self.__timeBase.val).name,
                self.__secs.val,
                self.__usecs.val,
                self.__timeContext.val,
            )

    def get_datetime(self, tz=None):
        """
        Returns the python datetime object for UTC time

        Args:
            tz (tzinfo, default=None): timezone to create the datetime object
               in. If tz=None, local time zone used.
        Returns:
            datetime object for the time type or None if the time couldn't
            be determined.
        """

        tb = TimeBase(self.__timeBase.val)
        dt = None

        if tb == TimeBase["TB_WORKSTATION_TIME"] or tb == TimeBase["TB_SC_TIME"]:

            # This finds the local time corresponding to the timestamp and
            # timezone object, or local time zone if tz=None
            dt = datetime.datetime.fromtimestamp(self.__secs.val, tz)

            dt = dt.replace(microsecond=self.__usecs.val)

        return dt

    def set_datetime(self, dt, time_base=0xFFFF):
        """
        Sets the timebase from a datetime object.

        Args:
            dt (datetime): datetime object to read from time.
        """
        total_seconds = (dt - datetime.datetime.fromtimestamp(0)).total_seconds()
        seconds = int(total_seconds)
        useconds = int((total_seconds - seconds) * 1000000)

        self._check_time_base(time_base)
        self._check_useconds(useconds)

        self.__timeBase = fprime.common.models.serialize.numerical_types.U16Type(time_base)
        self.__secs = fprime.common.models.serialize.numerical_types.U32Type(seconds)
        self.__usecs = fprime.common.models.serialize.numerical_types.U32Type(useconds)

    def __repr__(self):
        return "Time"

    """
    The following Python special methods add support for rich comparison of TimeTypes to other
    TimeTypes and numbers.
    Note: comparisons support comparing to numbers or other instances of TimeType. If comparing to
    another TimeType, these comparisons use the provided compare method. See TimeType.compare for
    a description of this behavior.
    """

    def __get_float(self):
        """
        a helper method that gets the current TimeType as a float where the non-fraction is seconds
        and the fraction is microseconds. This enables comparisons with numbers.
        """
        return self.seconds + (self.useconds / 1000000)

    def __lt__(self, other):
        if isinstance(other, TimeType):
            return self.compare(self, other) < 0
        else:
            return self.__get_float() < other

    def __le__(self, other):
        if isinstance(other, TimeType):
            return self.compare(self, other) <= 0
        else:
            return self.__get_float() <= other

    def __eq__(self, other):
        if isinstance(other, TimeType):
            return self.compare(self, other) == 0
        else:
            return self.__get_float() == other

    def __ne__(self, other):
        if isinstance(other, TimeType):
            return self.compare(self, other) != 0
        else:
            return self.__get_float() != other

    def __gt__(self, other):
        if isinstance(other, TimeType):
            return self.compare(self, other) > 0
        else:
            return self.__get_float() > other

    def __ge__(self, other):
        if isinstance(other, TimeType):
            return self.compare(self, other) >= 0
        else:
            return self.__get_float() >= other

    """
    The following helper methods enable support for arithmetic operations on TimeTypes.
    """

    def __set_float(self, num):
        """
        a helper method that takes a float and sets a TimeType's seconds and useconds fields.
        Note: This method is private because it is only used by the _get_type_from_float helper to
        generate new TimeType instances. It is not meant to be used to modify an existing timestamp.
        Note: Present implementation will set any negative result to 0
        """
        num = max(num, 0)
        self.seconds = int(math.floor(num))
        self.useconds = int(round((num - self.seconds) * 1000000))

    def __get_type_from_float(self, num):
        """
        a helper method that returns a new instance of TimeType and sets the seconds and useconds
        fields using the given number. The new TimeType's time_base and time_context will be
        preserved from the calling object.
        """
        tType = TimeType(self.__timeBase.val, self.__timeContext.val)
        tType.__set_float(num)
        return tType

    """
    The following Python special methods add support for arithmetic operations on TimeTypes.
    """

    def __add__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = self.__get_float() + other
        return self.__get_type_from_float(num)

    def __sub__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = self.__get_float() - other
        return self.__get_type_from_float(num)

    def __mul__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = self.__get_float() * other
        return self.__get_type_from_float(num)

    def __truediv__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = self.__get_float() / other
        return self.__get_type_from_float(num)

    def __floordiv__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = self.__get_float() // other
        return self.__get_type_from_float(num)

    """
    The following Python special methods add support for reflected arithmetic operations on
    TimeTypes.
    """

    def __radd__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = other + self.__get_float()
        return self.__get_type_from_float(num)

    def __rsub__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = other - self.__get_float()
        return self.__get_type_from_float(num)

    def __rmul__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = other * self.__get_float()
        return self.__get_type_from_float(num)

    def __rtruediv__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = other / self.__get_float()
        return self.__get_type_from_float(num)

    def __rfloordiv__(self, other):
        if isinstance(other, TimeType):
            other = other.__get_float()
        num = other // self.__get_float()
        return self.__get_type_from_float(num)


def ser_deser_test(t_base, t_context, secs, usecs, should_err=False):
    """
    Test serialization/deserialization of TimeType objects.

    This test function creates a time type object with the given parameters and
    then serializes it and deserializes it. Also prints it for visual inspection
    of the formatted output.

    Args:
        t_base (int): Time base for the new time type object
        t_context (int): Time context for the new time type object
        secs (int): Seconds value for the new time type object
        usecs (int): Seconds value for the new time type object
        should_err (int): True if error expected, else False

    Returns:
        True if test passed, False otherwise
    """
    print("\n")

    try:
        val = TimeType(t_base, t_context, secs, usecs)
        print(("creating: TimeType(%d, %d, %d, %d)" % (t_base, t_context, secs, usecs)))
        print((str(val)))

        buff = val.serialize()
        print(("Serialized: %s" % repr(buff)))
        type_base.showBytes(buff)

        val2 = TimeType()
        val2.deserialize(buff, 0)
        print(
            "Deserialized: TimeType(%s, %d, %d, %d)"
            % (val2.timeBase.value, val2.timeContext, val2.seconds, val2.useconds)
        )

        if val2.timeBase.value != t_base:
            return False
        elif val2.timeContext != t_context:
            return False
        elif val2.seconds != secs:
            return False
        elif val2.useconds != usecs:
            return False
        else:
            return True
    except TypeException as e:
        print("Exception: %s" % e.getMsg())
        if isinstance(e, TypeRangeException) and should_err:
            return True
        else:
            return False
