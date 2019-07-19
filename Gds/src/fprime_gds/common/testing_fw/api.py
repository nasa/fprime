"""
gds_test_api.py:

This file contains basic asserts that can support integration tests on an FPrime
deployment. This API uses the standard pipeline to get access to commands, events,
telemetry and dictionaries.

:author: koran
"""
import time
import signal

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.history.test import TestHistory
from fprime_gds.common.logger.test_logger import TestLogger
from fprime_gds.common.utils.event_severity import EventSeverity
from fprime.common.models.serialize.time_type import TimeType


class IntegrationTestAPI:
    """
    A value used to begin searches after the current contents in a history and only search future
    items
    """
    NOW = "NOW"

    def __init__(self, pipeline, logname=None):
        """
        Initializes API: constructs and registers test histories.
        Args:
            pipeline: a pipeline object providing access to basic GDS functionality
        """
        self.pipeline = pipeline
        # these are owned by the GDS and will not be modified by the test API.
        self.aggregate_command_history = pipeline.get_command_history()
        self.aggregate_telemetry_history = pipeline.get_channel_history()
        self.aggregate_event_history = pipeline.get_event_history()

        # these histories are owned by the TestAPI and are modified by the API.
        self.command_history = TestHistory()
        self.pipeline.register_command_consumer(self.command_history)
        self.telemetry_history = TestHistory()
        self.pipeline.register_telemetry_consumer(self.telemetry_history)
        self.event_history = TestHistory()
        self.pipeline.register_event_consumer(self.event_history)

        # Initialize latest time. Will be updated whenever a time query is made.
        self.latest_time = TimeType().useconds

        # Initialize the logger
        if logname is not None:
            self.logger = TestLogger(logname)
        else:
            self.logger = None
        
        # A predicate used as a filter to choose which EVR's to log automatically
        self.event_log_filter = self.get_event_predicate()
        self.pipeline.register_event_consumer(self)

    def teardown(self):
        """
        To be called once at the end of the API's use. Closes the test log and clears histories.
        """
        self.clear_histories()
        if self.logger is not None:
            self.logger.close_log()
            self.logger = None

    ######################################################################################
    #   API Functions
    ######################################################################################
    def start_test_case(self, case_name, case_id):
        """
        To be called at the start of a test case. This function inserts a log message to denote a
        new test case is beginning, records the latest time stamp in case the user clears the
        aggregate histories, and then clears the API's histories.

        Args:
            case_name: the name of the test case (str)
            case_id: a short identifier to denote the test case (str or number)
        """
        msg = "[STARTING CASE] {}".format(case_name)
        self.__log(msg, TestLogger.GRAY, TestLogger.BOLD, case_id=case_id)
        self.get_latest_fsw_time()  # called in case aggregate histories are cleared by the user
        self.clear_histories()

    def log_test_message(self, msg, color=None):
        """
        User-accessible function to log user messages to the test log.
        Args:
            msg: a user-provided message to add to the test log.
            color: a string containing a color hex code "######"
        """
        self.__log(msg, color, sender="Test API user")

    def get_latest_fsw_time(self):
        """
        Finds the latest flight software time received by either history.

        Returns:
            a flight software timestamp (TimeType)
        """
        events = self.aggregate_event_history.retrieve()
        e_time = TimeType().useconds
        if len(events) > 0:
            e_time = events[-1].get_time().useconds

        channels = self.aggregate_telemetry_history.retrieve()
        t_time = TimeType().useconds
        if len(channels) > 0:
            t_time = channels[-1].get_time().useconds

        self.latest_time = max(e_time, t_time, self.latest_time)
        return self.latest_time

    def clear_histories(self, time_stamp=None):
        """
        Clears the IntegrationTestAPI's histories. Because the command history is not correlated to
        a flight software timestamp, it will be cleared entirely. This function can be used to set
        up test cases so that the IntegrationTestAPI's histories only contain objects received
        during that test.
        Note: this will not clear user-created sub-histories nor the aggregate histories (histories
        owned by the gds)

        Args:
            time_stamp: If specified, histories are only cleared before the timestamp.
        """
        if time_stamp is not None:
            time_pred = predicates.greater_than_or_equal_to(time_stamp)
            e_pred = predicates.event_predicate(time_pred=time_pred)
            self.event_history.clear(e_pred)
            t_pred = predicates.telemetry_predicate(time_pred=time_pred)
            self.telemetry_history.clear(t_pred)
            msg = "Clearing Test Histories after {} us".format(time_stamp)
            self.__log(msg, TestLogger.WHITE)
        else:
            self.event_history.clear()
            self.telemetry_history.clear()
            msg = "Clearing Test Histories"
            self.__log(msg, TestLogger.WHITE)

        self.command_history.clear()

    def set_event_log_filter(self, event=None, args=None, time_pred=None):
        """
        Constructs an event predicate that is then used to filter which EVR's are interlaced in the
        test logs. This method replaces the current filter.

        Args:
            event: an optional mnemonic (str), id (int), or predicate to specify the event type
            args: an optional list of arguments (list of values, predicates, or None to ignore)
            time_pred: an optional predicate to specify the flight software timestamp
        """
        self.event_log_filter = self.get_event_predicate(event, args, time_pred)

    def get_command_test_history(self):
        """
        Accessor for IntegrationTestAPI's command history
        Returns:
            a history of CmdData objects
        """
        return self.command_history

    def get_telemetry_test_history(self):
        """
        Accessor for IntegrationTestAPI's telemetry history
        Returns:
            a history of ChData objects
        """
        return self.telemetry_history

    def get_event_test_history(self):
        """
        Accessor for IntegrationTestAPI's event history
        Returns:
            a history of EventData objects
        """
        return self.event_history

    ######################################################################################
    #   Command Functions
    ######################################################################################
    def translate_command_name(self, command):
        """
        This function will translate the given mnemonic into an ID as defined by the flight
        software dictionary. This call will raise an error if the command given is not in the
        dictionary.

        Args:
            channel: Either the channel id (int) or the channel mnemonic (str)

        Returns:
            The comand ID (int)
        """
        if isinstance(command, str):
            cmd_dict = self.pipeline.get_command_name_dictionary()
            if command in cmd_dict:
                return cmd_dict[command].get_id()
            else:
                msg = "The command mnemonic, {}, wasn't in the dictionary".format(command)
                raise KeyError(msg)
        else:
            cmd_dict = self.pipeline.get_command_id_dictionary()
            if command in cmd_dict:
                return command
            else:
                msg = "The command id, {}, wasn't in the dictionary".format(command)
                raise KeyError(msg)

    def send_command(self, command, args=[]):
        """
        Sends the specified command.
        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
        """
        msg = "Sending Command: {} {}".format(command, args)
        self.__log(msg, TestLogger.PURPLE)
        command = self.translate_command_name(command)
        self.pipeline.send_command(command, args)

    def send_and_await_telemetry(self, command, args=[], channels=[], timeout=5):
        """
        Sends the specified command and awaits the specified channel update or sequence of
        updates. See await_telemetry and await_telemetry_sequence for full details.
        Note: If awaiting a sequence avoid specifying timestamps.

        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
            channels: a single or a sequence of channel specs (event_predicates, mnemonics, or IDs)
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)

        Returns:
            The channel update or updates found by the search
        """
        start = self.telemetry_history.size()
        self.send_command(command, args)
        if isinstance(channels, list):
            return self.await_telemetry_sequence(channels, start=start, timeout=timeout)
        else:
            return self.await_telemetry(channels, start=start, timeout=timeout)

    def send_and_await_event(self, command, args=[], events=[], timeout=5):
        """
        Sends the specified command and awaits the specified event message or sequence of
        messages. See await_event and await event sequence for full details.
        Note: If awaiting a sequence avoid specifying timestamps.

        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
            events: a single or a sequence of event specifiers (event_predicates, mnemonics, or IDs)
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)

        Returns:
            The event or events found by the search
        """
        start = self.event_history.size()
        self.send_command(command, args)
        if isinstance(events, list):
            return self.await_event_sequence(events, start=start, timeout=timeout)
        else:
            return self.await_event(events, start=start, timeout=timeout)

    ######################################################################################
    #   Command Asserts
    ######################################################################################
    def send_and_assert_telemetry(self, command, args=[], channels=[], timeout=5):
        """
        Sends the specified command and asserts on the specified channel update or sequence of
        updates. See await_telemetry and await_telemetry_sequence for full details.
        Note: If awaiting a sequence avoid specifying timestamps.

        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
            channels: a single or a sequence of channel specs (event_predicates, mnemonics, or IDs)
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)

        Returns:
            The channel update or updates found by the search
        """
        start = self.telemetry_history.size()
        self.send_command(command, args)
        if isinstance(channels, list):
            return self.assert_telemetry_sequence(channels, start=start, timeout=timeout)
        else:
            return self.assert_telemetry(channels, start=start, timeout=timeout)

    def send_and_assert_event(self, command, args=[], events=[], timeout=5):
        """
        Sends the specified command and asserts on the specified event message or sequence of
        messages. See assert_event and assert event sequence for full details.

        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
            events: a single or a sequence of event specifiers (event_predicates, mnemonics, or IDs)
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)

        Returns:
            The event or events found by the search
        """
        start = self.event_history.size()
        self.send_command(command, args)
        if isinstance(events, list):
            return self.assert_event_sequence(events, start=start, timeout=timeout)
        else:
            return self.assert_event(events, start=start, timeout=timeout)

    ######################################################################################
    #   Telemetry Functions
    ######################################################################################
    def translate_telemetry_name(self, channel):
        """
        This function will translate the given mnemonic into an ID as defined by the flight
        software dictionary. This call will raise an error if the channel given is not in the
        dictionary.

        Args:
            channel: a channel mnemonic (str) or id (int)
        Returns:
            the channel ID (int)
        """
        if isinstance(channel, str):
            ch_dict = self.pipeline.get_channel_name_dictionary()
            if channel in ch_dict:
                return ch_dict[channel].get_id()
            else:
                msg = "The telemetry mnemonic, {}, wasn't in the dictionary".format(channel)
                raise KeyError(msg)
        else:
            ch_dict = self.pipeline.get_channel_id_dictionary()
            if channel in ch_dict:
                return channel
            else:
                msg = "The telemetry mnemonic, {}, wasn't in the dictionary".format(channel)
                raise KeyError(msg)

    def get_telemetry_predicate(self, channel=None, value=None, time_pred=None):
        """
        This function will translate the channel ID, and construct a telemetry_predicate object. It
        is used as a helper by the IntegrationTestAPI, but could also be helpful to a user of the
        test API. If  channel is already an instance of telemetry_predicate, it will be returned
        immediately. The provided implementation of telemetry_predicate evaluates true if and only
        if all specified constraints are satisfied. If a specific constraint isn't specified, then
        it will not effect the outcome; this means all arguments are optional. If no constraints
        are specified, the predicate will always return true.


        Args:
            channel: an optional mnemonic (str), id (int), or predicate to specify the channel type
            value: an optional value (object/number) or predicate to specify the value field
            time_pred: an optional predicate to specify the flight software timestamp
        Returns:
            an instance of telemetry_predicate
        """
        if isinstance(channel, predicates.telemetry_predicate):
            return channel

        if not predicates.is_predicate(channel) and channel is not None:
            channel = self.translate_telemetry_name(channel)
            channel = predicates.equal_to(channel)

        if not predicates.is_predicate(value) and value is not None:
            value = predicates.equal_to(value)

        return predicates.telemetry_predicate(channel, value, time_pred)

    def await_telemetry(
        self, channel, value=None, time_pred=None, history=None, start="NOW", timeout=5
    ):
        """
        A search for a single telemetry update received. By default, the call will only await
        until a correct update is found. The user can specify that await also searches the current
        history by specifying a value for start. On timeout, the search will return None.

        Args:
            channel: a channel specifier (mnemonic, id, or predicate)
            value: optional value (object/number) or predicate to specify the value field
            time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the ChData object found during the search, otherwise, None
        """
        t_pred = self.get_telemetry_predicate(channel, value, time_pred)

        if history is None:
            history = self.get_telemetry_test_history()

        msg = "Awaiting a single telemetry update: {}".format(t_pred)
        self.__log(msg, TestLogger.YELLOW)

        return self.find_history_item(t_pred, history, start, timeout)

    def await_telemetry_sequence(self, channels, history=None, start="NOW", timeout=5):
        """
        A search for a sequence of telemetry updates. By default, the call will only await until
        the sequence is completed. The user can specify that await also searches the history by
        specifying a value for start. On timeout, the search will return the list of found
        channel updates regardless of whether the sequence is complete.
        Note: It is reccomended (but not enforced) not to specify timestamps for this assert.
        Note: This function will always return a list of updates. The user should check if the
        sequence was completed.

        Args:
            channels: an ordered list of channel specifiers (mnemonic, id, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            an ordered list of ChData objects that satisfies the sequence
        """
        seq_preds = []
        for channel in channels:
            seq_preds.append(self.get_telemetry_predicate(channel))

        if history is None:
            history = self.get_telemetry_test_history()

        msg = "Awaiting a sequence of {} telemetry updates.".format(len(seq_preds))
        self.__log(msg, TestLogger.YELLOW)

        return self.find_history_sequence(seq_preds, history, start, timeout)

    def await_telemetry_count(
        self, count, channels=None, history=None, start="NOW", timeout=5
    ):
        """
        A search on the number of telemetry updates received. By default, the call will only await
        until a correct count is achieved. The user can specify that await also searches the current
        history by specifying a value for start. On timeout, the search will return the list of
        found channel updates regardless of whether a correct count is achieved.
        Note: this search will always return a list of objects. The user should check if the search
        was completed.

        Args:
            count: either an exact amount (int) or a predicate to specify how many objects to find
            channels: a channel specifier or list of channel specifiers (mnemonic, ID, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            a list of the ChData objects that were counted
        """
        if channels is None:
            search = None
        elif isinstance(channels, list):
            t_preds = []
            for channel in channels:
                t_preds.append(self.get_telemetry_predicate(channel=channel))
            search = predicates.satisfies_any(t_preds)
        else:
            search = self.get_telemetry_predicate(channel=channels)

        if history is None:
            history = self.get_telemetry_test_history()

        msg = "Awaiting a count ({}) of telemetry updates.".format(count)
        self.__log(msg, TestLogger.YELLOW)

        return self.find_history_count(count, history, search, start, timeout)

    ######################################################################################
    #   Telemetry Asserts
    ######################################################################################
    def assert_telemetry(
        self, channel, value=None, time_pred=None, history=None, start=None, timeout=0
    ):
        """
        An assert on a single telemetry update received. If the history doesn't have the
        correct update, the call will await until a correct update is received or the
        timeout, at which point it will assert failure.

        Args:
            channel: a channel specifier (mnemonic, id, or predicate)
            value: optional value (object/number) or predicate to specify the value field
            time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the ChData object found during the search
        """
        pred = self.get_telemetry_predicate(channel, value, time_pred)
        result = self.await_telemetry(
            channel, value, time_pred, history, start, timeout
        )
        self.__assert_pred("Telemetry Received", pred, result)
        return result

    def assert_telemetry_sequence(self, channels, history=None, start=None, timeout=0):
        """
        A search for a sing sequence of telemetry updates messages. If the history doesn't have the
        complete sequence, the call will await until the sequence is completed or the timeout, at
        which point it will return the list of found channel updates.
        Note: It is reccomended (but not enforced) not to specify timestamps for this assert.
        Note: This function will always return a list of updates the user should check if the
        sequence was completed.

        Args:
            channels: an ordered list of channel specifiers (mnemonic, id, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            an ordered list of ChData objects that satisfies the sequence
        """
        results = self.await_telemetry_sequence(channels, history, start, timeout)
        len_pred = predicates.equal_to(len(channels))
        self.__assert_pred("Telemetry Sequence", len_pred, len(results))
        return results

    def assert_telemetry_count(
        self, count, channels=None, history=None, start=None, timeout=0
    ):
        """
        An assert on the number of channel updates received. If the history doesn't have the
        correct update count, the call will await until a correct count is achieved or the
        timeout, at which point it will assert failure.

        Args:
            count: either an exact amount (int) or a predicate to specify how many objects to find
            channels: a channel specifier or list of channel specifiers (mnemonic, ID, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            a list of the ChData objects that were counted
        """
        results = self.await_telemetry_count(count, channels, history, start, timeout)
        if predicates.is_predicate(count):
            count_pred = count
        elif isinstance(count, int):
            count_pred = predicates.equal_to(count)
        self.__assert_pred("Telemetry Count", count_pred, len(results))
        return results

    ######################################################################################
    #   Event Functions
    ######################################################################################
    def translate_event_name(self, event):
        """
        This function will translate the given mnemonic into an ID as defined by the
        flight software dictionary. This call will raise an error if the event given is
        not in the dictionary.

        Args:
            event: an event mnemonic (str) or ID (int)
        Returns:
            the event ID (int)
        """
        if isinstance(event, str):
            event_dict = self.pipeline.get_event_name_dictionary()
            if event in event_dict:
                return event_dict[event].get_id()
            else:
                msg = "The event mnemonic, {}, wasn't in the dictionary".format(event)
                raise KeyError(msg)
        else:
            event_dict = self.pipeline.get_event_id_dictionary()
            if event in event_dict:
                return event
            else:
                msg = "The event id, {}, wasn't in the dictionary".format(event)
                raise KeyError(msg)

    def get_event_predicate(self, event=None, args=None, severity=None, time_pred=None):
        """
        This function will translate the event ID, and construct an event_predicate object. It is
        used as a helper by the IntegrationTestAPI, but could also be helpful to a user of the test
        API. If event is already an instance of event_predicate, it will be returned immediately.
        The provided implementation of event_predicate evaluates true if and only if all specified
        constraints are satisfied. If a specific constraint isn't specified, then it will not
        effect the outcome; this means all arguments are optional. If no constraints are specified,
        the predicate will always return true.

        Args:
            event: mnemonic (str), id (int), or predicate to specify the event type
            args: list of arguments (list of values, predicates, or None to ignore)
            severity: an EventSeverity enum or a predicate to specify the event severity
            time_pred: predicate to specify the flight software timestamp
        Returns:
            an instance of event_predicate
        """
        if isinstance(event, predicates.event_predicate):
            return event

        if not predicates.is_predicate(event) and event is not None:
            event = self.translate_event_name(event)
            event = predicates.equal_to(event)

        if not predicates.is_predicate(args) and args is not None:
            args = predicates.args_predicate(args)

        if not predicates.is_predicate(severity) and severity is not None:
            if not isinstance(severity, EventSeverity):
                raise TypeMismatchException("EventSeverity", type(severity))
            severity = predicates.equal_to(severity)

        return predicates.event_predicate(event, args, severity, time_pred)

    def await_event(
        self, event, args=None, severity=None, time_pred=None, history=None, start="NOW", timeout=5
    ):
        """
        A search for a single event message received. By default, the call will only await until a
        correct message is found. The user can specify that await also searches the current history
        by specifying a value for start. On timeout, the search will return None.

        Args:
            event: an event specifier (mnemonic, id, or predicate)
            args: a list of expected arguments (list of values, predicates, or None for don't care)
            severity: an EventSeverity enum or a predicate to specify the event severity
            time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the EventData object found during the search, otherwise, None
        """
        e_pred = self.get_event_predicate(event, args, severity, time_pred)

        if history is None:
            history = self.get_event_test_history()

        msg = "Awaiting a single telemetry update: {}".format(e_pred)
        self.__log(msg, TestLogger.YELLOW)

        return self.find_history_item(e_pred, history, start, timeout)

    def await_event_sequence(self, events, history=None, start="NOW", timeout=5):
        """
        A search for a sequence of event messages. By default, the call will only await until
        the sequence is completed. The user can specify that await also searches the history by
        specifying a value for start. On timeout, the search will return the list of found
        event messages regardless of whether the sequence is complete.
        Note: It is reccomended (but not enforced) not to specify timestamps for this assert.
        Note: This function will always return a list of events the user should check if the
        sequence was completed.

        Args:
            events: an ordered list of event specifiers (mnemonic, id, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            an ordered list of EventData objects that satisfies the sequence
        """
        seq_preds = []
        for event in events:
            seq_preds.append(self.get_event_predicate(event))

        if history is None:
            history = self.get_event_test_history()

        msg = "Awaiting a sequence of {} event messages.".format(len(seq_preds))
        self.__log(msg, TestLogger.YELLOW)

        return self.find_history_sequence(seq_preds, history, start, timeout)

    def await_event_count(
        self, count, events=None, history=None, start="NOW", timeout=5
    ):
        """
        A search on the number of events received. By default, the call will only await until a
        correct count is achieved. The user can specify that this await also searches the current
        history by specifying a value for start. On timeout, the search will return the list of
        found event messages regardless of whether a correct count is achieved.
        Note: this search will always return a list of objects. The user should check if the search
        was completed.

        Args:
            count: either an exact amount (int) or a predicate to specify how many objects to find
            events: an event specifier or list of event specifiers (mnemonic, ID, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            a list of the EventData objects that were counted
        """
        if events is None:
            search = None
        elif isinstance(events, list):
            e_preds = []
            for event in events:
                e_preds.append(self.get_event_predicate(event=event))
            search = predicates.satisfies_any(e_preds)
        else:
            search = self.get_event_predicate(event=events)

        if history is None:
            history = self.get_event_test_history()

        msg = "Awaiting a count ({}) of event messages.".format(count)
        self.__log(msg, TestLogger.YELLOW)

        return self.find_history_count(count, history, search, start, timeout)

    ######################################################################################
    #   Event Asserts
    ######################################################################################
    def assert_event(
        self, event, args=None, severity=None, time_pred=None, history=None, start=None, timeout=0
    ):
        """
        An assert on a single event message received. If the history doesn't have the
        correct message, the call will await until a correct message is received or the
        timeout, at which point it will assert failure.

        Args:
            event: an event specifier (mnemonic, id, or predicate)
            args: a list of expected arguments (list of values, predicates, or None for don't care)
            severity: an EventSeverity enum or a predicate to specify the event severity
            time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the EventData object found during the search
        """
        pred = self.get_event_predicate(event, args, severity, time_pred)
        result = self.await_event(event, args, severity, time_pred, history, start, timeout)
        self.__assert_pred("Event Received", pred, result)
        return result

    def assert_event_sequence(self, events, history=None, start=None, timeout=0):
        """
        An assert that a sequence of event messages is received. If the history doesn't have the
        complete sequence, the call will await until the sequence is completed or the timeout, at
        which point it will assert failure.
        Note: It is reccomended (but not enforced) not to specify timestamps for this assert.

        Args:
            events: an ordered list of event specifiers (mnemonic, id, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            an ordered list of EventData objects that satisfied the sequence
        """
        results = self.await_event_sequence(events, history, start, timeout)
        len_pred = predicates.equal_to(len(events))
        self.__assert_pred("Event Sequence", len_pred, len(results))
        return results

    def assert_event_count(
        self, count, events=None, history=None, start=None, timeout=0
    ):
        """
        An assert on the number of events received. If the history doesn't have the
        correct event count, the call will await until a correct count is achieved or the
        timeout, at which point it will assert failure.

        Args:
            count: either an exact amount (int) or a predicate to specify how many objects to find
            events: optional event specifier or list of specifiers (mnemonic, id, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            a list of the EventData objects that were counted
        """
        results = self.await_event_count(count, events, history, start, timeout)
        if predicates.is_predicate(count):
            count_pred = count
        elif isinstance(count, int):
            count_pred = predicates.equal_to(count)
        self.__assert_pred("Event Count", count_pred, len(results))
        return results

    ######################################################################################
    #   History Searches
    ######################################################################################
    class TimeoutException(Exception):
        pass

    def __timeout_sig_handler(self, signum, frame):
        raise self.TimeoutException()

    def find_history_item(self, search_pred, history, start=None, timeout=0):
        """
        This function can both search and await for an element in a history. The function will
        return the first valid object it finds. The search will return when an object is found, or
        the timeout is reached.

        Args:
            search_pred: a predicate to specify a history item.
            history: the history that the function will search and await
            start: an index or predicate to specify the earliest item from the history to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the data object found during the search, otherwise, None
        """
        if start == self.NOW:
            start = history.size()

        current = history.retrieve(start)
        for item in current:
            if search_pred(item):
                self.__log("History search found the specified item: {}".format(item), TestLogger.YELLOW)
                return item

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self.__timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if search_pred(item):
                            signal.alarm(0)
                            self.__log("History search found the specified item: {}".format(item), TestLogger.YELLOW)
                            return item
                    time.sleep(0.1)
            except self.TimeoutException:
                self.__log("History search timed out")
        self.__log("History search failed to find the specified item", TestLogger.YELLOW)
        return None

    def find_history_sequence(self, seq_preds, history, start=None, timeout=0):
        """
        This function can both search and await for a sequence of elements in a history. The
        function will return a list of the history objects to satisfy the sequence search. The
        search will return when an order of data objects is found that satisfies the entire
        sequence, or the timeout occurs.
        Note: this search will always return a list of objects. The user should check if the search
        was completed.

        Args:
            seq_preds: an ordered list of predicate objects to specify a sequence
            history: the history that the function will search and await
            start: an index or predicate to specify the earliest item from the history to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            a list of data objects that satisfied the sequence
        """
        if start == self.NOW:
            start = history.size()

        current = history.retrieve(start)
        sequence = []
        seq_preds = seq_preds.copy()

        if len(seq_preds) == 0:
            return []

        for item in current:
            if seq_preds[0](item):
                self.__log("Sequence search found the next item: {}".format(item))
                sequence.append(item)
                seq_preds.pop(0)
                if len(seq_preds) == 0:
                    self.__log("Sequence search found the last item.", TestLogger.YELLOW)
                    return sequence

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self.__timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if seq_preds[0](item):
                            self.__log("Sequence search found the next item: {}".format(item))
                            sequence.append(item)
                            seq_preds.pop(0)
                            if len(seq_preds) == 0:
                                signal.alarm(0)
                                self.__log("Sequence search found the last item.", TestLogger.YELLOW)
                                return sequence
                    time.sleep(0.1)
            except self.TimeoutException:
                self.__log("Sequence search timed out")
        self.__log("Sequence search failed to find a complete sequence", TestLogger.YELLOW)
        return sequence

    def find_history_count(
        self, count, history, search_pred=None, start=None, timeout=0
    ):
        """
        This function can both search and await for a number of elements in a history. The function
        will return a list of the history objects to satisfy the search. The search will return
        when a correct count of data objects is found, or the timeout occurs.
        Note: this search will always return a list of objects. The user should check if the search
        was completed.

        Args:
            count: either an exact amount (int) or a predicate to specify how many objects to find
            history: the history that the function will search and await
            search_pred: a predicate to specify which items to count. If left blank, all will count
            start: an index or predicate to specify the earliest item from the history to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            a list of data objects that were counted during the search
        """
        if predicates.is_predicate(count):
            count_pred = count
        elif isinstance(count, int):
            count_pred = predicates.equal_to(count)
        else:
            raise TypeError("Find history must receive a predicate or an integer")

        if start == self.NOW:
            start = history.size()

        objects = []
        if search_pred is None:
            search_pred = predicates.always_true()
            objects = history.retrieve(start)
        else:
            current = history.retrieve(start)
            for item in current:
                if search_pred(item):
                    self.__log("Count search counted another item: {}".format(item))
                    objects.append(item)

        if count_pred(len(objects)):
            msg = "Count search found a correct number of items: {}".format(len(objects))
            self.__log(msg, TestLogger.YELLOW)
            return objects

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self.__timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if search_pred(item):
                            self.__log("Count search counted another item: {}".format(item))
                            objects.append(item)
                            if count_pred(len(objects)):
                                signal.alarm(0)
                                msg = "Count search found a correct number of items: {}".format(len(objects))
                                self.__log(msg, TestLogger.YELLOW)
                                return objects
                    time.sleep(0.1)
            except self.TimeoutException:
                self.__log("Count search timed out")
        msg = "Count search failed to find the correct number of objects"
        self.__log(msg, TestLogger.YELLOW)
        return objects

    ######################################################################################
    #   API Helper Methods
    ######################################################################################
    def __log(self, message, color=None, style=None, sender="Test API", case_id=None):
        if not isinstance(message, str):
            message = str(message)
        if self.logger is None:
            print("[{}] {}".format(sender, message))
        else:
            self.logger.log_message(message, sender, color, style, case_id)

    def __assert_pred(self, name, predicate, value):
        pred_msg = predicates.get_descriptive_string(value, predicate)
        if predicate(value):
            msg = name + " Assertion was successful.\nassert " + pred_msg
            self.__log(msg, TestLogger.GREEN)
            assert True, pred_msg
        else:
            msg = name + " Assertion failed!\nassert " + pred_msg
            self.__log(msg, TestLogger.RED)
            assert False, pred_msg

    def data_callback(self, data_object):
        """
        Data callback used by the api to subscribe to EVR's

        Args:
            data_object: object to store
        """
        if self.event_log_filter(data_object):
            msg = "GDS received EVR: {}".format(data_object.get_str(verbose=True))
            self.__log(msg, TestLogger.BLUE)

    def __history_search(history, name="generic history search", search_pred=None, start=None, timeout=0):
        ##########################################
        # Search-specific argument parsing (init)
        ##########################################

        if start == self.NOW:
            start = history.size()
        elif isinstance(start, TimeType):
            # TODO start = some predicate for timestamp
            pass

        current = history.retrieve(start)
        ###########################################
        # Search-specific search on current items
        ###########################################

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self.__timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        ###########################################
                        # Search-specific incremental search on future items
                        ###########################################
                    time.sleep(0.1)
            except self.TimeoutException:
                self.__log(name + " timed out and ended unsuccessfully.", TestLogger.YELLOW)
            finally:
                signal.alarm(0)
        ###########################################
        # Search-specific return
        ###########################################
