"""
gds_test_api.py:

This file contains basic asserts that can support integration tests on an FPrime deployment. 
This API uses the standard pipeline to get access to commands, events, telemetry and dictionaries. 

:author: koran
"""

class IntegrationTestAPI:
    """
    Class used to collect basic assertions that would be used on a GDS Pipeline to carry out integration tests.
    :param pipeline: a pipeline object providing access to basic GDS functionality
    """
    def __init__(self, pipeline):
        self.pipeline = pipeline

    def clear_histories(self, fsw_time_stamp=None):
        """
        Clears histories. Because the command history is not correlated to a FSW timestamp, it will be cleared 
        entirely. This function can be used to setup tests so that the histories only represent objects received 
        during a test.
        :param fsw_time_stamp: If specified, event and telemetry histories will be cleared up until the timestamp. 
        """
        pass
        
    ###################################################################################
    ##  History Functions
    ###################################################################################
    def assert_send_command(self, command, args):
        """
        Sends a command and asserts that the command was translated successfully. If the command is in conflict
        with the flight dictionary, this will raise a test error. 
        Note: this assert does not check that the command was received by flight software, only that the command
        and arguments were valid with respect to the flight dictionary
        :param command: Either the command id or a command mnemonic to specify the type of command
        :param args: A list of command arguments to send
        :return: If the assert is successful, will return an instance of CmdData from data_types.cmd_data
        """
        pass

    ###################################################################################
    ##  Telemetry Functions
    ###################################################################################
    def await_telemetry(self, channel, val_pred=None, fsw_time_pred=None, history=None, timeout=5):
        """
        This function will first search the history for a telemetry update that satisfies the specified constraints. 
        Then, if no update was found, await_telemetry will wait for a valid telemetry update until the timeout. 
        :param channel: Either the channel id or a channel mnemonic to specify the telemetry channel
        :param val_pred: If specified, the telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, the telemetry update must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, will search and await the given history instead of the default history.
        :param timeout: The maximum time to wait for 
        :return: If the search is successful, will return the instance of ChData from data_types.ch_data that satisfied the assert, otherwise will return None.
        """
        pass

    def assert_receive_telemetry(self, channel, val_pred=None, fsw_time_pred=None, history=None, timeout=0):
        """
        Asserts that a specified telemetry update was received. This function will search the current history,
        and, if necessary wait for an update that satisfies the specified constraints. If no valid update was 
        received before the timeout, this call will assert failure.
        :param channel: Either the channel id or a channel mnemonic to specify the telemetry channel
        :param val_pred: If specified, a valid telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, a valid telemetry update must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the default history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, will return the instance of ChData from data_types.ch_data that satisfied the assert.
        """
        pass

    def assert_telemetry_count(self, count_pred, channel_list=None, val_pred=None, fsw_time_pred=None, history=None, timeout=0):
        """
        An assert on the number of telemetry updates received. If the history doesn't have the correct update 
        count, the call will await until a correct count is achieved or the timeout, at which point it will 
        assert failure.
        :param count_pred: A predicate to determine whether the correct amount has been received.
        :param channel_list: If specified, a counted telemetry update must have a channel on this list of channels.
        :param val_pred: If specified, a counted telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, a counted telemetry update must have a FSW timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the default history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, this call will return the number that satisfied the count predicate.
        """
        pass

    ###################################################################################
    ##  Event Functions
    ###################################################################################
    def await_event(self, event, args=None, fsw_time_pred=None, history=None, timeout=5):
        """
        This function will first search the history for an event that satisfies the specified constraints. 
        Then, if no message was found, await_event will wait for a valid event message until the timeout. 
        :param event: Either the event id or an event mnemonic to specify the event type.
        :param args: If specified, the event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, the telemetry update must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, will search and await the given history instead of the default history.
        :param timeout: The maximum time to wait for 
        :return: If the search is successful, will return the instance of ChData from data_types.ch_data that satisfied the assert, otherwise will return None.
        """
        pass

    def assert_receive_event(self, event, args=None, fsw_time_pred=None, history=None, timeout=0):
        """
        Asserts that a specified event was received. This function will search the current history, and, 
        if necessary, wait for an message that satisfies the specified constraints. If no valid event was 
        received before the timeout, this call will assert failure.
        :param event: Either the event id or an event mnemonic to specify the event type.
        :param args: If specified, a valid event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, a valid event must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the default history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, this call will return the instance of EventData from data_types.event_data that satisfied the assert.
        """
        pass

    def assert_event_count(self, count_pred, event_list=None, args=None, fsw_time_pred=None, history=None, timeout=0):
        """
        An assert on the number of events received. If the history doesn't have the correct event count, the
        call will await until a correct count is achieved or the timeout at which point it will assert failure.
        :param count_pred: A predicate to determine whether the correct amount has been received.
        :param event_list: If specified, a counted event must have an ID on this list of channels.
        :param args: If specified, a valid event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, a valid event must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the default history.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :return: If the assert is successful, this call will return the number that satisfied the count predicate.
        """
        pass

    ###################################################################################
    ##  Combined Functions
    ###################################################################################
    def send_and_await_telemetry(self, command, args, channel, timeout=5):
        """
        Sends the specified command and awaits the specified telemetry.
        :param command: Either the command id or a command mnemonic to specify the type of command
        :param args: A list of command arguments to send 
        :param channel: Either the channel id or a channel mnemonic to specify the telemetry channel
        :param timeout: The maximum time to wait for
        :return: If the search is successful, will return the instance of ChData from data_types.ch_data, otherwise will return None.
        """
        pass

    def send_and_await_event(self, command, args, event, timeout=5):
        """
        Sends the specified command and awaits the specified event message.
        :param command: Either the command id or a command mnemonic to specify the type of command
        :param args: A list of command arguments to send 
        :param channel: Either the channel id or a channel mnemonic to specify the type of event
        :param timeout: The maximum time to wait for
        :return: If the search is successful, will return the instance of EventData from data_types.event_data, otherwise will return None.
        """
        pass