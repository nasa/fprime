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
        self.aggregate_telemetry_history = pipeline.get_channel_history()
        self.aggregate_event_history = pipeline.get_event_history()

        # these histories are owned by the TestAPI and are modified by the API.
        self.command_history = TestHistory()
        self.telemetry_history = TestHistory()
        self.pipeline.register_telemetry_history(self.telemetry_history)
        self.event_history = TestHistory()
        self.pipeline.register_event_history(self.event_history)

    def log_test_message(self, msg):
        """
        TODO: Define what a test log should look like and describe its parameters.
        """
        pass

    def start_test_case(self, name):
        """
        TODO: Define what should happen at the start of each test case and what parameters
              would be included.
        """
        self.log_test_message("Starting new test case: {}".format(name))
        self.clear_histories()

    def get_latest_fsw_time(self):
        """
        Finds the latest flight software time received by either the event or telemetry
        history.
        :return: a flight software timestamp
        """
        e_time = self.event_history[-1].get_time()
        t_time = self.telemetry_history[-1].get_time()
        return max(e_time, t_time)

    def clear_histories(self, fsw_time_stamp=None):
        """
        Clears the IntegrationTestAPI's histories. Because the command history is not
        correlated to a flight software timestamp, it will be cleared entirely. This
        function can be used to set up test cases so that the IntegrationTestAPI's
        histories only contain objects received during that test.
        Note: this will not clear user-created sub-histories nor the aggregate histories
        (histories owned by the gds)

        :param fsw_time_stamp: If specified, event and telemetry histories will be
            cleared up until the timestamp.
        """
        fsw_pred = predicates.greater_than(fsw_time_stamp)
        e_pred = predicates.event_predicate(fsw_time_pred=fsw_pred)
        self.event_history.clear(e_pred)
        t_pred = predicates.telemetry_predicate(fsw_time_pred=fsw_pred)
        self.command_history.clear(t_pred)
        self.command_history.clear()

    ######################################################################################
    #   Command Functions
    ######################################################################################
    def translate_command_name(self, command):
        """
        This function will translate the given mnemonic into an ID as defined by the
        flight software dictionary. This call will raise an error if the command given
        is not in the dictionary.

        :param channel: Either the channel id or the channel mnemonic
        :return: The comand ID
        """
        cmd_dict = self.pipeline.get_command_dictionary()
        # TODO investigate dictionary structure/implementation to make sure we get the
        # correct version of the dictionary.

    def send_command(self, command, args):
        """
        Sends the specified command.
        :param command:
        """
        command = self.translate_command_name(command)
        self.pipeline.send_command(command, args)

    def send_and_await_telemetry(self, command, args, channels, start=None, timeout=5):
        """
        Sends the specified command and awaits the specified telemetry update or sequence of updates. This
        function will enforce that each element of the sequence occurred in order with respect to the flight
        software timestamps. If the specification of timestamp predicates is not sequential, the timestamps
        will likely fail.
        Note: It is reccomended (but not enforced) not to specify timestamps for this assert.
        """
        self.send_command(command, args)
        if isinstance(channels, list):
            self.await_telemetry_sequence(channels, start=start, timeout=timeout)
        else:
            self.await_telemetry(channels, start=start, timeout=timeout)

    def send_and_await_event(self, command, args, events, start=None, timeout=5):
        """
        Sends the specified command and awaits the specified event message or sequence of
        messages. This function will enforce that each element of the sequence occurred
        in order with respect to the flight software timestamps. If the specification of
        timestamp predicates is not sequential, the timestamps will likely fail. Note: It
        is reccomended (but not enforced) not to specify timestamps for this assert.
        """
        self.send_command(command, args)
        if isinstance(events, list):
            self.await_event_sequence(events, start=start, timeout=timeout)
        else:
            self.await_event(events, start=start, timeout=timeout)

    ######################################################################################
    #   Command Asserts
    ######################################################################################
    def assert_send_command(self, command, args):
        """
        Sends a command and asserts that the command was translated. If the command is in
        conflict with the flight dictionary, this will raise a test error. Note: This
        assert does not check that the command was  received by flight software, only that
        the command and arguments were valid with respect to the flight dictionary.

        :param command: Either the command id or a command mnemonic to define the type of
            command
        :param args: A list of command arguments to send
        :return: If the assert is successful, will return an instance of CmdData
        """
        try:
            command = self.translate_command_name(command)
            # TODO: catch the key error and assert failure.
            self.pipeline.send_command(command, args)
        except KeyError:
            # TODO: Print readable test log messages describing the input that caused the
            # error
            assert False
        assert True

    def send_and_assert_telemetry(self, command, args, channels, start=None, timeout=5):
        """
        TODO: Define what a send and assert should look like and describe its parameters.
        """
        self.send_command(command, args)
        if isinstance(channels, list):
            self.assert_telemetry_sequence(channels, start=start, timeout=timeout)
        else:
            self.assert_telemetry(channels, start=start, timeout=timeout)

    def send_and_assert_event(self, command, args, events, start=None, timeout=5):
        """
        TODO: Define what a send and assert should look like and describe its parameters.
        """
        self.send_command(command, args)
        if isinstance(events, list):
            self.assert_event_sequence(events, start=start, timeout=timeout)
        else:
            self.assert_event(events, start=start, timeout=timeout)

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
        """
        if isinstance(channel, predicates.telemetry_predicate):
            return channel

        c_pred = None
        v_pred = None
        t_pred = None

        if predicates.is_predicate(channel):
            c_pred = event
        else:
            channel = self.translate_event_name(channel)
            c_pred = predicates.equal_to(channel)

        if predicates.is_predicate(val_pred):
            v_pred = val_pred

        if predicates.is_predicate(fsw_time_pred):
            t_pred = fsw_time_pred

        return predicates.event_predicate(c_pred, v_pred, t_pred)

    def await_telemetry(
        self,
        channel,
        val_pred=None,
        fsw_time_pred=None,
        history=None,
        start=None,
        timeout=5,
    ):
        """
        TODO
        """
        t_pred = self.get_telemetry_predicate(channel, val_pred, fsw_time_pred)

        if history is None:
            history = self.get_telemetry_test_history()

        return self.find_history_item(history, t_pred, start, timeout)

    def await_telemetry_sequence(self, channels, history=None, start=None, timeout=5):
        """
        TODO
        """
        seq_preds = []
        for channel in channels:
            seq_preds.append(self.get_telemetry_predicate(channel))

        if history is None:
            history = self.get_telemetry_test_history()

        return self.find_history_sequence(seq_preds, history, start, timeout)

    def await_telemetry_count(
        self, count_pred, channels=None, history=None, start=None, timeout=5
    ):
        """
        TODO
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

        return self.find_history_count(count_pred, history, search, start, timeout)

    ######################################################################################
    #   Telemetry Asserts
    ######################################################################################
    def assert_telemetry(
        self,
        channel,
        val_pred=None,
        fsw_time_pred=None,
        history=None,
        start=None,
        timeout=0,
    ):
        """
        Asserts that a specified telemetry update was received. This function will first
        search the history then await future updates to find every specified channel in a
        sequence. If no valid update was received, then received before the timeout, this
        call will assert failure.
        """
        result = self.await_telemetry(
            channel, val_pred, fsw_time_pred, history, start, timeout
        )
        assert result is not None
        return result

    def assert_telemetry_sequence(self, channels, history=None, start=None, timeout=0):
        """
        Asserts that a sequence of Telemetry updates was received. This function will
        first search the history then await future updates to find every specified
        channel update in a sequence. This function will enforce that each element of the
        sequence occurred in order with respect to the flight software timestamps. If the
        specification of timestamp predicates is not sequential, the timestamps will
        likely fail. Note: It is reccomended (but not enforced) not to specify timestamps
        for this assert.
        """
        results = self.await_telemetry_sequence(channels, history, start, timeout)
        if results is None:
            assert False
        assert len(channels) == len(results)
        return results

    def assert_telemetry_count(
        self, count_pred, channels=None, history=None, start=None, timeout=0
    ):
        """
        An assert on the number of telemetry updates received. If the history doesn't
        have the correct update count, the call will await until a correct count is
        achieved or the timeout, at which point it will assert failure.
        """
        results = self.await_telemetry_count(
            count_pred, channels, history, start, timeout
        )
        if results is None:
            assert False
        assert count_pred(len(results))
        return results

    ######################################################################################
    #   Event Functions
    ######################################################################################
    def translate_event_name(self, event):
        """
        This function will translate the given mnemonic into an ID as defined by the
        flight software dictionary. This call will raise an error if the event given is
        not in the dictionary.
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
        """
        if isinstance(event, predicates.event_predicate):
            return event

        e_pred = None
        a_pred = None
        t_pred = None

        if predicates.is_predicate(event):
            e_pred = event
        else:
            event = self.translate_event_name(event)
            e_pred = predicates.equal_to(event)

        # TODO argument list predicate.

        if predicates.is_predicate(fsw_time_pred):
            t_pred = fsw_time_pred

        return predicates.event_predicate(e_pred, a_pred, t_pred)

    def await_event(
        self, event, args=None, fsw_time_pred=None, history=None, start=None, timeout=5
    ):
        """
        This function will first search the history for an event that satisfies the
        specified constraints. Then, if no message was found, await_event will wait for a
        valid event message until the timeout.
        """
        e_pred = self.get_event_predicate(event, args, fsw_time_pred)

        if history is None:
            history = self.get_event_test_history()

        return self.find_history_item(history, e_pred, start, timeout)

    def await_event_sequence(self, events, history=None, start=None, timeout=5):
        """
        This function will first search the history then await future messages to find
        every specified event in a sequence. This function will enforce that each element
        of the sequence occurred in order with respect to the flight software timestamps.
        Note: This function will return a list of events regardless of whether or not it
        finds all elements.
        """
        seq_preds = []
        for event in events:
            seq_preds.append(self.get_event_predicate(event))

        if history is None:
            history = self.get_event_test_history()

        return self.find_history_sequence(seq_preds, history, start, timeout)

    def await_event_count(
        self, count_pred, events=None, history=None, start=None, timeout=5
    ):
        """
        Will search the specified history until an amount of events is found. By default only searches future history.
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

        return self.find_history_count(count_pred, history, search, start, timeout)

    ######################################################################################
    #   Event Asserts
    ######################################################################################
    def assert_event(
        self, event, args=None, fsw_time_pred=None, history=None, start=None, timeout=0
    ):
        """
        Asserts that a specified event was received. This function will search the
        current history, and, if necessary, wait for an message that satisfies the
        specified constraints. If no valid event was received before the timeout,
        this call will assert failure.
        """
        result = self.await_event(event, args, fsw_time_pred, history, start, timeout)
        assert result is not None
        return result

    def assert_event_sequence(self, events, history=None, start=None, timeout=0):
        """
        Asserts that a sequence of event messages was received. This function will
        first search the history then await future messages to find every specified
        event in a sequence. This function will enforce that each element of the sequence
        occurred in order with respect to the flight software timestamps. If the
        specification of timestamp predicates is not sequential, the timestamps will
        likely fail. Note: It is reccomended (but not enforced) not to specify timestamps
        for this assert.
        """
        results = self.await_event_sequence(events, history, start, timeout)
        if results is None:
            assert False
        assert len(events) == len(results)
        return results

    def assert_event_count(
        self, count_pred, events=None, history=None, start=None, timeout=0
    ):
        """
        An assert on the number of events received. If the history doesn't have the
        correct event count, the call will await until a correct count is achieved or the
        timeout at which point it will assert failure.
        """
        results = self.await_event_count(count_pred, event, history, start, timeout)
        if results is None:
            assert False
        assert count_pred(len(results))
        return results

    ######################################################################################
    #   History Functions
    ######################################################################################
    def get_command_test_history(self):
        """
        Accessor for IntegrationTestAPI's command history
        :return: a history of CmdData Objects
        """
        return self.command_history

    def get_telemetry_test_history(self):
        """
        Accessor for IntegrationTestAPI's telemetry history
        :return: a history of ChData Objects
        """
        return self.telemetry_history

    def get_event_test_history(self):
        """
        Accessor for IntegrationTestAPI's event history
        :return: a history of EventData Objects
        """
        return self.event_history

    class TimeoutException(Exception):
        pass

    def _timeout_sig_handler(self, signum, frame):
        raise self.TimeoutException()

    def find_history_item(self, search_pred, history, start=None, timeout=0):
        """
        Awaits a single item in a history. First searches the history for the item, then
        waits until the item is found or the timeout is reached. Used as helpers for the
        test api.

        :param history: a history to await on.
        :param search_pred: a predicate to await with
        :param start: an indictor of when to start the search. See history.retrieve()
        :param timeout: how long to await the search in seconds.

        :return: If found, a data object, otherwise, None
        """
        current = history.retrieve(start)
        for item in current:
            if search_pred(item):
                return item

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self._timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if search_pred(item):
                            return item
                    time.sleep(0.1)
            except self.TimeoutException:
                # TODO Final check?
                return None
        else:
            return None

    def find_history_sequence(self, seq_preds, history, start=None, timeout=0):
        """
        This function can both search and await for a sequence of elements in a history.
        """
        current = history.retrieve(start)
        objects = []
        seq_preds = seq_preds.copy()

        if len(seq_preds) == 0:
            return []

        for item in current:
            if seq_preds[0](item):
                objects.append(item)
                seq_preds.pop(0)
                if len(seq_preds) == 0:
                    return objects

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self._timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if seq_preds[0](item):
                            objects.append(item)
                            seq_preds.pop(0)
                            if len(seq_preds) == 0:
                                signal.alarm(0)
                                return objects
                    time.sleep(0.1)
            except self.TimeoutException:
                # TODO Final check?
                return None
        else:
            return None

    def find_history_count(
        self, count_pred, history, search_pred=None, start=None, timeout=0
    ):
        """
        This function can both search and await for an amount of items in a history. First searches the history for the
        items, then waits until more are found to satisfy the count predicate or the
        timeout is reached. Used as a helper for the test api.

        :param count_pred: a predicate to determine if a correct amount has been reached.
        :param history: a history to await on.
        :param search_pred: a predicate to collect and count with. If not specified, all
            items will be counted.
        :param start: an indictor of when to start the search. See history.retrieve()
        :param timeout: the maximum time, in seconds, to wait and search.

        :return: If found a correct amount is found, a list data objects, otherwise, None
        """
        objects = []
        if search_pred is None:
            search_pred = predicates.always_true()
            objects = history.retrieve(start)
        else:
            current = history.retrieve(start)
            for item in current:
                if search_pred(item):
                    objects.append(item)

        if count_pred(len(objects)):
            return objects

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self._timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if search_pred(item):
                            objects.append(item)
                            if count_pred(len(objects)):
                                signal.alarm(0)
                                return objects
                    time.sleep(0.1)
            except self.TimeoutException:
                # TODO Final check?
                return None
        else:
            return None
