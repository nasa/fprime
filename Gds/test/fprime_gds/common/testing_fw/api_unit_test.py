import math
import os
import sys
import threading
import time
import unittest

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../../../src")
fprimeName = os.path.join(filename, "../../../../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.history.test import TestHistory
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.utils.config_manager import ConfigManager

# these imports are needed to generate data objects.
from fprime.common.models.serialize.i32_type import I32Type
from fprime.common.models.serialize.u32_type import U32Type
from fprime.common.models.serialize.string_type import StringType
from fprime.common.models.serialize.time_type import TimeType

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.cmd_data import CmdData
from fprime_gds.common.data_types.event_data import EventData


class UTPipeline(StandardPipeline):
    """
    This pipeline shares many of the same calls available in pipeline.standard. It
    is used by this testcase to feed simulated data to the test api.
    """
    def __init__(self):
        self.command_count = 0
        self.t0 = TimeType()
        StandardPipeline.__init__(self)

    def connect(self, address, port):
        pass

    def disconnect(self):
        pass

    def send_command(self, command, args):
        command_template = self.command_id_dict[command]
        cmd_data = CmdData(tuple(args), command_template)
        self.command_hist.data_callback(cmd_data)
        for hist in self.command_subscribers:
            hist.data_callback(cmd_data)

        ev_temp = self.event_name_dict["CommandReceived"]
        event = EventData((U32Type(cmd_data.get_id()),), self.t0 + time.time(), ev_temp)
        self.enqueue_event(event)

        ev_temp = self.event_name_dict["HistorySizeUpdate"]
        evr_size = U32Type(len(self.event_hist.retrieve()))
        cmd_size = U32Type(len(self.command_hist.retrieve()))
        ch_size = U32Type(len(self.channel_hist.retrieve()))
        event = EventData((evr_size, cmd_size, ch_size), self.t0 + time.time(), ev_temp)
        self.enqueue_event(event)

        self.command_count += 1
        ch_temp = self.channel_name_dict["CommandCounter"]
        update = ChData(U32Type(self.command_count), self.t0 + time.time(), ch_temp)
        self.enqueue_telemetry(update)

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
    @classmethod
    def setUpClass(cls):
        cls.pipeline = UTPipeline()
        config = ConfigManager()
        filename = os.path.dirname(__file__)
        path = os.path.join(filename, "./UnitTestDictionary.xml")
        cls.pipeline.setup(config, path)
        log_path = os.path.join(filename, "./logs")
        cls.api = IntegrationTestAPI(cls.pipeline, log_path)
        cls.case_list = [] # TODO find a better way to do this.
        cls.threads = []

    def setUp(self):
        for t in self.threads:
            if t.isAlive():
                t.join()
        self.threads.clear()
        count = len(self.case_list)
        self.api.start_test_case(self._testMethodName, count)
        self.case_list.append(1)
        self.tHistory = TestHistory()
        self.t0 = TimeType()

    @classmethod
    def tearDownClass(cls):
        cls.pipeline.disconnect()
        cls.api.teardown()

    ######################################################################################
    #   Test Case Helper Methods
    ######################################################################################
    def fill_history(self, callback, items, timestep=0):
        for item in items:
            if timestep:
                time.sleep(timestep)
            if isinstance(item, ChData) or isinstance(item, EventData):
                if item.time == 0:
                    item.time = self.t0 + time.time()
            callback(item)

    def fill_history_async(self, callback, items, timestep=1):
        t = threading.Thread(target=self.fill_history, args=(callback, items, timestep))
        self.threads.append(t)
        t.start()
        return t

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

    def get_counter_sequence(self, length):
        seq = []
        for i in range(0, length):
            ch_temp = self.pipeline.get_channel_name_dictionary()["Counter"]
            seq.append(ChData(U32Type(i), TimeType(), ch_temp))
        return seq

    def get_oscillator_sequence(self, length):
        seq = []
        for i in range(0, length):
            ch_temp = self.pipeline.get_channel_name_dictionary()["Oscillator"]
            val = int(round(10*math.sin(math.radians(i))))
            seq.append(ChData(I32Type(val), TimeType(), ch_temp))
        return seq

    def get_severity_event(self, severity="DIAGNOSTIC"):
        name = "Severity" + severity
        temp = self.pipeline.get_event_name_dictionary()[name]
        event = EventData(tuple(), TimeType(), temp)
        return event

    def get_severity_sequence(self, length, severity="DIAGNOSTIC"):
        seq = []
        for i in range(0, length):
            seq.append(self.get_severity_event(severity))
        return seq

    class AssertionFailure(Exception):
        """
        Used to differentiate an AssertionError in test cases that intentionally raise an
        assertion error.
        """
        pass

    ######################################################################################
    #   Test Cases
    ######################################################################################
    def test_dummy_pipeline(self):
        length = 15
        event_list = self.get_severity_sequence(length)
        t1 = self.fill_history_async(self.pipeline.enqueue_event, event_list, 0.1)
        print("waiting for queue to fill")
        pred = predicates.greater_than_or_equal_to(length // 2)
        results = self.api.await_event_count(pred)
        assert pred(len(results)), "the correct amount of objects was received"
        t1.join()
        evr_hist = self.api.get_event_test_history()
        item_count = len(evr_hist)
        assert (
            item_count == length
        ), "Were the correct number of items in the history? ({},{})".format(
            item_count, length
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

    def test_get_latest_fsw_time(self):
        ts0 = self.api.get_latest_time()

        time.sleep(0.1)

        ts1 = self.api.get_latest_time()
        assert ts0 is ts1, "The starting timestamp should not have changed if no dataobjects were enqueued"

        count_seq = self.get_counter_sequence(100)
        event_seq = self.get_severity_sequence(100)
        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.02)
        t2 = self.fill_history_async(self.pipeline.enqueue_event, event_seq, 0.02)

        last = ts0
        for i in range(1, 10):
            time.sleep(0.1)
            tsi = self.api.get_latest_time()
            assert tsi > last, "Iter {}: {} should be greater than {}".format(i, tsi, last)
            last = tsi

        t1.join()
        t2.join()

        tsn_1 = self.api.get_latest_time()
        assert tsn_1 > last, "The final timestamp, {}, should be greater than {}.".format(tsn_1, last)

        time.sleep(0.1)

        tsn_2 = self.api.get_latest_time()
        assert tsn_2 == tsn_1, "The timestamp should not have changed, while no data was streaming."

    def test_clear_histories(self):
        eventHistory = self.api.get_event_test_history()
        channelHistory = self.api.get_telemetry_test_history()
        commandHistory = self.api.get_command_test_history()

        self.api.clear_histories()
        assert eventHistory.size() == 0, "eventHistory should be empty"
        assert channelHistory.size() == 0, "channelHistory should be empty"

        count_seq = self.get_counter_sequence(100)
        event_seq = self.get_severity_sequence(100)
        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.02)
        t2 = self.fill_history_async(self.pipeline.enqueue_event, event_seq, 0.02)
        t1.join()
        t2.join()

        sizeE = eventHistory.size()
        iE = sizeE // 2
        firstE = eventHistory[iE]
        timeE = firstE.get_time()

        sizeC = channelHistory.size()
        iC = 0
        for i in range(0, channelHistory.size()):
            if channelHistory[i].get_time() >= timeE:
                iC = i
                break
        firstC = channelHistory[iC]

        self.api.clear_histories(timeE)
        msg = "The event history should have been reduced by {} elements".format(iE)
        assert sizeE - iE == eventHistory.size(), msg
        msg = "The element with the timestamp should be first in the history"
        assert firstE is eventHistory[0], msg
        msg = "The channel history should have been reduced by {} elements".format(iC)
        assert sizeC - iC == channelHistory.size(), msg
        msg = "The first element in the history should be the first with a valid time"
        assert firstC is channelHistory[0], msg

        args1 = []
        self.api.send_command("TEST_CMD_1", args1)
        assert commandHistory.size() > 0, "history size should be greater than 0"
        assert channelHistory.size() > 0, "history size should be greater than 0"
        assert eventHistory.size() > 0, "history size should be greater than 0"

        self.api.clear_histories()

        assert commandHistory.size() == 0, "history size should be 0"
        assert channelHistory.size() == 0, "history size should be 0"
        assert eventHistory.size() == 0, "history size should be 0"

    def test_registering_and_removing_subhistories(self):
        # Verifying that retrieving a subhistory for events behaves as expected
        event_hist = self.api.get_event_test_history()
        self.pipeline.enqueue_event(self.get_severity_event())
        assert event_hist.size() == 1, "There should be one event in the api's history"

        event_subhist = self.api.get_event_subhistory()
        assert event_subhist.size() == 0, "There should be no events in the subhistory"

        self.pipeline.enqueue_event(self.get_severity_event())

        assert event_hist.size() == 2, "There should be two events in the api's history"
        assert event_subhist.size() == 1, "There should be one event in the subhistory"

        assert self.api.remove_event_subhistory(event_subhist), "remove should succeed"

        self.pipeline.enqueue_event(self.get_severity_event())

        assert event_hist.size() == 3, "There should be three events in the api's history"
        assert event_subhist.size() == 1, "There should be one event in the subhistory"

        self.api.clear_histories()

        assert event_hist.size() == 0, "There should be no events in the api's history"
        assert event_subhist.size() == 1, "There should be one event in the subhistory"

        assert not self.api.remove_event_subhistory(event_subhist), "should not remove twice"

        # same checks, but for telemetry
        telem_seq = self.get_counter_sequence(3)
        telem_hist = self.api.get_telemetry_test_history()
        self.pipeline.enqueue_telemetry(telem_seq[0])
        assert telem_hist.size() == 1, "There should be one update in the api's history"

        telem_subhist = self.api.get_telemetry_subhistory()
        assert telem_subhist.size() == 0, "There should be no updates in the subhistory"

        self.pipeline.enqueue_telemetry(telem_seq[1])

        assert telem_hist.size() == 2, "There should be two updates in the api's history"
        assert telem_subhist.size() == 1, "There should be one update in the subhistory"

        assert self.api.remove_telemetry_subhistory(telem_subhist), "remove should succeed"

        self.pipeline.enqueue_telemetry(telem_seq[2])

        assert telem_hist.size() == 3, "There should be three updates in the api's history"
        assert telem_subhist.size() == 1, "There should be one update in the subhistory"

        self.api.clear_histories()

        assert telem_hist.size() == 0, "There should be no updates in the api's history"
        assert telem_subhist.size() == 1, "There should be one update in the subhistory"

        assert not self.api.remove_telemetry_subhistory(telem_subhist), "should not remove twice"

    def test_translate_command_name(self):
        assert self.api.translate_command_name("TEST_CMD_1") == 1
        assert self.api.translate_command_name("TEST_CMD_2") == 2
        assert self.api.translate_command_name("TEST_CMD_3") == 3
        assert self.api.translate_command_name(1) == 1
        assert self.api.translate_command_name(2) == 2
        assert self.api.translate_command_name(3) == 3
        try:
            self.api.translate_command_name("DOES_NOT_EXIST")
            assert False, "the api should have raised a KeyError"
        except KeyError:
            assert True, "the api raised the correct error"
        try:
            self.api.translate_command_name(0)
            assert False, "the api should have raised a KeyError"
        except KeyError:
            assert True, "the api raised the correct error"

    def test_send_command(self):
        args1 = []
        self.api.send_command("TEST_CMD_1", args1)
        self.api.send_command(1, args1)
        args2 = ["0x01", "0x02"]
        self.api.send_command("TEST_CMD_2", args2)
        self.api.send_command(2, args2)
        args3 = ["test message for the test command"]
        self.api.send_command("TEST_CMD_3", args3)
        self.api.send_command(3, args3)
        hist = self.api.get_command_test_history()
        assert hist.size() == 6
        for cmd in hist:
            print(cmd)

    def test_send_and_await_telemetry(self):
        result = self.api.send_and_await_telemetry("TEST_CMD_1", channels="CommandCounter")
        assert result is not None, "the search should find the telemetry generated by UTPipeline"

        self.api.clear_histories()

        seq = ["CommandCounter"] + ["Counter"] * 5
        self.fill_history_async(self.pipeline.enqueue_telemetry, self.get_counter_sequence(10), 0.01)
        results1 = self.api.send_and_await_telemetry("TEST_CMD_1", channels=seq)
        assert len(results1) == 6, "Should have gotten 6 results out of the await"

        self.fill_history_async(self.pipeline.enqueue_telemetry, self.get_counter_sequence(10), 0.01)
        results2 = self.api.send_and_await_telemetry("TEST_CMD_1", channels=seq)
        assert len(results2) == 6, "Should have gotten 6 results out of the await"

        for i in range(0, 6):
            assert results1[i] != results2[i], "These sequences should be unique items"

        self.api.clear_histories()

        seq = ["CommandCounter"] + ["Oscillator"] * 5
        self.fill_history_async(self.pipeline.enqueue_telemetry, self.get_oscillator_sequence(10), 0.01)
        results = self.api.send_and_await_telemetry("TEST_CMD_1", channels=seq)
        assert len(results) == 6, "Should have gotten 6 results out of the await"

    def test_send_and_await_event(self):
        result = self.api.send_and_await_event("TEST_CMD_1", events="CommandReceived")
        assert result is not None, "the search should have found the CommandReceived Event"

        self.api.clear_histories()

        seq = ["CommandReceived"] + ["SeverityDIAGNOSTIC"] * 5
        self.fill_history_async(self.pipeline.enqueue_event, self.get_severity_sequence(10), 0.01)
        results1 = self.api.send_and_await_event("TEST_CMD_1", events=seq)
        assert len(results1) == 6, "Should have gotten 6 results out of the await"

        self.fill_history_async(self.pipeline.enqueue_event, self.get_severity_sequence(10), 0.01)
        results2 = self.api.send_and_await_event("TEST_CMD_1", events=seq)
        assert len(results2) == 6, "Should have gotten 6 results out of the await"

        for i in range(0, 6):
            assert results1[i] != results2[i], "These sequences should be unique items"

    def test_send_and_assert_telemetry(self):
        self.api.send_and_assert_telemetry("TEST_CMD_1", channels="CommandCounter")

        self.api.clear_histories()

        seq = ["CommandCounter"] + ["Counter"] * 5
        self.fill_history_async(self.pipeline.enqueue_telemetry, self.get_counter_sequence(10), 0.01)
        results1 = self.api.send_and_assert_telemetry("TEST_CMD_1", channels=seq, timeout=5)

        self.fill_history_async(self.pipeline.enqueue_telemetry, self.get_counter_sequence(10), 0.01)
        results2 = self.api.send_and_assert_telemetry("TEST_CMD_1", channels=seq, timeout=5)

        for i in range(0, 6):
            assert results1[i] != results2[i], "These sequences should be unique items"

        self.api.clear_histories()

        seq = ["CommandCounter"] + ["Oscillator"] * 5
        self.fill_history_async(self.pipeline.enqueue_telemetry, self.get_oscillator_sequence(10), 0.01)
        self.api.send_and_assert_telemetry("TEST_CMD_1", channels=seq, timeout=5)

    def test_send_and_assert_event(self):
        self.api.send_and_assert_event("TEST_CMD_1", events="CommandReceived")

        self.api.clear_histories()

        seq = ["CommandReceived"] + ["SeverityDIAGNOSTIC"] * 5
        self.fill_history_async(self.pipeline.enqueue_event, self.get_severity_sequence(10), 0.01)
        results1 = self.api.send_and_assert_event("TEST_CMD_1", events=seq, timeout=5)

        self.fill_history_async(self.pipeline.enqueue_event, self.get_severity_sequence(10), 0.01)
        results2 = self.api.send_and_assert_event("TEST_CMD_1", events=seq, timeout=5)

        for i in range(0, 6):
            assert results1[i] != results2[i], "These sequences should be unique items"

    def test_translate_telemetry_name(self):
        assert self.api.translate_telemetry_name("CommandCounter") == 1
        assert self.api.translate_telemetry_name("Oscillator") == 2
        assert self.api.translate_telemetry_name("Counter") == 3
        assert self.api.translate_telemetry_name(1) == 1
        assert self.api.translate_telemetry_name(2) == 2
        assert self.api.translate_telemetry_name(3) == 3
        try:
            self.api.translate_command_name("DOES_NOT_EXIST")
            assert False, "the api should have raised a KeyError"
        except KeyError:
            assert True, "the api raised the correct error"
        try:
            self.api.translate_command_name(0)
            assert False, "the api should have raised a KeyError"
        except KeyError:
            assert True, "the api raised the correct error"

    def test_get_telemetry_pred(self):
        pred = predicates.telemetry_predicate()
        result = self.api.get_telemetry_pred(pred)
        assert pred == result, "should return when channel is already telem_pred"

        update = self.get_counter_sequence(1)[0]
        pred = self.api.get_telemetry_pred(update.get_id(), update.get_val())
        assert pred(update), "predicate should return true when fields are specified"

    def test_await_telemetry(self):
        seq = self.get_counter_sequence(20)
        self.fill_history_async(self.pipeline.enqueue_telemetry, seq[0:10], 0.01)
        result = self.api.await_telemetry("Counter", 8)
        assert result is not None, "Await should have found a correct channel update: {}".format(result)

        time.sleep(1)

        self.fill_history_async(self.pipeline.enqueue_telemetry, seq[10:20], 0.01)
        result = self.api.await_telemetry("Counter", 8)
        assert result is None, "Await should not have found an update: {}".format(result)

        self.api.clear_histories()

        self.fill_history_async(self.pipeline.enqueue_telemetry, seq, 0.1)
        result = self.api.await_telemetry("Counter", 15, timeout=1)
        assert result is None, "Await should not have found an update: {}".format(result)

    def test_await_telemetry_sequence(self):
        count_seq = self.get_counter_sequence(20)
        sin_seq = self.get_oscillator_sequence(100)

        search_seq = []
        for i in range(15, 20):
            pred = self.api.get_telemetry_pred("Counter", i)
            search_seq.append(pred)

        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        t2 = self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        results = self.api.await_telemetry_sequence(search_seq)

        assert len(results) == len(search_seq), "lists should have the same length"
        for i in range(0, len(results)):
            msg = predicates.get_descriptive_string(results[i], search_seq[i])
            assert search_seq[i](results[i]), msg

        t1.join()
        t2.join()
        results = self.api.await_telemetry_sequence(search_seq)
        assert len(results) < len(search_seq), "repeating the search should not complete"

        self.api.clear_histories()

        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        t2 = self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        results = self.api.await_telemetry_sequence(search_seq, timeout=1)
        assert len(results) < len(search_seq), "repeating the search should not complete"

        t1.join()
        t2.join()

    def test_await_telemetry_count(self):
        count_seq = self.get_counter_sequence(20)
        sin_seq = self.get_oscillator_sequence(100)

        pred = predicates.greater_than_or_equal_to(10)
        search_pred = self.api.get_telemetry_pred("Counter", pred)
        count_pred = predicates.within_range(10, 20)

        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        t2 = self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        results = self.api.await_telemetry_count(count_pred, search_pred)
        msg = predicates.get_descriptive_string(len(results), count_pred)
        assert count_pred(len(results)), msg
        t1.join()
        t2.join()

        self.api.clear_histories()

        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        t2 = self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        results = self.api.await_telemetry_count(100)
        assert len(results) == 100, "await count should have found 100 items"
        t1.join()
        t2.join()

        self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.02)
        results = self.api.await_telemetry_count(100, timeout=1)
        assert len(results) < 100, "await count should have found fewer 100 items"

    def test_assert_telemetry(self):
        seq = self.get_counter_sequence(20)
        self.fill_history_async(self.pipeline.enqueue_telemetry, seq[0:10], 0.01)
        self.api.assert_telemetry("Counter", 8, timeout=1)

        time.sleep(1)

        self.fill_history_async(self.pipeline.enqueue_telemetry, seq[10:20], 0.01)
        try:
            self.api.assert_telemetry("Counter", 8, start="NOW", timeout=1)
            raise self.AssertionFailure()
        except AssertionError:
            assert True, "api raised the correct error"
        except self.AssertionFailure:
            assert False, "api failed to raise an assertion error"

        self.api.clear_histories()

        self.fill_history_async(self.pipeline.enqueue_telemetry, seq, 0.1)
        try:
            self.api.assert_telemetry("Counter", 15, timeout=1)
            raise self.AssertionFailure()
        except AssertionError:
            assert True, "api raised the correct error"
        except self.AssertionFailure:
            assert False, "api failed to raise an assertion error"

    def test_assert_telemetry_sequence(self):
        count_seq = self.get_counter_sequence(20)
        sin_seq = self.get_oscillator_sequence(100)

        search_seq = []
        for i in range(15, 20):
            pred = self.api.get_telemetry_pred("Counter", i)
            search_seq.append(pred)

        self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        self.api.assert_telemetry_sequence(search_seq, start="NOW", timeout=5)
        self.api.assert_telemetry_sequence(search_seq)

        time.sleep(1)
        try:
            self.api.assert_telemetry_sequence(search_seq, start="NOW", timeout=5)
            raise self.AssertionFailure()
        except AssertionError:
            assert True, "api raised the correct error"
        except self.AssertionFailure:
            assert False, "api failed to raise an assertion error"

        self.api.clear_histories()

        self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.07)
        self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        try:
            self.api.assert_telemetry_sequence(search_seq, start="NOW", timeout=1)
            raise self.AssertionFailure()
        except AssertionError:
            assert True, "api raised the correct error"
        except self.AssertionFailure:
            assert False, "api failed to raise an assertion error"

    def test_assert_telemetry_count(self):
        count_seq = self.get_counter_sequence(20)
        sin_seq = self.get_oscillator_sequence(100)

        pred = predicates.greater_than_or_equal_to(10)
        search_pred = self.api.get_telemetry_pred("Counter", pred)
        count_pred = predicates.within_range(10, 20)

        self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        self.api.assert_telemetry_count(count_pred, search_pred, timeout=2)
        self.api.assert_telemetry_count(count_pred, search_pred)

        self.api.clear_histories()

        t1 = self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        t2 = self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.01)
        self.api.assert_telemetry_count(100, timeout=2)
        t1.join()
        t2.join()
        try:
            self.api.assert_telemetry_count(100)
            raise self.AssertionFailure()
        except AssertionError:
            assert True, "api raised the correct error"
        except self.AssertionFailure:
            assert False, "api failed to raise an assertion error"

        self.api.clear_histories()

        self.fill_history_async(self.pipeline.enqueue_telemetry, count_seq, 0.05)
        self.fill_history_async(self.pipeline.enqueue_telemetry, sin_seq, 0.02)
        try:
            self.api.assert_telemetry_count(100, timeout=1)
            raise self.AssertionFailure()
        except AssertionError:
            assert True, "api raised the correct error"
        except self.AssertionFailure:
            assert False, "api failed to raise an assertion error"

    def test_translate_event_name(self):
        assert self.api.translate_event_name("CommandReceived") == 1
        assert self.api.translate_event_name("HistorySizeUpdate") == 2
        assert self.api.translate_event_name("SeverityCOMMAND") == 3
        assert self.api.translate_event_name("SeverityACTIVITY_LO") == 4
        assert self.api.translate_event_name("SeverityACTIVITY_HI") == 5
        assert self.api.translate_event_name("SeverityWARNING_LO") == 6
        assert self.api.translate_event_name("SeverityWARNING_HI") == 7
        assert self.api.translate_event_name("SeverityDIAGNOSTIC") == 8
        assert self.api.translate_event_name("SeverityFATAL") == 9
        for i in range(1, 10):
            assert self.api.translate_event_name(i) == i

        try:
            self.api.translate_event_name("DOES_NOT_EXIST")
            assert False, "the api should have raised a KeyError"
        except KeyError:
            assert True, "the api raised the correct error"
        try:
            self.api.translate_event_name(0)
            assert False, "the api should have raised a KeyError"
        except KeyError:
            assert True, "the api raised the correct error"

    def test_get_event_pred(self):
        pred = predicates.event_predicate()
        result = self.api.get_event_pred(pred)
        assert pred == result, "should return when channel is already event_pred"

        message = self.get_severity_event("FATAL")
        pred = self.api.get_event_pred(message.get_id(), message.get_args(), message.get_severity())
        assert pred(message), "predicate should return true when fields are specified"
"""
    def test_await_event(self):
        raise NotImplementedError("Test Case is not yet implemented")

    def test_await_event_sequence(self):
        raise NotImplementedError("Test Case is not yet implemented")

    def test_await_event_count(self):
        raise NotImplementedError("Test Case is not yet implemented")

    def test_assert_event(self):
        raise NotImplementedError("Test Case is not yet implemented")

    def test_assert_event_sequence(self):
        raise NotImplementedError("Test Case is not yet implemented")

    def test_assert_event_count(self):
        raise NotImplementedError("Test Case is not yet implemented")
"""

if __name__ == "__main__":
    unittest.main()
