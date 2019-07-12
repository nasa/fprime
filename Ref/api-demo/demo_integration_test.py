import os
import sys

filename = os.path.dirname(__file__)
gdsName = os.path.join(filename, "../../Gds/src")
fprimeName = os.path.join(filename, "../../Fw/Python/src")
sys.path.insert(0, gdsName)
sys.path.insert(0, fprimeName)

from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
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

    @classmethod
    def teardown_class(cls):
        cls.pipeline.disconnect()
        cls.api.teardown()

    def setup_method(self, method):
        self.api.start_test_case(method.__name__)

    def test_is_streaming(self):
        results = self.api.assert_telemetry_count(5, timeout=10)
        for result in results:
            msg = "received channel {} update: {}".format(
                result.get_id(), result.get_str()
            )
            print(msg)

    def test_send_command(self):
        self.api.send_command("CMD_NO_OP", [])
        assert self.api.get_command_test_history().size() == 1
        print(self.api.get_command_test_history()[0])

    def test_send_and_assert_no_op(self):
        evr_seq = ["OpCodeDispatched", "NoOpReceived", "OpCodeCompleted"]
        results = self.api.send_and_assert_event("CMD_NO_OP", events=evr_seq)
        for event in results:
            print("[{}] EVR {}".format(event.get_time().useconds, event))
        results = self.api.send_and_assert_event("CMD_NO_OP", events=evr_seq)
        for event in results:
            print("[{}] EVR {}".format(event.get_time().useconds, event))

    def test_bd_cycles_ascending(self):
        tlm = ["BD_Cycles"] * 10
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

    def test_assert_telemetry(self):
        result = self.api.assert_telemetry("BD_Cycles", timeout=5)
        val_pred = predicates.greater_than(result.get_val())
        result = self.api.assert_telemetry("BD_Cycles", value=val_pred, timeout=5)
        val_pred = predicates.greater_than(result.get_val())
        result = self.api.assert_telemetry("BD_Cycles", value=val_pred, timeout=5)
        val_pred = predicates.greater_than(result.get_val())
        result = self.api.assert_telemetry("BD_Cycles", value=val_pred, timeout=5)
