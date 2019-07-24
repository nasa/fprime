import os
import sys

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../Gds/src")
fprimeName = os.path.join(filename, "../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.config_manager import ConfigManager


class TestRefAppClass(object):

    @classmethod
    def setup_class(cls):
        cls.pipeline = StandardPipeline()
        config = ConfigManager()
        filename = os.path.dirname(__file__)
        path = os.path.join(filename, "../Top/RefTopologyAppDictionary.xml")
        cls.pipeline.setup(config, path)
        cls.pipeline.connect("127.0.0.1", 50000)
        cls.api = IntegrationTestAPI(cls.pipeline, "./demo_log.xlsx")
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
        """
        self.api.log_test_message("Starting assert_command helper")
        cmd_id = self.api.translate_command_name(command)
        events = []
        events.append(self.api.get_event_predicate("OpCodeDispatched", [cmd_id, None]))
        events.append(self.api.get_event_predicate("OpCodeCompleted", [cmd_id]))
        results = self.api.send_and_assert_event(command, args, events)
        if max_delay is not None:
            delay = results[1].get_time() - results[0].get_time()
            msg = "The delay, {}, between the two events should be below {}".format(delay, max_delay)
            assert delay < max_delay, msg
        self.api.log_test_message("assert_command helper completed successfully")
        return results

    def test_send_command(self):
        self.assert_command("CMD_NO_OP", max_delay=0.0001)
        assert self.api.get_command_test_history().size() == 1
        self.assert_command("CMD_NO_OP", max_delay=0.0001)
        assert self.api.get_command_test_history().size() == 2

    def test_send_and_assert_no_op(self):
        evr_seq = ["OpCodeDispatched", "NoOpReceived", "OpCodeCompleted"]
        results = self.api.send_and_assert_event("CMD_NO_OP", events=evr_seq)
        for event in results:
            print("[{}] EVR {}".format(event.get_time().useconds, event))
        results = self.api.send_and_assert_event("CMD_NO_OP", events=evr_seq)
        for event in results:
            print("[{}] EVR {}".format(event.get_time().useconds, event))

    def test_bd_cycles_ascending(self):
        tlm = ["BD_Cycles"] * 3
        results = self.api.assert_telemetry_sequence(tlm, timeout=20)
        last = 0
        for tlm in results:
            msg = "[{}] EVR {}".format(tlm.get_time().useconds, tlm)
            print(msg)
            recent = tlm.get_val()
            msg = "BD_Cycles should increase, but {} is not greater than {}.".format(
                recent, last
            )
            assert recent > last, msg
            last = recent