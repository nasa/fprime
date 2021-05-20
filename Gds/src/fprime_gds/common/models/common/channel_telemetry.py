"""
Created on Apr. 27, 2015

@author: reder
"""

from fprime.common.models.serialize.type_base import BaseType

# Import the types this way so they do not need prefixing for execution.
from fprime.common.models.serialize.type_exceptions import (
    TypeException,
    TypeMismatchException,
)


class Channel:
    """
    Channel class is for deserialize channel telemetry value.
    Really this is a container since the type will have it's own deserialize
    implementation.
    """

    def __init__(
        self,
        name,
        channel_id,
        comp_name,
        ch_description,
        ch_type,
        ch_format_string,
        low_red,
        low_orange,
        low_yellow,
        high_yellow,
        high_orange,
        high_red,
    ):
        """
        Constructor
        """
        #
        # Make sure correct types are passed
        #
        if not isinstance(name, str):
            raise TypeMismatchException(str, type(name))

        if not isinstance(channel_id, int):
            raise TypeMismatchException(int, type(channel_id))

        if not isinstance(ch_description, str):
            raise TypeMismatchException(str, type(ch_description))

        if not isinstance(ch_type, BaseType):
            raise TypeMismatchException(BaseType, type(ch_type))

        # Initialize event internal variables
        self.__name = name
        self.__comp_name = comp_name
        self.__id = channel_id
        self.__ch_desc = ch_description
        self.__ch_type = ch_type
        self.__format_string = ch_format_string
        self.__low_red = low_red
        self.__low_orange = low_orange
        self.__low_yellow = low_yellow
        self.__high_yellow = high_yellow
        self.__high_orange = high_orange
        self.__high_red = high_red
        #
        self.__time_base = None
        self.__time_context = None
        self.__time_sec = None
        self.__time_usec = None
        #
        self.__changed = False

    def deserialize(self, ser_data, offset):
        """
        Deserialize event arguments
        :param ser_data: Binary input of the channel value.
        :param offset: offset in data to deserialize from
        :return: value from deserialized channel
        """
        # type_base.showBytes(ser_data[offset:])
        #
        try:
            self.__ch_type.deserialize(ser_data, offset)
            val = self.__ch_type.val
        except TypeException as e:
            print("Channel deserialize exception %s" % (e.getMsg()))
            val = "ERR"

        #
        return val

    def setTime(self, time_base, time_context, time_sec, time_usec):
        """
        Channel telemetry time updater.
        """
        self.__time_base = time_base
        self.__time_context = time_context
        self.__time_sec = time_sec
        self.__time_usec = time_usec

    def getTime(self):
        """
        Return time tuple for UI updater use.
        """
        return (
            self.__time_base,
            self.__time_context,
            self.__time_sec,
            self.__time_usec,
        )

    def getName(self):
        return self.__name

    def getCompName(self):
        return self.__comp_name

    def getId(self):
        return self.__id

    def getChDesc(self):
        return self.__ch_desc

    def getType(self):
        return self.__ch_type

    def getTimeBase(self):
        return self.__time_base

    def getTimeContext(self):
        return self.__time_context

    def getTimeSec(self):
        return self.__time_sec

    def getTimeUsec(self):
        return self.__time_usec

    def getFormatString(self):
        return self.__format_string

    def getLowRed(self):
        return self.__low_red

    def getLowOrange(self):
        return self.__low_orange

    def getLowYellow(self):
        return self.__low_yellow

    def getHighYellow(self):
        return self.__high_yellow

    def getHighOrange(self):
        return self.__high_orange

    def getHighRed(self):
        return self.__high_red

    @property
    def changed(self):
        """
        change is True if recently updated.
        change is False if not changed.
        """
        return self.__changed

    @changed.setter
    def changed(self, ch):
        if not ch == False or not ch == True:
            ch = True
        self.__changed = ch
