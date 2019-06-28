import os
import sys
import time
import thread
import unittest
filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, '../../../')
fprimeName = os.path.join(filename, '../../../../../Fw/Python/src')
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.testing_fw import api


class dummyPipeline():
    """
    This pipeline shares many of the same calls available in pipeline.standard. It
    is used by this testcase to feed simulated data to the test api.
    """
    def __init__(self):
        # History stores for each above type
        self.command_hist = None
        self.event_hist = None
        self.channel_hist = None
        self.event_subscribers = []
        self.channel_subscribers = []

    def setup(self, config, dictionary, logging_prefix=None, packet_spec=None):
        self.config = config
        self.dictionary = dictionary
        self.logging = logging_prefix
        self.packet_spec = packet_spec

    def connect(self, address, port):
        self.address = address
        self.port = port

    def send_command(self, command, args):
        # TODO construct CmdData and enqueue command history

    def get_event_history(self):
        return self.event_hist

    def get_channel_history(self):
        return self.channel_hist

    def get_command_history(self):
        return self.command_hist

    def register_event_history(self, history):
        self.event_subscribers.append(history)

    def remove_event_history(self, history):
        self.event_subscribers.remove(history)

    def register_telemetry_history(self, history):
        self.channel_subscribers.append(history)

    def remove_telemetry_history(self, history):
        self.channel_subscribers.remove(history)

    def enqueue_event(self, event):
        """
        Used by the unit test to feed simulated data objects into the pipeline
        """
        self.event_hist.data_callback(event)
        for history in self.event_subscribers:
            history.data_callback(event)

    def enqueue_telemetry(self, channel):
        """
        Used by the unit test to feed simulated data objects into the pipeline
        """
        self.channel_hist.data_callback(channel)
        for history in self.channel_subscribers:
            history.data_callback(channel)


class APITestCases(unittest.TestCase):
    def setUp(self):
        self.pipeline = dummyPipeline()
        self.api = IntegrationTestAPI(self.pipeline)

    def fill_history(callback, items, timestep=0):
        for items in items:
            if(timestep):
                time.sleep(timestep)
            callback(item)

    def fill_history_async(self, callback, items, timestep=1):
        thread.start_new_thread(self.fill_history, (callback, items, timestep))

    def assert_lists_equal(self, listA, listB):
        assert len(listA) == len(listB), "assert that both lists to compare are of equal length({}, {})".format(len(listA), len(listB))
        for i in range(len(listA)):
            assert listA[i] == listB[i], "assert that element {} of the lists are equal ({} == {})".format(i, listA[i], listB[i])

    def test_history_search(self):
        assert False, "congrats, you set up the class correctly."


if __name__ == '__main__':
    unittest.main()
