import os
import sys
import time
from enum import Enum

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../Gds/src")
fprimeName = os.path.join(filename, "../../Fw/Python/src")
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
        path = os.path.join(filename, "../Top/RefTopologyAppDictionary.xml")
        cls.pipeline.setup(config, path)
        cls.pipeline.connect("127.0.0.1", 50000)
        cls.api = IntegrationTestAPI(cls.pipeline, "./")
        cls.case_list = [] # TODO find a better way to do this. 

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
        self.api.log("Starting assert_command helper")
        cmd_id = self.api.translate_command_name(command)
        events = []
        events.append(self.api.get_event_pred("OpCodeDispatched", [cmd_id, None]))
        events.append(self.api.get_event_pred("OpCodeCompleted", [cmd_id]))
        results = self.api.send_and_assert_event(command, args, events)
        if max_delay is not None:
            delay = results[1].get_time() - results[0].get_time()
            msg = "The delay, {}, between the two events should be < {}".format(delay, max_delay)
            assert delay < max_delay, msg
        self.api.log("assert_command helper completed successfully")
        return results

    """
    This enum is includes the values of EventSeverity that can be filtered by the ActiveLogger Component
    """
    FilterSeverity = Enum('FilterSeverity' , 'WARNING_HI WARNING_LO COMMAND ACTIVITY_HI ACTIVITY_LO DIAGNOSTIC')

    def set_event_filter(self, severity, enabled):
        """
        This helper will send a command that updates the given severity filter on the ActiveLogger
        Component in the Ref App.
        Args:
            severity: A valid FilterSeverity Enum Value (str) or an instance of FilterSeverity
            enabled: a boolean of whether or not to enable the given severity
        Return:
            boolean of wether the report filter was set successfully.
        """
        enabled = "ENABLED" if enabled else "DISABLED"
        if isinstance(severity, self.FilterSeverity):
            severity = severity.name
        else:
            severity = self.FilterSeverity[severity].name
        try:
            self.api.send_command("ALOG_SET_EVENT_REPORT_FILTER", ["INPUT_" + severity, "INPUT_" + enabled])
            self.api.send_command("ALOG_SET_EVENT_SEND_FILTER", ["SEND_" + severity, "SEND_" + enabled])
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
        self.assert_command("CMD_NO_OP", max_delay=0.1)
        assert self.api.get_command_test_history().size() == 1
        self.assert_command("CMD_NO_OP", max_delay=0.1)
        assert self.api.get_command_test_history().size() == 2

    def test_send_and_assert_no_op(self):
        length = 5
        failed = 0
        evr_seq = ["OpCodeDispatched", "NoOpReceived", "OpCodeCompleted"]
        any_reordered = False
        dropped = False
        for i in range(0, length):
            results = self.api.send_and_await_event("CMD_NO_OP", events=evr_seq, timeout=2)
            if len(results) < 3:
                items = self.api.get_event_test_history().retrieve()
                last = None
                reordered = False
                for item in items:
                    if last is not None:
                        if item.get_time() < last.get_time():
                            self.api.log("during iteration #{}, a reordered event was detected: {}".format(i, item), "FF9999")
                            any_reordered = True
                            reordered = True
                            break
                    last = item
                if not reordered:
                    self.api.log("during iteration #{}, a dropped event was detected".format(i), "FF9999")
                    dropped = True
                failed += 1
            else:
                self.api.log("Send and assert #{} was successful.".format(i), "ADEBAD")
            self.api.clear_histories()

        if not any_reordered:
            self.api.log("All searched events were ordered.", "ADEBAD")
        else:
            self.api.log("Event messages were reordered.", "FF9999")

        if not dropped:
            self.api.log("No expected events were dropped", "ADEBAD")
        else:
            self.api.log("At least one event was dropped.", "FF9999")

        if failed == 0:
            self.api.log("All {} send and assert sequences were received correctly".format(length), "ADEBAD")
        else:
            msg = "{} out of {} send and assert no_op searches failed".format(failed, length)
            self.api.log(msg, "FF9999")

        assert not any_reordered, "at least one event was received out of order."
        assert not dropped, "at least one event was dropped or not sent."
        assert failed == 0, "at least one iteration failed."

    def test_bd_cycles_ascending(self):
        length = 5
        count_pred = predicates.greater_than(length - 1)
        results = self.api.await_telemetry_count(count_pred, "BD_Cycles", timeout=length)
        last = None
        reordered = False
        ascending = True
        for result in results:
            if last is not None:
                last_time = last.get_time()
                result_time = result.get_time()
                if result_time - last_time > 1.5:
                    msg = "Double buffering may have caused an update to be skipped in FSW between {} and {}".format(last_time.to_readable(), result_time.to_readable())
                    self.api.log(msg, "FFCC99")
                elif result_time < last_time:
                    msg = "There is potential reorder error between {} and {}".format(last_time, result_time)
                    self.api.log(msg, "FF9999")
                    reordered = True

                if not result.get_val() > last.get_val():
                    msg = "There is either a counter rollover, or a potential reorder error between {} and {}".format(result_time, last_time)
                    self.api.log(msg, "FFCC99")
                    ascending = False

            last = result

        if ascending:
            self.api.log("All BD_Cycles updates increased in value", "ADEBAD")
        else:
            self.api.log("Channel updates were potentially reordered. Two non-ascending values were found", "FF9999")

        if not reordered:
            self.api.log("All BD_Cycles updates were ordered.", "ADEBAD")
        else:
            self.api.log("Channel updates were potentially reordered.", "FF9999")

        if count_pred(len(results)):
            self.api.log("The search found all the updates", "ADEBAD")
        else:
            msg = "Double buffering may have caused an update to be skipped in FSW. exp: {} counted: {}".format(length, len(results))
            self.api.log(msg, "FF9999")

        self.api.assert_telemetry_count(0, "RgCycleSlips")
        assert ascending, "Not BD_Cycles increased in value, See test log."
        assert not reordered, "Channel updates were potentially reordered or not ascending. See test log."

    def test_active_logger_filter(self):
        self.set_default_filters()
        try: 
            cmd_events = self.api.get_event_pred(severity=EventSeverity.COMMAND)
            actHI_events = self.api.get_event_pred(severity=EventSeverity.ACTIVITY_HI)
            pred = predicates.greater_than(0)
            zero = predicates.equal_to(0)

            self.assert_command("CMD_NO_OP")
            self.assert_command("CMD_NO_OP")

            time.sleep(3)
            
            self.api.assert_event_count(pred, cmd_events)
            self.api.assert_event_count(pred, actHI_events)
            
            self.set_event_filter(self.FilterSeverity.COMMAND, False)
            self.api.clear_histories()
            self.api.send_command("CMD_NO_OP")
            self.api.send_command("CMD_NO_OP")

            self.api.assert_event_count(zero, cmd_events)
            self.api.assert_event_count(pred, actHI_events)
        finally:
            self.set_default_filters()
