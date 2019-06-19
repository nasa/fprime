"""
gds_test_api.py:

This file contains basic asserts that can support integration tests on an FPrime
deployment. This API uses the standard pipeline to get access to commands, events,
telemetry and dictionaries.

:author: koran
"""


class IntegrationTestAPI:
    """
    Class used to collect basic assertions that would be used on a GDS Pipeline to carry
    out integration tests.
    :param pipeline: a pipeline object providing access to basic GDS functionality
    """

    def __init__(self, pipeline):
        self.pipeline = pipeline
        # these are owned by the GDS and will not be modified by the test API.
        self.aggregate_command_history = pipeline.get_command_history()
        self.aggregate_telemetry_history = pipeline.get_telemetry_history()
        self.aggregate_event_history = pipeline.get_event_history()

        # these histories are owned by the TestAPI and are modified by the API.
        # TODO implement test-case histories once, supported by gds helper.
        self.command_history = None
        self.telemetry_history = None
        self.event_history = None

    ######################################################################################
    #   History Functions
    ######################################################################################
    def get_command_test_history(self):
        """
        Accessor for IntegrationTestAPI's command history
        :return: a history of CmdData Objects
        """
        pass

    def get_telemetry_test_history(self):
        """
        Accessor for IntegrationTestAPI's telemetry history
        :return: a history of ChData Objects
        """
        pass

    def get_event_test_history(self):
        """
        Accessor for IntegrationTestAPI's event history
        :return: a history of EventData Objects
        """
        pass

    def get_latest_fsw_time(self):
        """
        Finds the latest flight software time received by either the event or telemetry
        history.
        :return: a flight software timestamp
        """
        pass

    def clear_histories(self, fsw_time_stamp=None):
        """
        Clears the IntegrationTestAPI's histories. Because the command history is not
        correlated to a flight software timestamp, it will be cleared entirely. This
        function can be used to set up test case so that the IntegrationTestAPI's
        histories only contain objects received during that test.Note: this will not
        clear user-created sub-histories nor the aggregate histories (histories owned
        by the gds)

        :param fsw_time_stamp: If specified, event and telemetry histories will be cleared up until the timestamp.
        """
        pass

    ######################################################################################
    #   Command Asserts and Functions
    ######################################################################################
    def assert_send_command(self, command, args):
        """
        Sends a command and asserts that the command was translated. If the command is in
        conflict with the flight dictionary, this will raise a test error. Note: This
        assert does not check that the command was  received by flight software, only that
        the command and arguments were valid with respect to the flight dictionary.

        :param command: Either the command id or a command mnemonic to define the type of command
        :param args: A list of command arguments to send
        :return: If the assert is successful, will return an instance of CmdData
        """
        pass

    def send_and_await_telemetry(self, command, args, channels, timeout=5):
        """
        Sends the specified command and awaits the specified telemetry update or sequence
        of updates. This function will enforce that each element of the sequence occurred
        in order with respect to the flight software timestamps. If the specification of
        timestamp predicates is not sequential, the timestamps will likely fail. Note: It
        is reccomended (but not enforced) not to specify timestamps for this assert.

        :param command: Either the command id or a command mnemonic to specify the type of command
        :param args: A list of command arguments to send
        :param channels: Either a single channel specifier, or a sequence of channel specifiers, where a channel specifier is an id, mnemonic, or a telemetry_predicate
        :param timeout: The maximum time to wait for
        :return: If the search is successful, will return the list of ChData objects to satisfy the search, otherwise will return None.
        """
        pass

    def send_and_await_event(self, command, args, events, timeout=5):
        """
        Sends the specified command and awaits the specified event message or sequence of
        messages. This function will enforce that each element of the sequence occurred
        in order with respect to the flight software timestamps. If the specification of
        timestamp predicates is not sequential, the timestamps will likely fail. Note: It
        is reccomended (but not enforced) not to specify timestamps for this assert.

        :param command: Either the command id or a command mnemonic to specify the type of command
        :param args: A list of command arguments to send
        :param events:  Either a single event specifier, or a sequence of event specifiers, where an event specifier is an id, mnemonic, or an event_predicate
        :param timeout: The maximum time to wait for
        :return: If the search is successful, will return the instance of EventData, otherwise will return None.
        """
        pass

    ######################################################################################
    #   Telemetry Functions
    ######################################################################################
    def translate_telemetry_name(self, channel):
        """
        This function will translate the given mnemonic into an ID as defined by the
        flight software dictionary. This call will raise an error if the channel given
        is not in the dictionary.

        :param channel: Either the channel id or the channel mnemonic
        :return: The channel ID
        """
        pass

    def get_telemetry_predicate(self, channel=None, val_pred=None, fsw_time_pred=None):
        """
        This function will translate the channel ID, and construct a telemetry_predicate
        object. It is used as a helper by the IntegrationTestAPI, but could also be
        helpful to a user of the test API. If channel is already an instance of
        telemetry_predicate, it will be returned immediately. The provided implementation
        of telemetry_predicate evaluates true when all specified constraints are
        satisfied. If a specific constraint isn't specified, then it will be ignored.

        :param channel: If specified, a channel id, channel mnemonic, or a predicate to call on the channel field to specify the telemetry channel
        :param val_pred: If specified, the telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, the telemetry update must have a  timestamp that satisfies this predicate.
        :return: an instance of telemetry_predicate
        """
        pass

    def await_telemetry(
        self, channel, val_pred=None, fsw_time_pred=None, history=None, timeout=5
    ):
        """
        This function will first search the history for a telemetry update that satisfies
        the specified constraints. Then, if no update was found, await_telemetry will
        wait for a valid telemetry update until the timeout.

        :param channel: A channel id, channel mnemonic, or a telemetry_predicate to specify the telemetry channel
        :param val_pred: If specified, the telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, the telemetry update must have a flight software timestamp that satisfies this predicate.
        :param history: If specified, will search and await the given history instead of the IntegrationTestAPI's history.
        :param timeout: The maximum time to wait for
        :return: If the search is successful, will return the instance of ChData from that satisfied the assert, otherwise will return None.
        """
        pass

    def await_telemetry_sequence(self, channels, history=None, timeout=5):
        """
        This function will first search the history then await future updates to find
        every specified channel in a sequence. This function will enforce that each
        element of the sequence occurred in order with respect to the flight software
        timestamps. Note: This function will return a list of what it finds regardless of
        whether or not it finds all elements.

        :param channels: A list of channel specifiers, where a channel specifier is a channel id, channel mnemonic, or a telemetry_predicate
        :param history: If specified, will search and await the given history instead of the IntegrationTestAPI's history
        :param timeout: The maximum time to wait for
        :return: will return the sequence of ChData objects to satisfy the search. If the sequence was only partially satisfied, then it will return that sub-list.
        """
        pass

    ######################################################################################
    #   Telemetry Asserts
    ######################################################################################
    def assert_receive_telemetry(
        self, channel, val_pred=None, fsw_time_pred=None, history=None, timeout=0
    ):
        """
        Asserts that a specified telemetry update was received. This function will first
        search the history then await future updates to find every specified channel in a
        sequence. If no valid update was received, then received before the timeout, this
        call will assert failure.

        :param channel: Either a channel id, channel mnemonic, or a telemetry_predicate to specify the telemetry channel.
        :param val_pred: If specified, a valid telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, a valid telemetry update must have a flight software timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the IntegrationTestAPI's history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, will return the instance of ChData that satisfied the assert.
        """
        pass

    def assert_telemetry_sequence(self, channels, history=None, timeout=0):
        """
        Asserts that a sequence of Telemetry updates was received. This function will
        first search the history then await future updates to find every specified
        channel update in a sequence. This function will enforce that each element of the
        sequence occurred in order with respect to the flight software timestamps. If the
        specification of timestamp predicates is not sequential, the timestamps will
        likely fail. Note: It is reccomended (but not enforced) not to specify timestamps
        for this assert.

        :param channels: A list of channel specifiers, where a channel specifier is a channel id, channel mnemonic, or a telemetry_predicate
        :param history: If specified, will search and await the given history instead of the IntegrationTestAPI's history
        :param timeout: The maximum time to wait for
        :return: If the assert is successful, will return the sequence of ChData objects to satisfy the search from data_types.ch_data
        """
        pass

    def assert_telemetry_count(
        self, count_pred, channels=None, history=None, timeout=0
    ):
        """
        An assert on the number of telemetry updates received. If the history doesn't
        have the correct update count, the call will await until a correct count is
        achieved or the timeout, at which point it will assert failure.

        :param count_pred: A predicate to determine whether the correct amount has been received.
        :param channels: A channel specifier or a list of channel specifiers, where a channel specifier is an id, mnemonic, or a telemetry_predicate. If specified, an update will only be counted if it satisfies at least one channel on this list.
        :param history: If specified, the assert will substitute the given history for the IntegrationTestAPI's history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, this call will return a list of the objects counted to satisfy the count predicate
        """
        pass

    ######################################################################################
    #   Event Functions
    ######################################################################################
    def translate_event_name(self, event):
        """
        This function will translate the given mnemonic into an ID as defined by the
        flight software dictionary. This call will raise an error if the event given is
        not in the dictionary.

        :param event: Either the event id or the event mnemonic
        :return: The event ID
        """
        pass

    def get_event_predicate(self, event=None, args=None, fsw_time_pred=None):
        """
        This function will translate the event ID, and construct an event_predicate
        object. It is used as a helper by the IntegrationTestAPI, but could also be
        helpful to a user of the test API. If event is already an instance of
        event_predicate, it will be returned immediately. The provided implementation of
        event_predicate evaluates true if and only if all specified constraints are
        satisfied. If a specific constraint isn't specified, then it will not effect the
        outcome. If no constraints are specified, the predicate will always return true.

        :param event: If specified, either the event id, an event mnemonic, or an event_predicate to specify the event
        :param args: If specified, a valid event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, the event must have a timestamp that satisfies this predicate.
        :return: an instance of event_predicate
        """
        pass

    def await_event(
        self, event, args=None, fsw_time_pred=None, history=None, timeout=5
    ):
        """
        This function will first search the history for an event that satisfies the
        specified constraints. Then, if no message was found, await_event will wait for a
        valid event message until the timeout.

        :param event: Either the event id, an event mnemonic, or an event_predicate to specify the event type
        :param args: If specified, the event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, the telemetry update must have a flight software timestamp that satisfies this predicate.
        :param history: If specified, will search and await the given history instead of the IntegrationTestAPI's history.
        :param timeout: The maximum time to wait for
        :return: If the search is successful, will return the instance of EventData that satisfied the assert, otherwise will return None.
        """
        pass

    def await_event_sequence(self, events, history=None, timeout=5):
        """
        This function will first search the history then await future messages to find
        every specified event in a sequence. This function will enforce that each element
        of the sequence occurred in order with respect to the flight software timestamps.
        Note: This function will return a list of events regardless of whether or not it
        finds all elements.

        :param events: A list of event specifiers, where an event specifier is an id, mnemonic, or an event_predicate
        :param history: If specified, will search and await the given history instead of the IntegrationTestAPI's history
        :param timeout: The maximum time to wait for
        :return: will return the sequence of EventData objects to satisfy the search. If the sequence was only partially satisfied, then it will return that sub-list.
        """
        pass

    ######################################################################################
    #   Event Asserts
    ######################################################################################
    def assert_receive_event(
        self, event, args=None, fsw_time_pred=None, history=None, timeout=0
    ):
        """
        Asserts that a specified event was received. This function will search the
        current history, and, if necessary, wait for an message that satisfies the
        specified constraints. If no valid event was received before the timeout,
        this call will assert failure.

        :param event: Either the event id, an event mnemonic, or an event_predicate to specify the event type
        :param args: If specified, a valid event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, a valid event must have a flight software timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the IntegrationTestAPI's history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, this call will return the instance of EventData that satisfied the assert.
        """
        pass

    def assert_event_sequence(self, events=None, history=None, timeout=0):
        """
        Asserts that a sequence of event messages was received. This function will
        first search the history then await future messages to find every specified
        event in a sequence. This function will enforce that each element of the sequence
        occurred in order with respect to the flight software timestamps. If the
        specification of timestamp predicates is not sequential, the timestamps will
        likely fail. Note: It is reccomended (but not enforced) not to specify timestamps
        for this assert.

        :param events: A list of event specifiers, where a channel specifier is an id, mnemonic, or an event_predicate
        :param history: If specified, will search and await the given history instead of the IntegrationTestAPI's history
        :param timeout: The maximum time to wait for
        :return: If the assert is successful, will return the sequence of ChData objects to satisfy the search from data_types.ch_data
        """
        pass

    def assert_event_count(self, count_pred, events=None, history=None, timeout=0):
        """
        An assert on the number of events received. If the history doesn't have the
        correct event count, the call will await until a correct count is achieved or the
        timeout at which point it will assert failure.

        :param count_pred: A predicate to determine whether the correct amount has been received.
        :param events: An event specifier or a list of event specifiers. Where a specifier is an id, mnemonic or telemetry_predicate. If specified, an event will only be counted if it satisfies at least one specifier on this list.
        :param history: If specified, the assert will substitute the given history for the IntegrationTestAPI's history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, this call will return the number that satisfied the count predicate.
        """
        pass
