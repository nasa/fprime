"""
gds_test_api.py:

This file contains basic asserts that can support integration tests on an FPrime
deployment. This API uses the standard pipeline to get access to commands, events,
telemetry and dictionaries.

:author: koran
"""
import signal
import time

from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.handlers import DataHandler
from fprime_gds.common.history.chrono import ChronologicalHistory
from fprime_gds.common.history.test import TestHistory
from fprime_gds.common.logger.test_logger import TestLogger
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity


class IntegrationTestAPI(DataHandler):
    """
    A value used to begin searches after the current contents in a history and only search future
    items
    """

    NOW = "NOW"

    def __init__(self, pipeline, logpath=None, fsw_order=True):
        """
        Initializes API: constructs and registers test histories.
        Args:
            pipeline: a pipeline object providing access to basic GDS functionality
            logpath: an optional output destination for the api test log
            fsw_order: a flag to determine whether the API histories will maintain FSW time order.
        """
        self.pipeline = pipeline
        # these are owned by the GDS and will not be modified by the test API.
        self.aggregate_command_history = pipeline.histories.commands
        self.aggregate_telemetry_history = pipeline.histories.channels
        self.aggregate_event_history = pipeline.histories.events

        # these histories are owned by the TestAPI and are modified by the API.
        self.fsw_ordered = fsw_order
        if fsw_order:
            self.command_history = ChronologicalHistory()
            self.telemetry_history = ChronologicalHistory()
            self.event_history = ChronologicalHistory()
        else:
            self.command_history = TestHistory()
            self.telemetry_history = TestHistory()
            self.event_history = TestHistory()
        self.pipeline.coders.register_command_consumer(self.command_history)
        self.pipeline.coders.register_event_consumer(self.event_history)
        self.pipeline.coders.register_channel_consumer(self.telemetry_history)

        # Initialize latest time. Will be updated whenever a time query is made.
        self.latest_time = TimeType()

        # Initialize the logger
        if logpath is not None:
            self.logger = TestLogger(logpath)
        else:
            self.logger = None

        # A predicate used as a filter to choose which events to log automatically
        self.event_log_filter = self.get_event_pred()
        self.pipeline.coders.register_event_consumer(self)

        # Used by the data_callback method to detect if events have been received out of order.
        self.last_evr = None

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
        self.get_latest_time()  # called in case aggregate histories are cleared by the user
        self.clear_histories()

    def log(self, msg, color=None):
        """
        User-accessible function to log user messages to the test log.
        Args:
            msg: a user-provided message to add to the test log. (str)
            color: a string containing a color hex code "######" (str)
        """
        self.__log(msg, color, sender="API user")

    def get_latest_time(self):
        """
        Finds the latest flight software time received by either history.

        Returns:
            a flight software timestamp (TimeType)
        """
        events = self.aggregate_event_history.retrieve()
        e_time = TimeType()
        if len(events) > 0:
            e_time = events[-1].get_time()

        channels = self.aggregate_telemetry_history.retrieve()
        t_time = TimeType()
        if len(channels) > 0:
            t_time = channels[-1].get_time()

        self.latest_time = max(e_time, t_time, self.latest_time)
        return self.latest_time

    def test_assert(self, value, msg="", expect=False):
        """
        this assert gives the user the ability to add formatted assert messages to the test log and
        raise an assertion.
        Args:
            value: a boolean value that determines if the assert is successful.
            msg: a string describing what is checked by the assert.
            expect: when True, the call will behave as an expect, and will skip the assert (boolean)
        Return:
            True if the assert was successful, False otherwise
        """
        if not expect:
            ast_msg = "User assertion"
            fail_color = TestLogger.RED
        else:
            ast_msg = "User expectation"
            fail_color = TestLogger.ORANGE

        if value:
            ast_msg = ast_msg + " succeeded: " + msg
            self.__log(ast_msg, TestLogger.GREEN)
        else:
            ast_msg = ast_msg + " failed: " + msg
            self.__log(ast_msg, fail_color)

        if not expect:
            assert value, ast_msg
        return value

    def predicate_assert(self, predicate, value, msg="", expect=False):
        """
        API assert gives the user the ability to add formatted assert messages to the test log and
        raise an assertion.
        Args:
            value: the value to be evaluated by the predicate. (object)
            predicate: an instance of predicate that will decided if the test passes (predicate)
            msg: a string describing what is checked by the assert. (str)
            expect: when True, the call will behave as an expect, and will skip the assert (boolean)
        Return:
            True if the assert was successful, False otherwise
        """
        return self.__assert_pred("User", predicate, value, msg, expect)

    def clear_histories(self, time_stamp=None):
        """
        Clears the IntegrationTestAPI's histories. Because the command history is not correlated to
        a flight software timestamp, it will be cleared entirely. This function can be used to set
        up test cases so that the IntegrationTestAPI's histories only contain objects received
        during that test.
        Note: this will not clear user-created sub-histories nor the aggregate histories (histories
        owned by the GDS)

        Args:
            time_stamp: If specified, histories are only cleared before the timestamp.
        """
        if time_stamp is not None:
            time_pred = predicates.greater_than_or_equal_to(time_stamp)
            e_pred = predicates.event_predicate(time_pred=time_pred)
            self.event_history.clear(e_pred)
            t_pred = predicates.telemetry_predicate(time_pred=time_pred)
            self.telemetry_history.clear(t_pred)
            msg = "Clearing Test Histories after {}".format(time_stamp)
            self.__log(msg, TestLogger.WHITE)
        else:
            self.event_history.clear()
            self.telemetry_history.clear()
            msg = "Clearing Test Histories"
            self.__log(msg, TestLogger.WHITE)

        self.command_history.clear()

    def set_event_log_filter(
        self, event=None, args=None, severity=None, time_pred=None
    ):
        """
        Constructs an event predicate that is then used to filter which events are interlaced in the
        test logs. This method replaces the current filter. Calling this method with no arguments
        will effectively reset the filter.

        Args:
            event: an optional mnemonic (str), id (int), or predicate to specify the event type
            args: an optional list of arguments (list of values, predicates, or None to ignore)
            severity: an EventSeverity enum or a predicate to specify the event severity
            time_pred: an optional predicate to specify the flight software timestamp
        """
        self.event_log_filter = self.get_event_pred(event, args, severity, time_pred)

    ######################################################################################
    #   History Functions
    ######################################################################################
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

    def get_telemetry_subhistory(self, telemetry_filter=None, fsw_order=True):
        """
        Returns a new instance of TestHistory that will be updated with new telemetry updates as
        they come in. Specifying a filter will only enqueue updates that satisfy the filter in this
        new sub-history. The returned history can be substituted into the await and assert methods
        of this API.

        Args:
            telemetry_filter: an optional predicate used to filter a subhistory.
            fsw_order: a flag to determine whether this subhistory will maintain FSW time order.
        Returns:
            an instance of TestHistory
        """
        if fsw_order:
            subhist = ChronologicalHistory(telemetry_filter)
        else:
            subhist = TestHistory(telemetry_filter)
        self.pipeline.coders.register_channel_consumer(subhist)
        return subhist

    def remove_telemetry_subhistory(self, subhist):
        """
        De-registers the subhistory from the GDS. Once called, the given subhistory will stop
        receiving telemetry updates.

        Args:
            subhist: a TestHistory instance that is subscribed to event messages
        Returns:
            True if the subhistory was removed, False otherwise
        """
        return self.pipeline.coders.remove_channel_consumer(subhist)

    def get_event_subhistory(self, event_filter=None, fsw_order=True):
        """
        Returns a new instance of TestHistory that will be updated with new events as they come in.
        Specifying a filter will only enqueue events that satisfy the filter in this new
        sub-history. The returned history can be substituted into the await and assert methods of
        this API.

        Args:
            event_filter: an optional predicate to filter a subhistory.
            fsw_order: a flag to determine whether this subhistory will maintain FSW time order.
        Returns:
            an instance of TestHistory
        """
        if fsw_order:
            subhist = ChronologicalHistory(event_filter)
        else:
            subhist = TestHistory(event_filter)
        self.pipeline.coders.register_event_consumer(subhist)
        return subhist

    def remove_event_subhistory(self, subhist):
        """
        De-registers the subhistory from the GDS. Once called, the given subhistory will stop
        receiving event messages.

        Args:
            subhist: a TestHistory instance that is subscribed to event messages
        Returns:
            True if the subhistory was removed, False otherwise
        """
        return self.pipeline.coders.remove_event_consumer(subhist)

    ######################################################################################
    #   Command Functions
    ######################################################################################
    def translate_command_name(self, command):
        """
        This function will translate the given mnemonic into an ID as defined by the flight
        software dictionary. This call will raise an error if the command given is not in the
        dictionary.

        Args:
            command: Either the command id (int) or the command mnemonic (str)

        Returns:
            The command ID (int)
        """
        if isinstance(command, str):
            cmd_dict = self.pipeline.dictionaries.command_name
            if command in cmd_dict:
                return cmd_dict[command].get_id()
            else:
                msg = "The command mnemonic, {}, wasn't in the dictionary".format(
                    command
                )
                raise KeyError(msg)
        else:
            cmd_dict = self.pipeline.dictionaries.command_id
            if command in cmd_dict:
                return command
            else:
                msg = "The command id, {}, wasn't in the dictionary".format(command)
                raise KeyError(msg)

    def send_command(self, command, args=None):
        """
        Sends the specified command.
        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
        """
        if args is None:
            args = []

        msg = "Sending Command: {} {}".format(command, args)
        self.__log(msg, TestLogger.PURPLE)
        command = self.translate_command_name(command)
        self.pipeline.send_command(command, args)

    def send_and_await_telemetry(self, command, args=None, channels=None, timeout=5):
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
        if args is None:
            args = []
        if channels is None:
            channels = []

        start = self.telemetry_history.size()
        self.send_command(command, args)
        if isinstance(channels, list):
            return self.await_telemetry_sequence(channels, start=start, timeout=timeout)
        else:
            return self.await_telemetry(channels, start=start, timeout=timeout)

    def send_and_await_event(self, command, args=None, events=None, timeout=5):
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
        if args is None:
            args = []
        if events is None:
            events = []

        start = self.event_history.size()
        self.send_command(command, args)
        if isinstance(events, list):
            return self.await_event_sequence(events, start=start, timeout=timeout)
        else:
            return self.await_event(events, start=start, timeout=timeout)

    ######################################################################################
    #   Command Asserts
    ######################################################################################
    def send_and_assert_telemetry(self, command, args=None, channels=None, timeout=5):
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
        if args is None:
            args = []
        if channels is None:
            channels = []

        start = self.telemetry_history.size()
        self.send_command(command, args)
        if isinstance(channels, list):
            return self.assert_telemetry_sequence(
                channels, start=start, timeout=timeout
            )
        else:
            return self.assert_telemetry(channels, start=start, timeout=timeout)

    def send_and_assert_event(self, command, args=None, events=None, timeout=5):
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
        if args is None:
            args = []
        if events is None:
            events = []

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
            ch_dict = self.pipeline.dictionaries.channel_name
            if channel in ch_dict:
                return ch_dict[channel].get_id()
            else:
                msg = "The telemetry mnemonic, {}, wasn't in the dictionary".format(
                    channel
                )
                raise KeyError(msg)
        else:
            ch_dict = self.pipeline.dictionaries.channel_id
            if channel in ch_dict:
                return channel
            else:
                msg = "The telemetry mnemonic, {}, wasn't in the dictionary".format(
                    channel
                )
                raise KeyError(msg)

    def get_telemetry_pred(self, channel=None, value=None, time_pred=None):
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
        t_pred = self.get_telemetry_pred(channel, value, time_pred)

        if history is None:
            history = self.get_telemetry_test_history()

        return self.find_history_item(t_pred, history, start, timeout)

    def await_telemetry_sequence(self, channels, history=None, start="NOW", timeout=5):
        """
        A search for a sequence of telemetry updates. By default, the call will only await until
        the sequence is completed. The user can specify that await also searches the history by
        specifying a value for start. On timeout, the search will return the list of found
        channel updates regardless of whether the sequence is complete.
        Note: It is recommended (but not enforced) not to specify timestamps for this assert.
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
            seq_preds.append(self.get_telemetry_pred(channel))

        if history is None:
            history = self.get_telemetry_test_history()

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
                t_preds.append(self.get_telemetry_pred(channel=channel))
            search = predicates.satisfies_any(t_preds)
        else:
            search = self.get_telemetry_pred(channel=channels)

        if history is None:
            history = self.get_telemetry_test_history()

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
        pred = self.get_telemetry_pred(channel, value, time_pred)
        result = self.await_telemetry(
            channel, value, time_pred, history, start, timeout
        )
        msg = "checks if item search found a correct update"
        self.__assert_pred("Telemetry received", pred, result, msg)
        return result

    def assert_telemetry_sequence(self, channels, history=None, start=None, timeout=0):
        """
        A search for a sing sequence of telemetry updates messages. If the history doesn't have the
        complete sequence, the call will await until the sequence is completed or the timeout, at
        which point it will return the list of found channel updates.
        Note: It is recommended (but not enforced) not to specify timestamps for this assert.
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
        msg = "checks if sequence search found every update"
        self.__assert_pred("Telemetry sequence", len_pred, len(results), msg)
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
        count_pred = (
            count if predicates.is_predicate(count) else predicates.equal_to(count)
        )
        msg = "checks if count search found a correct amount of updates"
        self.__assert_pred("Telemetry count", count_pred, len(results), msg)
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
            event_dict = self.pipeline.dictionaries.event_name
            if event in event_dict:
                return event_dict[event].get_id()
            else:
                msg = "The event mnemonic, {}, wasn't in the dictionary".format(event)
                raise KeyError(msg)
        else:
            event_dict = self.pipeline.dictionaries.event_id
            if event in event_dict:
                return event
            else:
                msg = "The event id, {}, wasn't in the dictionary".format(event)
                raise KeyError(msg)

    def get_event_pred(self, event=None, args=None, severity=None, time_pred=None):
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
                msg = "Given severity was not a valid Severity Enum Value: {} ({})".format(
                    severity, type(severity)
                )
                raise TypeError(msg)
            severity = predicates.equal_to(severity)

        return predicates.event_predicate(event, args, severity, time_pred)

    def await_event(
        self,
        event,
        args=None,
        severity=None,
        time_pred=None,
        history=None,
        start="NOW",
        timeout=5,
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
        e_pred = self.get_event_pred(event, args, severity, time_pred)

        if history is None:
            history = self.get_event_test_history()

        return self.find_history_item(e_pred, history, start, timeout)

    def await_event_sequence(self, events, history=None, start="NOW", timeout=5):
        """
        A search for a sequence of event messages. By default, the call will only await until
        the sequence is completed. The user can specify that await also searches the history by
        specifying a value for start. On timeout, the search will return the list of found
        event messages regardless of whether the sequence is complete.
        Note: It is recommended (but not enforced) not to specify timestamps for this assert.
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
            seq_preds.append(self.get_event_pred(event))

        if history is None:
            history = self.get_event_test_history()

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
                e_preds.append(self.get_event_pred(event=event))
            search = predicates.satisfies_any(e_preds)
        else:
            search = self.get_event_pred(event=events)

        if history is None:
            history = self.get_event_test_history()

        return self.find_history_count(count, history, search, start, timeout)

    ######################################################################################
    #   Event Asserts
    ######################################################################################
    def assert_event(
        self,
        event,
        args=None,
        severity=None,
        time_pred=None,
        history=None,
        start=None,
        timeout=0,
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
        pred = self.get_event_pred(event, args, severity, time_pred)
        result = self.await_event(
            event, args, severity, time_pred, history, start, timeout
        )
        msg = "checks if item search found a correct event"
        self.__assert_pred("Event received", pred, result, msg)
        return result

    def assert_event_sequence(self, events, history=None, start=None, timeout=0):
        """
        An assert that a sequence of event messages is received. If the history doesn't have the
        complete sequence, the call will await until the sequence is completed or the timeout, at
        which point it will assert failure.
        Note: It is recommended (but not enforced) not to specify timestamps for this assert.

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
        msg = "checks if sequence search found every event"
        self.__assert_pred("Event sequence", len_pred, len(results), msg)
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
        count_pred = (
            count if predicates.is_predicate(count) else predicates.equal_to(count)
        )
        msg = "checks if count search found a correct amount of events"
        self.__assert_pred("Event count", count_pred, len(results), msg)
        return results

    ######################################################################################
    #   History Searches
    ######################################################################################
    class __HistorySearcher:
        """
        This class defines the calls made by the __history_search helper method and has a unique
        implementation for each type of search provided by the api.
        """

        def __init__(self):
            self.ret_val = None
            self.repeats = False

        def search_current_history(self, items):
            """
            Searches the scoped existing history
            Return:
                True if the search was satisfied, False otherwise
            """
            raise NotImplementedError()

        def incremental_search(self, item):
            """
            Searches one awaited item at a time
            Return:
                True if the search was satisfied, False otherwise
            """
            raise NotImplementedError()

        def get_return_value(self):
            """
            Returns the result of the search whether the search is successful or not
            """
            return self.ret_val

        def requires_repeats(self):
            """
            Returns a flag to determine if the history searcher needs repeated data objects when receive order does not match chronological order.
            """
            return self.repeats

    class TimeoutException(Exception):
        """
        This exception is used by the history searches to signal the end of the timeout.
        """

    def __timeout_sig_handler(self, signum, frame):
        raise self.TimeoutException()

    def __search_test_history(self, searcher, name, history, start=None, timeout=0):
        """
        This helper method contains the common logic to all search methods in the test API. This
        means searches on both the event and channel histories rely on this helper. Each history
        search is performed on both current history items and then on items that have yet to be
        added to the history. The API defines these two scopes using the variables start and
        timeout. They have several useful behaviors.

        start is used to pick the earliest item to search in the current history. start can be
        specified as either a predicate to search for the first item, an index of the history, the
        API variable NOW, or an instance of the TimeType timestamp object. The behavior of NOW is
        to ignore the current history and only search awaited items until the timestamp. The
        behavior of giving a timetype is to only search items that happened at or after the
        specified timestamp.

        timeout is a specification of how long to await future items in seconds. Specifying a
        timeout of 0 will ignore all future items. The timeout specifies an increment of time
        relative to the local clock, not the embedded application's clock.
        Note: the API does not try to check for edge cases where the final item in a search is
        received as the search times out. The user should ensure that their timeouts are sufficient
        to complete any awaiting searches.

        Finally, the test API supports the ability to substitute a history object for any search.
        This is part of why history must be specified for each search

        Args:
            searcher: an instance of __HistorySearcher to execute search-specific logic
            name: a string name to differentiate the type of search
            history: the TestHistory object to conduct the search on
            start: an index, a predicate, the NOW variable, or a TimeType timestamp to pick the
                first item to search
            timeout: the number of seconds to await future items
        """
        if start == self.NOW:
            start = history.size()
        elif isinstance(start, TimeType):
            time_pred = predicates.greater_than_or_equal_to(start)
            e_pred = self.get_telemetry_pred(time_pred=time_pred)
            t_pred = self.get_event_pred(time_pred=time_pred)
            start = predicates.satisfies_any([e_pred, t_pred])

        current = history.retrieve(start)
        if searcher.search_current_history(current):
            return searcher.get_return_value()

        if timeout:
            self.__log(name + " now awaiting for at most {} s.".format(timeout))
            check_repeats = isinstance(history, ChronologicalHistory)
            try:
                signal.signal(signal.SIGALRM, self.__timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    if check_repeats:
                        new_items = history.retrieve_new(searcher.requires_repeats())
                    else:
                        new_items = history.retrieve_new()
                    for item in new_items:
                        if searcher.incremental_search(item):
                            return searcher.get_return_value()
                    time.sleep(0.1)
            except self.TimeoutException:
                self.__log(
                    name + " timed out and ended unsuccessfully.", TestLogger.YELLOW
                )
            finally:
                signal.alarm(0)
        else:
            self.__log(name + " ended unsuccessfully.", TestLogger.YELLOW)
        return searcher.get_return_value()

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

        class __ItemSearcher(self.__HistorySearcher):
            def __init__(self, log, search_pred):
                super().__init__()
                self.log = log
                self.search_pred = search_pred
                self.repeats = False
                self.ret_val = None
                msg = "Beginning an item search for an item that satisfies:\n    {}".format(
                    self.search_pred
                )
                self.log(msg, TestLogger.YELLOW)

            def search_current_history(self, items):
                for item in items:
                    if self.incremental_search(item):
                        return True
                return False

            def incremental_search(self, item):
                if self.search_pred(item):
                    msg = "History search found the specified item: {}".format(item)
                    self.log(msg, TestLogger.YELLOW)
                    self.ret_val = item
                    return True
                return False

        searcher = __ItemSearcher(self.__log, search_pred)
        return self.__search_test_history(
            searcher, "Item search", history, start, timeout
        )

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

        class __SequenceSearcher(self.__HistorySearcher):
            def __init__(self, log, seq_preds):
                super().__init__()
                self.log = log
                self.ret_val = []
                self.seq_preds = seq_preds.copy()
                self.repeats = True
                msg = "Beginning a sequence search of {} items.".format(
                    len(self.seq_preds)
                )
                self.log(msg, TestLogger.YELLOW)

            def search_current_history(self, items):
                if len(self.seq_preds) == 0:
                    msg = "Sequence search finished, as the specified sequence had 0 items."
                    self.log(msg, TestLogger.YELLOW)
                    return True

                for item in items:
                    if self.incremental_search(item):
                        return True
                return False

            def incremental_search(self, item):
                if self.seq_preds[0](item):
                    self.log("Sequence search found the next item: {}".format(item))
                    self.ret_val.append(item)
                    self.seq_preds.pop(0)
                    if len(self.seq_preds) == 0:
                        self.log(
                            "Sequence search found the last item.", TestLogger.YELLOW
                        )
                        return True
                return False

        searcher = __SequenceSearcher(self.__log, seq_preds)
        return self.__search_test_history(
            searcher, "Sequence search", history, start, timeout
        )

    def find_history_count(
        self, count, history, search_pred=None, start=None, timeout=0
    ):
        """
        This function first counts all valid items in the current history, then can await until a
        valid number of elements is received by the history. The function will return a list of the
        history objects counted by the search. The search will return when a correct count of data
        objects is found, or the timeout occurs.
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

        class __CountSearcher(self.__HistorySearcher):
            def __init__(self, log, count, search_pred):
                super().__init__()
                self.log = log
                self.ret_val = []
                if predicates.is_predicate(count):
                    self.count_pred = count
                else:
                    self.count_pred = predicates.equal_to(count)
                self.search_pred = search_pred
                self.repeats = False

                msg = "Beginning a count search for an amount of items ({}).".format(
                    self.count_pred
                )
                self.log(msg, TestLogger.YELLOW)

            def search_current_history(self, items):
                if self.search_pred is None:
                    self.search_pred = predicates.always_true()
                    self.ret_val = items
                else:
                    for item in items:
                        if search_pred(item):
                            self.log(
                                "Count search counted another item: {}".format(item)
                            )
                            self.ret_val.append(item)

                if self.count_pred(len(self.ret_val)):
                    msg = "Count search found a correct amount: {}".format(
                        len(self.ret_val)
                    )
                    self.log(msg, TestLogger.YELLOW)
                    return True
                return False

            def incremental_search(self, item):
                if self.search_pred(item):
                    self.log("Count search counted another item: {}".format(item))
                    self.ret_val.append(item)
                    if self.count_pred(len(self.ret_val)):
                        msg = "Count search found a correct amount: {}".format(
                            len(self.ret_val)
                        )
                        self.log(msg, TestLogger.YELLOW)
                        return True
                return False

        searcher = __CountSearcher(self.__log, count, search_pred)
        return self.__search_test_history(
            searcher, "Count search", history, start, timeout
        )

    ######################################################################################
    #   API Helper Methods
    ######################################################################################
    def __log(self, message, color=None, style=None, sender="Test API", case_id=None):
        """
        Logs and prints an API Message. If the API isn't using a Logger, then only a message will
        be printed.
        Args:
            message: a string containing the message to log
            color: a color hex code (str)
            style: a style option from test logger (str) ["BOLD", "ITALICS", "UNDERLINED"]
            sender: a marker for where the message originated
            case_id: a tag for the current test case. Only needs to be set when a new case starts
        """
        if not isinstance(message, str):
            message = str(message)
        if self.logger:
            self.logger.log_message(message, sender, color, style, case_id)
        # TODO: Re-add way of printing to console?

    def __assert_pred(self, name, predicate, value, msg="", expect=False):
        """
        Helper to assert that a value satisfies a predicate. Includes arguments to provide more
        descriptive messages in the logs.
        Args:
            name: short name describing the check
            predicate: an instance of predicate to determine if the assert is successful
            value: the object evaluated by the predicate
            msg: a string message to describe what the assert is checking
            expect: a boolean value: True will be have as an expect and not raise an assertion.
        Returns:
            True if the assertion was successful, False otherwise
        """
        name = name + (" expectation" if expect else " assertion")
        pred_msg = predicates.get_descriptive_string(value, predicate)
        if predicate(value):
            ast_msg = name + " succeeded: {}\nassert ".format(msg) + pred_msg
            self.__log(ast_msg, TestLogger.GREEN)
            if not expect:
                assert True, pred_msg
            return True
        else:
            ast_msg = name + " failed: {}\nassert ".format(msg) + pred_msg
            if not expect:
                self.__log(ast_msg, TestLogger.RED)
                assert False, pred_msg
            else:
                self.__log(ast_msg, TestLogger.ORANGE)
            return False

    def data_callback(self, data, sender=None):
        """
        Data callback used by the api to log events and detect when they are received out of order.
        Args:
            data: object to store
        """
        if self.event_log_filter(data):
            msg = "Received EVR: {}".format(data.get_str(verbose=True))
            self.__log(msg, TestLogger.BLUE, sender="GDS")
        if self.last_evr is not None and data.get_time() < self.last_evr.get_time():
            msg = "API detected out of order evrs!"
            msg = msg + "\nReceived First:{}".format(
                self.last_evr.get_str(verbose=True)
            )
            msg = msg + "\nReceived Second:{}".format(data.get_str(verbose=True))
            self.__log(msg, TestLogger.ORANGE)
        self.last_evr = data
