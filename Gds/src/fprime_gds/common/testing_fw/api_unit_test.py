import os
import sys
import time
import _thread
import unittest

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../../")
fprimeName = os.path.join(filename, "../../../../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.history.test import TestHistory
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager

# these imports are needed to generate data objects.
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.i32_type import I32Type
from fprime.common.models.serialize.string_type import StringType

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.event_data import EventData


class UTPipeline(StandardPipeline):
    """
    This pipeline shares many of the same calls available in pipeline.standard. It
    is used by this testcase to feed simulated data to the test api.
    """

    def connect(self, address, port):
        pass

    def disconnect(self):
        pass

    def send_command(self, command, args):
        command_template = self.command_id_dict[command]
        cmd_data = fprime_gds.common.data_types.cmd_data.CmdData(tuple(args), command_template)
        self.command_hist.data_callback(cmd_data)
        for hist in self.command_subscribers:
            hist.data_callback(cmd_data)

        # TODO Add command responses

    def get_event_history(self):
        return self.event_hist

    def get_channel_history(self):
        return self.channel_hist

    def get_command_history(self):
        return self.command_hist

    def enqueue_event(self, event):
        """
        Used by the unit test to feed simulated data objects into the pipeline
        """
        self.event_decoder.send_to_all(event)

    def enqueue_telemetry(self, channel):
        """
        Used by the unit test to feed simulated data objects into the pipeline
        """
        self.channel_decoder.send_to_all(channel)


class APITestCases(unittest.TestCase):
    def setUp(self):
        self.pipeline = UTPipeline()
        config = ConfigManager()
        filename = os.path.dirname(__file__)
        path = os.path.join(filename, "./UnitTestDictionary.xml")
        self.pipeline.setup(config, path)
        self.api = IntegrationTestAPI(self.pipeline)
        self.tHistory = TestHistory()

    def fill_history(self, callback, items, timestep=0):
        for item in items:
            if timestep:
                time.sleep(timestep)
            callback(item)

    def fill_history_async(self, callback, items, timestep=1):
        _thread.start_new_thread(self.fill_history, (callback, items, timestep))

    def assert_lists_equal(self, expected, actual):
        assert len(expected) == len(
            actual
        ), "the given list should have had the length {}, but instead had {}\nExpected {}\nActual{}".format(
            len(expected), len(actual), expected, actual
        )
        for i in range(len(expected)):
            assert (
                expected[i] == actual[i]
            ), "the {} element of the expected list should be {}, but was {}.".format(
                i, expected[i], actual[i]
            )

    def test_dummy_pipeline(self):
        listA = range(0, 50)
        self.fill_history_async(self.pipeline.enqueue_event, listA, 0.01)
        print("waiting for queue to fill")
        pred = predicates.equal_to(10)
        results = self.api.await_event_count(pred)
        assert pred(len(results)), "the correct amount of objects was received"
        print("received 10 objects: ")
        print(results)
        time.sleep(0.6)
        evr_hist = self.api.get_event_test_history()
        item_count = len(evr_hist)
        assert (
            item_count == 50
        ), "Were the correct number of items in the history? ({},{})".format(
            item_count, 50
        )

    def test_find_history_item(self):
        self.fill_history(self.tHistory.data_callback, range(0, 50))

        self.fill_history(self.tHistory.data_callback, range(0, 50))

        pred = predicates.equal_to(25)

        result = self.api.find_history_item(pred, self.tHistory)
        assert result == 25, "The search should have returned 25, but found {}".format(
            result
        )
        result = self.api.find_history_item(pred, self.tHistory, start=50)
        assert result == 25, "The search should have returned 25, but found {}".format(
            result
        )
        result = self.api.find_history_item(pred, self.tHistory, start=80)
        assert (
            result is None
        ), "The search should have returned None, but found {}".format(result)

    def test_find_history_item_timeout(self):
        pred = predicates.equal_to(25)

        listA = range(0, 50)
        self.fill_history_async(self.tHistory.data_callback, listA, 0.01)
        result = self.api.find_history_item(pred, self.tHistory, timeout=1)
        assert result == 25, "The search should have returned 25, but found {}".format(
            result
        )

        pred = predicates.equal_to(49)
        result = self.api.find_history_item(pred, self.tHistory, timeout=1)
        assert result == 49, "The search should have returned 49, but found {}".format(
            result
        )

        self.tHistory.clear()

        listA = range(0, 50)
        pred = predicates.equal_to(49)
        self.fill_history_async(self.tHistory.data_callback, listA, 0.1)
        result = self.api.find_history_item(pred, self.tHistory, timeout=1)
        assert (
            result is None
        ), "The search should have returned None, but found {}".format(result)

    def test_find_history_sequence(self):
        sequence = []
        for i in range(30, 40, 2):
            sequence.append(predicates.equal_to(i))

        self.fill_history(self.tHistory.data_callback, range(0, 50))
        results = self.api.find_history_sequence(sequence, self.tHistory)
        assert (
            len(results) == len(sequence)
        ), "The search should have found {}, but returned {}".format(range(30, 40, 2), results)
        self.assert_lists_equal(range(30, 40, 2), results)

        results = self.api.find_history_sequence(sequence, self.tHistory, start=34)
        assert (
            len(results) != len(sequence)
        ), "The search should have returned an incomplete list, but found {}".format(results)

        self.fill_history(self.tHistory.data_callback, range(0, 50))
        results = self.api.find_history_sequence(sequence, self.tHistory, start=34)
        assert (
            len(results) == len(sequence)
        ), "The search should have found {}, but returned {}".format(range(30, 40, 2), results)
        self.assert_lists_equal(range(30, 40, 2), results)

        results = self.api.find_history_sequence(sequence, self.tHistory, start=90)
        assert (
            len(results) != len(sequence)
        ), "The search should have returned an incomplete list, but found {}".format(results)

    def test_find_history_sequence_timeout(self):
        sequence = []
        for i in range(30, 40, 2):
            sequence.append(predicates.equal_to(i))

        self.fill_history_async(self.tHistory.data_callback, range(0, 50), 0.01)
        results = self.api.find_history_sequence(sequence, self.tHistory, timeout=1)
        assert results is not None, "The search should have found a sequence"
        self.assert_lists_equal(range(30, 40, 2), results)

        self.fill_history_async(self.tHistory.data_callback, range(0, 50), 0.01)
        results = self.api.find_history_sequence(
            sequence, self.tHistory, start=34, timeout=1
        )
        assert results is not None, "The search should have found a sequence"
        self.assert_lists_equal(range(30, 40, 2), results)

        self.tHistory.clear()

        self.fill_history_async(self.tHistory.data_callback, range(25, 50), 0.1)
        results = self.api.find_history_sequence(
            sequence, self.tHistory, start=90, timeout=1
        )
        assert (
            len(results) != len(sequence)
        ), "The search should have returned an incomplete list, but found {}".format(results)

    def test_find_history_count(self):
        count_pred = predicates.greater_than_or_equal_to(10)
        search_pred = predicates.greater_than_or_equal_to(40)

        self.fill_history(self.tHistory.data_callback, range(0, 50))
        results = self.api.find_history_count(count_pred, self.tHistory)
        self.assert_lists_equal(range(0, 50), results)

        results = self.api.find_history_count(count_pred, self.tHistory, search_pred)
        self.assert_lists_equal(range(40, 50), results)

        self.fill_history(self.tHistory.data_callback, range(50, 70))

        results = self.api.find_history_count(count_pred, self.tHistory, search_pred)
        self.assert_lists_equal(range(40, 70), results)

        results = self.api.find_history_count(count_pred, self.tHistory, start=60)
        self.assert_lists_equal(range(60, 70), results)

    def test_find_history_count_timeout(self):
        count_pred = predicates.greater_than_or_equal_to(10)
        search_pred = predicates.greater_than_or_equal_to(40)

        self.fill_history_async(self.tHistory.data_callback, range(0, 50), 0.01)

        results = self.api.find_history_count(count_pred, self.tHistory)
        assert (
            len(results) < 10
        ), "The search should have returned an incomplete list, but found {}".format(results)

        results = self.api.find_history_count(
            count_pred, self.tHistory, search_pred, timeout=2
        )
        self.assert_lists_equal(range(40, 50), results)

        self.fill_history_async(self.tHistory.data_callback, range(50, 60), 0.01)

        results = self.api.find_history_count(
            count_pred, self.tHistory, search_pred, start=50, timeout=2
        )
        self.assert_lists_equal(range(50, 60), results)

        self.tHistory.clear()

        self.fill_history_async(self.tHistory.data_callback, range(35, 60), 0.1)
        results = self.api.find_history_count(
            count_pred, self.tHistory, search_pred, timeout=1
        )
        assert (
            len(results) < 10
        ), "The search should have returned an incomplete list, but found {}".format(results)


if __name__ == "__main__":
    unittest.main()
