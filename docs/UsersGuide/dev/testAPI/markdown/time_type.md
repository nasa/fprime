# TimeType Serializable

The TimeType object from the framework code has been modified to be easier to compare and do math operations on.

@file time_tag.py
@brief Class used to parse and store time tags sent with serialized data

This Class is used to parse, store, and create human readable strings for the
time tags sent with serialized data in the fprime architecture.

@date Created Dec 16, 2015
@author: dinkel

@date Updated June 18, 2018
@author R. Joseph Paetz ([rpaetz@jpl.nasa.gov](mailto:rpaetz@jpl.nasa.gov))

@date Updated July 22, 2019
@author Kevin C Oran ([kevin.c.oran@jpl.nasa.gov](mailto:kevin.c.oran@jpl.nasa.gov))

@bug No known bugs


#### class fprime.common.models.serialize.time_type.TimeBase()
Bases: `enum.Enum`

An enumeration.


#### class fprime.common.models.serialize.time_type.TimeType(time_base=0, time_context=0, seconds=0, useconds=0)
Bases: `fprime.common.models.serialize.type_base.BaseType`

Representation of the time type

Used to parse, store, and create human readable versions of the time tags
included in serialized output from fprime_gds systems


#### __init__(time_base=0, time_context=0, seconds=0, useconds=0)
Constructor

Args

    time_base (int): Time base index for the time tag. Must be a valid

        integer for a TimeBase Enum value.

    time_context (int): Time context for the time tag
    seconds (int): Seconds elapsed since specified time base
    useconds (int): Microseconds since start of current second. Must

    > be in range [0, 999999] inclusive

Returns:

    An initialized TimeType object


#### _check_useconds(useconds)
Checks if a given microsecond value is valid.

Args:

    usecs (int): The value to check

Returns:

    None if valid, raises TypeRangeException if not valid.


#### _check_time_base(time_base)
Checks if a given TimeBase value is valid.

Args:

    time_base (int): The value to check

Returns:

    Returns if valid, raises TypeRangeException if not valid.


#### to_jsonable()
JSONable object format


#### serialize()
Serializes the time type

Returns:

    Byte array containing serialized time type


#### deserialize(data, offset)
Deserializes a serialized time type in data starting at offset.

Internal values to the object are updated.

Args:

    data: binary data containing the time tag (type = bytearray)
    offset: Index in data where time tag starts


#### getSize()
Return the size of the time type object when serialized

Returns:

    The size of the time type object when serialized


#### static compare(t1, t2)
Compares two TimeType objects

This function sorts times in the order of: timeBase, secs, usecs, and
then timeContext.

Returns:

    Negative, 0, or positive for t1<t2, t1==t2, t1>t2 respectively


#### __str__()
Formats the time type object for printing

Returns:

    A string representing the time type object


#### to_readable(time_zone=None)
Returns a string of the time object in a human readable format

Args:

    time_zone (tzinfo, default=None): Time zone to convert the TimeType

        object to before printing. Timezone also displayed.
        If time_zone=None, local timezone is used.

Returns:

    A human readable string representing the time type object


#### get_datetime(tz=None)
Returns the python datetime object for UTC time

Args:

    tz (tzinfo, default=None): timezone to create the datetime object

        in. If tz=None, local time zone used.

Returns:

    datetime object for the time type or None if the time couldn’t
    be determined.


#### __repr__()
Return repr(self).


#### __lt__(other)
Return self<value.


#### __le__(other)
Return self<=value.


#### __eq__(other)
Return self==value.


#### __ne__(other)
Return self!=value.


#### __gt__(other)
Return self>value.


#### __ge__(other)
Return self>=value.


#### _TimeType__get_float()
a helper method that gets the current TimeType as a float where the non-fraction is seconds
and the fraction is microseconds. This enables comparisons with numbers.


#### _TimeType__get_type_from_float(num)
a helper method that returns a new instance of TimeType and sets the seconds and useconds
fields using the given number. The new TimeType’s time_base and time_context will be
preserved from the calling object.


#### _TimeType__set_float(num)
a helper method that takes a float and sets a TimeType’s seconds and useconds fields.
Note: This method is private because it is only used by the _get_type_from_float helper to
generate new TimeType instances. It is not meant to be used to modify an existing timestamp.
Note: Present implementation will set any negative result to 0


#### fprime.common.models.serialize.time_type.ser_deser_test(t_base, t_context, secs, usecs, should_err=False)
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

|Quick Links|
|:----------|
|[Integration Test API User Guide](../user_guide.md)|
|[GDS Overview](https://github.com/fprime-community/fprime-gds)|
|[Integration Test API](integration_test_api.md)|
|[Histories](histories.md)|
|[Predicates](predicates.md)|
|[Test Logger](test_logger.md)|
|[Standard Pipeline](standard_pipeline.md)|
|[TimeType Serializable](time_type.md)|
