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
        
    def assert_send_cmd(self, cmd, cmd_args):
        """
        Sends a command and asserts that the command was translated successfully. If the command is in conflict
        with the flight dictionary, this will raise a test failure. 
        :param cmd: either the command id or a command mnemonic to specify the type of command
        :param cmd_args: a list of command arguments to send
        :return: If the assert is successful, will an instance of CmdData from data_types.cmd_data
        """
        pass
    
    def assert_received_tlm(self, tlm, timeout=0, val_pred=None, fsw_time_pred=None, history=None):
        """
        Asserts that a telemetry update was received on the specified channel. This assert will search the 
        history for a telemetry that satisfies the specified constraints. If unsuccessful, the assert will 
        then await a valid telemetry update on the history until the timeout. The assert will succeed and 
        return immediately when a valid telemetry object is found. If no valid update was found in the history 
        nor while waiting, the assert will fail. 
        :param tlm: either the channel id or a channel mnemonic to specify the telemetry channel
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :param val_pred: If specified, a valid telemetry update must have a value that satisfies this predicate.
        :param fsw_time_pred: If specified, a valid telemetry update must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the aggregate history.
        :return: If the assert is successful, will return the instance of ChData from data_types.ch_data that satisfied the assert.
        """
        pass

    def assert_received_evr(self, evr, timeout=0, args=None, fsw_time_pred=None, history=None):
        """
        Asserts that a specified event message was received. This funciton will search the history for an 
        event that satisfies the specified constraints. If unsuccessful, this call will await a valid 
        event until the timeout. The assert will succeed and return immediately when a valid event object 
        is found. If no valid message was found in the history nor while waiting, the assert will fail. 
        :param tlm: either the event id or a event mnemonic to specify the event type.
        :param timeout: If specified, the assert will behave as an await statement after searching available history. Negative values are ignored.
        :param args: If specified, a valid event message must have arguments matching the given list of args. Include None to ignore an element.
        :param fsw_time_pred: If specified, a valid event must have a fsw timestamp that satisfies this predicate.
        :param history: If specified, the assert will substitute the given history for the aggregate history.
        :return: If the assert is successful, will return the instance of ChData from data_types.ch_data that satisfied the assert.
        """
        pass
