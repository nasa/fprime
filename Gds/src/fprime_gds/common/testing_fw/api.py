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

        # Initialize latest time. Will be updated whenever a time query is made. 
        self.latest_time = -1

    def log_test_message(self, msg):
        """
        TODO: Define what a test log should look like and describe its parameters.
        """
        pass

    def start_test_case(self, name):
        """
        To be called at the start of a test case. This function inserts a log message to denote a new test
        case is beginning, records the latest time stamp in case the user clears the aggregate histories, and
        then clears the API's histories.
        """
        self.get_latest_fsw_time()  # called in case aggregate histories are cleared by the user
        self.log_test_message("Starting new test case: {}".format(name))
        self.clear_histories()

    def get_latest_fsw_time(self):
        """
        Finds the latest flight software time received by either the event or telemetry
        history.
        :return: a flight software timestamp
        """
        events = self.aggregate_event_history.retrieve()
        e_time = -1
        if len(events) > 0:
            e_time = events[-1].get_time()

        channels = self.aggregate_telemetry_history.retrieve()
        t_time = -1
        if len(channels) > 0:
            t_time = channels[-1].get_time()

        self.latest_time = max(e_time, t_time, self.latest_time)
        return self.latest_time

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
        if isinstance(command, str):
            cmd_dict = self.pipeline.get_command_name_dictionary()
            if command in cmd_dict:
                return cmd_dict.get(command, "id")
            else:
                raise KeyError("The given command mnemonic, {}, was not in the dictionary".format(command))
        else:
            cmd_dict = self.pipeline.get_command_id_dictionary()
            if command in cmd_dict:
                return command
            else:
                raise KeyError("The given command id, {}, was not in the dictionary".format(command))

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
        flight software dictionary. This call will raise an error if the channel given is
        not in the dictionary.

        Args:
            event: a mnemonic or id to specify the type of event
        Returns:
            the channel ID
        """
        if isinstance(channel, str):
            ch_dict = self.pipeline.get_channel_name_dictionary()
            if channel in ch_dict:
                return ch_dict.get(channel, "id")
            else:
                raise KeyError("The given channel mnemonic, {}, was not in the dictionary".format(channel))
        else:
            ch_dict = self.pipeline.get_channel_id_dictionary()
            if channel in ch_dict:
                return channel
            else:
                raise KeyError("The given channel id, {}, was not in the dictionary".format(channel))

    def get_telemetry_predicate(self, channel=None, value=None, fsw_time_pred=None):
        """
        This function will translate the channel ID, and construct a telemetry_predicate object. It
        is used as a helper by the IntegrationTestAPI, but could also be helpful to a user of the
        test API. If  channel is already an instance of telemetry_predicate, it will be returned
        immediately. The provided implementation of telemetry_predicate evaluates true if and only
        if all specified constraints are satisfied. If a specific constraint isn't specified, then
        it will not effect the outcome. If no constraints are specified, the predicate will always
        return true.

        Args:
            channel: an optional mnemonic (str), id (int), or predicate to specify the channel type
            value: optional value (object/number) or predicate to specify the value field
            fsw_time_pred: an optional predicate to specify the flight software timestamp
        Returns:
            an instance of telemetry_predicate
        """
        if isinstance(channel, predicates.telemetry_predicate):
            return channel

        c_pred = None
        v_pred = None
        t_pred = None

        if predicates.is_predicate(channel):
            c_pred = event
        elif channel is not None:
            channel = self.translate_event_name(channel)
            c_pred = predicates.equal_to(channel)

        if predicates.is_predicate(value):
            v_pred = value
        elif value is not None:
            v_pred = predicates.equal_to(value)

        if predicates.is_predicate(fsw_time_pred):
            t_pred = fsw_time_pred

        return predicates.event_predicate(c_pred, v_pred, t_pred)

    def await_telemetry(
        self, channel, value=None, fsw_time_pred=None, history=None, start=None, timeout=5,
    ):
        """
        A search for a single telemetry update received. If the history doesn't have the
        correct update, the call will await until a correct update is received or the
        timeout, at which point it will return None.

        Args:
            channel: a channel specifier (mnemonic, id, or predicate)
            value: optional value (object/number) or predicate to specify the value field
            fsw_time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the data EChData object found during the search, otherwise, None
        """
        t_pred = self.get_telemetry_predicate(channel, value, fsw_time_pred)

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
        self, count, channels=None, history=None, start=None, timeout=5
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

        return self.find_history_count(count, history, search, start, timeout)

    ######################################################################################
    #   Telemetry Asserts
    ######################################################################################
    def assert_telemetry(
        self, channel, value=None, fsw_time_pred=None, history=None, start=None, timeout=0,
    ):
        """
        Asserts that a specified telemetry update was received. This function will first
        search the history then await future updates to find every specified channel in a
        sequence. If no valid update was received, then received before the timeout, this
        call will assert failure.
        """
        result = self.await_telemetry(
            channel, value, fsw_time_pred, history, start, timeout
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
        self, count, channels=None, history=None, start=None, timeout=0
    ):
        """
        An assert on the number of telemetry updates received. If the history doesn't
        have the correct update count, the call will await until a correct count is
        achieved or the timeout, at which point it will assert failure.
        """
        results = self.await_telemetry_count(
            count, channels, history, start, timeout
        )
        if results is None:
            assert False
        if predicates.is_predicate(count):
            count_pred = count
        elif isinstance(count, int):
            count_pred = predicates.equal_to(count)
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

        Args:
            event: a mnemonic (str) or ID (int) to specify the type of event
        Returns:
            the event ID (int)
        """
        if isinstance(event, str):
            event_dict = self.pipeline.get_event_name_dictionary()
            if event in event_dict:
                return event_dict.get(event, "id")
            else:
                raise KeyError("The given event mnemonic, {}, wstringas not in the dictionary".format(event))
        else:
            event_dict = self.pipeline.get_event_id_dictionary()
            if event in event_dict:
                return event
            else:
                raise KeyError("The given event id, {}, was not in the dictionary".format(event))

    def get_event_predicate(self, event=None, args=None, fsw_time_pred=None):
        """
        This function will translate the event ID, and construct an event_predicate object. It is
        used as a helper by the IntegrationTestAPI, but could also be helpful to a user of the test
        API. If event is already an instance of event_predicate, it will be returned immediately.
        The provided implementation of event_predicate evaluates true if and only if all specified
        constraints are satisfied. If a specific constraint isn't specified, then it will not
        effect the outcome. If no constraints are specified, the predicate will always return true.

        Args:
            event: an optional mnemonic (str), id (int), or predicate to specify the event type
            args: optional list of expected arguments (list of values, predicates, or None to ignore)
            fsw_time_pred: an optional predicate to specify the flight software timestamp
        Returns:
            an instance of event_predicate
        """
        if isinstance(event, predicates.event_predicate):
            return event

        e_pred = None
        a_pred = None
        t_pred = None

        if predicates.is_predicate(event):
            e_pred = event
        elif event is not None:
            event = self.translate_event_name(event)
            e_pred = predicates.equal_to(event)

        if predicates.is_predicate(args):
            a_pred = args
        elif args is not None:
            a_pred = predicates.args_predicate(args)

        if predicates.is_predicate(fsw_time_pred):
            t_pred = fsw_time_pred

        return predicates.event_predicate(e_pred, a_pred, t_pred)

    def await_event(
        self, event, args=None, fsw_time_pred=None, history=None, start=None, timeout=5
    ):
        """
        A search for a single event message received. If the history doesn't have the
        correct message, the call will await until a correct message is received or the
        timeout, at which point it will return None.

        Args:
            event: an event specifier (mnemonic, id, or predicate)
            args: a list of expected arguments (list of values, predicates, or None for don't care)
            fsw_time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the data EventData object found during the search, otherwise, None
        """
        e_pred = self.get_event_predicate(event, args, fsw_time_pred)

        if history is None:
            history = self.get_event_test_history()

        return self.find_history_item(history, e_pred, start, timeout)

    def await_event_sequence(self, events, history=None, start=None, timeout=5):
        """
        A search for a sequence of event messages. If the history doesn't have the complete
        sequence, the call will await until the sequence is completed or the timeout, at
        which point it will assert failure.
        Note: It is reccomended (but not enforced) not to specify timestamps for this assert.
        Note: This function will always return a list of events the user should check if the
        sequence was completed.

        Args:
            events: an ordered list of event specifiers (mnemonic, id, or predicate)
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            an ordered list of EventData objects that satisfied the sequence
        """
        seq_preds = []
        for event in events:
            seq_preds.append(self.get_event_predicate(event))

        if history is None:
            history = self.get_event_test_history()

        return self.find_history_sequence(seq_preds, history, start, timeout)

    def await_event_count(
        self, count, events=None, history=None, start=None, timeout=5
    ):
        """
        A search on the number of events received. If the history doesn't have the correct event
        count, the call will await until a correct count is achieved or the timeout at which point
        it will return.
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

        return self.find_history_count(count, history, search, start, timeout)

    ######################################################################################
    #   Event Asserts
    ######################################################################################
    def assert_event(
        self, event, args=None, fsw_time_pred=None, history=None, start=None, timeout=0
    ):
        """
        An assert on a single event message received. If the history doesn't have the
        correct message, the call will await until a correct message is received or the
        timeout, at which point it will assert failure.

        Args:
            event: an event specifier (mnemonic, id, or predicate)
            args: a list of expected arguments (list of values, predicates, or None for don't care)
            fsw_time_pred: an optional predicate to specify the flight software timestamp
            history: if given, a substitute history that the function will search and await
            start: an optional index or predicate to specify the earliest item to search
            timeout: the number of seconds to wait before terminating the search (int)
        Returns:
            the data EventData object found during the search
        """
        pred = self.get_event_predicate(event, args, fsw_time_pred)
        result = self.await_event(event, args, fsw_time_pred, history, start, timeout)
        assert pred(result)
        return result

    def assert_event_sequence(self, events, history=None, start=None, timeout=0):
        """
        Asserts that a sequence of event messages is received. If the history doesn't have the
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
        assert len(events) == len(results)
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
        assert count_pred(len(results))
        return results

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

    class TimeoutException(Exception):
        pass

    def _timeout_sig_handler(self, signum, frame):
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
                return None
        else:
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
        current = history.retrieve(start)
        sequence = []
        seq_preds = seq_preds.copy()

        if len(seq_preds) == 0:
            return []

        for item in current:
            if seq_preds[0](item):
                sequence.append(item)
                seq_preds.pop(0)
                if len(seq_preds) == 0:
                    return sequence

        if timeout:
            try:
                signal.signal(signal.SIGALRM, self._timeout_sig_handler)
                signal.alarm(timeout)
                while True:
                    new_items = history.retrieve_new()
                    for item in new_items:
                        if seq_preds[0](item):
                            sequence.append(item)
                            seq_preds.pop(0)
                            if len(seq_preds) == 0:
                                signal.alarm(0)
                                return sequence
                    time.sleep(0.1)
            except self.TimeoutException:
                return sequence
        else:
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
                return objects
        else:
            return objects
