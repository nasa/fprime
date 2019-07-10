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
        path = os.path.join(filename, '../Top/RefTopologyAppDictionary.xml')
        cls.pipeline.setup(config, path)
        cls.pipeline.connect('127.0.0.1', 50000)
        cls.api = IntegrationTestAPI(cls.pipeline)

    @classmethod
    def teardown_class(cls):
        cls.pipeline.disconnect()

    def setup_method(self, method):
        self.api.start_test_case(method.__name__)

    def test_is_streaming(self):
        pred = predicates.greater_than_or_equal_to(5)
        results = self.api.assert_telemetry_count(pred, timeout=10)
        for result in results:
            print("received channel update: {}".format(result.get_str()))

    def test_send_command(self):
        self.api.send_command("CMD_NO_OP", [])
        assert self.api.get_command_test_history().size() == 1
        print(self.api.get_command_test_history()[0])

    def test_send_and_assert_no_op(self):
        evrs = ["OpCodeDispatched", "NoOpReceived", "OpCodeCompleted"]
        results = self.api.send_and_assert_event("CMD_NO_OP", events=evrs)
        for event in results:
            print(event)
