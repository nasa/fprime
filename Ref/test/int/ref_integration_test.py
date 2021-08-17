import os
import sys
import time
import platform
import subprocess
from enum import Enum

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../../Gds/src")
fprimeName = os.path.join(filename, "../../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.utils.event_severity import EventSeverity


class TestRefAppClass(object):
    @classmethod
    def setup_class(cls):
        cls.pipeline = StandardPipeline()
        config = ConfigManager()
        filename = os.path.dirname(__file__)
        path = os.path.join(
            filename,
            "../../build-artifacts/{}/dict/RefTopologyAppDictionary.xml".format(
                platform.system()
            ),
        )
        cls.pipeline.setup(config, path, "/tmp")
        cls.pipeline.connect("127.0.0.1", 50050)
        logpath = os.path.join(filename, "./logs")
        cls.api = IntegrationTestAPI(cls.pipeline, logpath)
        cls.case_list = []  # TODO find a better way to do this.
        cls.dictionary = path

    @classmethod
    def teardown_class(cls):
        cls.pipeline.disconnect()
        cls.api.teardown()

    def setup_method(self, method):
        self.case_list.append(method.__name__)
        self.api.start_test_case(method.__name__, len(self.case_list))

    def test_is_streaming(self):
        results = self.api.assert_telemetry_count(5, timeout=10)
        for result in results:
            msg = "received channel {} update: {}".format(
                result.get_id(), result.get_str()
            )
            print(msg)

    def assert_command(self, command, args=[], max_delay=None, timeout=5):
        """
        This helper will send a command and verify that the command was dispatched and completed
        within the F' deployment. This helper can retroactively check that the delay between
        dispatch and completion is less than a maximum allowable delay.

        Args:
            command: the mnemonic (str) or ID (int) of the command to send
            args: a list of command arguments.
            max_delay: the maximum allowable delay between dispatch and completion (int/float)
            timeout: the number of seconds to wait before terminating the search (int)
        Return:
            returns a list of the EventData objects found by the search
        """
        cmd_id = self.api.translate_command_name(command)
        self.api.log(
            "Starting assert_command helper for {}({})".format(command, cmd_id)
        )
        events = []
        events.append(self.api.get_event_pred("OpCodeDispatched", [cmd_id, None]))
        events.append(self.api.get_event_pred("OpCodeCompleted", [cmd_id]))
        results = self.api.send_and_assert_event(command, args, events, timeout=timeout)
        if max_delay is not None:
            delay = results[1].get_time() - results[0].get_time()
            msg = "The delay, {}, between the two events should be < {}".format(
                delay, max_delay
            )
            assert delay < max_delay, msg
        self.api.log("assert_command helper completed successfully")
        return results

    """
    This enum is includes the values of EventSeverity that can be filtered by the ActiveLogger Component
    """
    FilterSeverity = Enum(
        "FilterSeverity",
        "WARNING_HI WARNING_LO COMMAND ACTIVITY_HI ACTIVITY_LO DIAGNOSTIC",
    )

    def set_event_filter(self, severity, enabled):
        """
        This helper will send a command that updates the given severity filter on the ActiveLogger
        Component in the Ref App.
        Args:
            severity: A valid FilterSeverity Enum Value (str) or an instance of FilterSeverity
            enabled: a boolean of whether or not to enable the given severity
        Return:
            boolean of whether the report filter was set successfully.
        """
        enabled = "ENABLED" if enabled else "DISABLED"
        if isinstance(severity, self.FilterSeverity):
            severity = severity.name
        else:
            severity = self.FilterSeverity[severity].name
        try:
            self.api.send_command(
                "eventLogger.SET_EVENT_FILTER",
                ["FILTER_" + severity, "FILTER_" + enabled],
            )
            return True
        except AssertionError:
            return False

    def set_default_filters(self):
        """
        Sets the default send filters on the ref aps ActiveLogger
        """
        self.set_event_filter("COMMAND", True)
        self.set_event_filter("ACTIVITY_LO", True)
        self.set_event_filter("ACTIVITY_HI", True)
        self.set_event_filter("WARNING_LO", True)
        self.set_event_filter("WARNING_HI", True)
        self.set_event_filter("DIAGNOSTIC", False)

    def test_send_command(self):
        self.assert_command("cmdDisp.CMD_NO_OP", max_delay=0.1)
        assert self.api.get_command_test_history().size() == 1
        self.assert_command("cmdDisp.CMD_NO_OP", max_delay=0.1)
        assert self.api.get_command_test_history().size() == 2

    def test_send_and_assert_no_op(self):
        length = 100
        failed = 0
        evr_seq = ["OpCodeDispatched", "NoOpReceived", "OpCodeCompleted"]
        any_reordered = False
        dropped = False
        for i in range(0, length):
            results = self.api.send_and_await_event(
                "cmdDisp.CMD_NO_OP", events=evr_seq, timeout=25
            )
            msg = "Send and assert NO_OP Trial #{}".format(i)
            if not self.api.test_assert(len(results) == 3, msg, True):
                items = self.api.get_event_test_history().retrieve()
                last = None
                reordered = False
                for item in items:
                    if last is not None:
                        if item.get_time() < last.get_time():
                            self.api.log(
                                "during iteration #{}, a reordered event was detected: {}".format(
                                    i, item
                                )
                            )
                            any_reordered = True
                            reordered = True
                            break
                    last = item
                if not reordered:
                    self.api.log(
                        "during iteration #{}, a dropped event was detected".format(i)
                    )
                    dropped = True
                failed += 1
            self.api.clear_histories()

        case = True
        case &= self.api.test_assert(
            not any_reordered, "Expected no events to be reordered.", True
        )
        case &= self.api.test_assert(
            not dropped, "Expected no events to be dropped.", True
        )
        msg = "{} sequences failed out of {}".format(failed, length)
        case &= self.api.test_assert(failed == 0, msg, True)

        assert (
            case
        ), "Expected all checks to pass (reordering, dropped events, all passed). See log."

    def test_bd_cycles_ascending(self):
        length = 60
        count_pred = predicates.greater_than(length - 1)
        results = self.api.await_telemetry_count(
            count_pred, "BD_Cycles", timeout=length
        )
        last = None
        reordered = False
        ascending = True
        for result in results:
            if last is not None:
                last_time = last.get_time()
                result_time = result.get_time()
                if result_time - last_time > 1.5:
                    msg = "FSW didn't send an update between {} and {}".format(
                        last_time.to_readable(), result_time.to_readable()
                    )
                    self.api.log(msg)
                elif result_time < last_time:
                    msg = "There is potential reorder error between {} and {}".format(
                        last_time, result_time
                    )
                    self.api.log(msg)
                    reordered = True

                if not result.get_val() > last.get_val():
                    msg = "Not all updates ascended: First ({}) Second ({})".format(
                        last.get_val(), result.get_val()
                    )
                    self.api.log(msg)
                    ascending = False

            last = result

        case = True
        case &= self.api.test_assert(ascending, "Expected all updates to ascend.", True)
        case &= self.api.test_assert(
            not reordered, "Expected no updates to be dropped.", True
        )
        self.api.predicate_assert(
            count_pred,
            len(results) - 1,
            "Expected >= {} updates".format(length - 1),
            True,
        )
        self.api.assert_telemetry_count(0, "RgCycleSlips")
        assert case, "Expected all checks to pass (ascending, reordering). See log."

    def test_active_logger_filter(self):
        self.set_default_filters()
        try:
            cmd_events = self.api.get_event_pred(severity=EventSeverity.COMMAND)
            actHI_events = self.api.get_event_pred(severity=EventSeverity.ACTIVITY_HI)
            pred = predicates.greater_than(0)
            zero = predicates.equal_to(0)
            # Drain time for dispatch events
            time.sleep(10)

            self.assert_command("cmdDisp.CMD_NO_OP")
            self.assert_command("cmdDisp.CMD_NO_OP")

            time.sleep(0.5)

            self.api.assert_event_count(pred, cmd_events)
            self.api.assert_event_count(pred, actHI_events)

            self.set_event_filter(self.FilterSeverity.COMMAND, False)
            # Drain time for dispatch events
            time.sleep(10)
            self.api.clear_histories()
            self.api.send_command("cmdDisp.CMD_NO_OP")
            self.api.send_command("cmdDisp.CMD_NO_OP")

            time.sleep(0.5)

            self.api.assert_event_count(zero, cmd_events)
            self.api.assert_event_count(pred, actHI_events)
        finally:
            self.set_default_filters()

    def test_seqgen(self):
        """Tests the seqgen code"""
        sequence = os.path.join(os.path.dirname(__file__), "test_seq.seq")
        assert (
            subprocess.run(
                [
                    "fprime-seqgen",
                    "-d",
                    self.dictionary,
                    sequence,
                    "/tmp/ref_test_int.bin",
                ]
            ).returncode
            == 0
        ), "Failed to run fprime-seqgen"
        self.assert_command(
            "cmdSeq.CS_RUN", args=["/tmp/ref_test_int.bin", "SEQ_BLOCK"], max_delay=5
        )
