"""test_cdh_core.py:

Test the core functionality of the CdhCore subtopology with:
1. Commands
2. Events
3. Telemetry channels
"""

import random
from fprime_gds.common.testing_fw.api import IntegrationTestAPI


def test_basic_command_and_event(fprime_test_api: IntegrationTestAPI):
    """Test that we can send a command and receive the expected event in response"""

    # Send NO_OP command to FSW and wait for expected event
    fprime_test_api.send_and_assert_command(
        f"{fprime_test_api.get_mnemonic('Svc.CommandDispatcher')}.CMD_NO_OP",
    )


def test_command_and_event_with_string_arg(fprime_test_api: IntegrationTestAPI):
    """Test that we can send a command with arguments and receive the expected event with args in response"""

    TEST_STRING = f"test string {random.random()}"

    test_event = fprime_test_api.get_event_pred("NoOpStringReceived", [TEST_STRING])

    # Send NO_OP command to FSW and wait for expected event
    fprime_test_api.send_and_assert_event(
        f"{fprime_test_api.get_mnemonic('Svc.CommandDispatcher')}.CMD_NO_OP_STRING",
        [TEST_STRING],
        events=[test_event],
        timeout=2,
    )


def test_command_and_event_with_many_args(fprime_test_api: IntegrationTestAPI):
    """Test that we can send a command with arguments and receive the expected event with args in response"""

    # types are (I32, F32, U8) - random float precision is finnicky, so just use a fixed value
    TEST_ARGS = [
        random.randint(-(2 ** 31), 2 ** 31 - 1),
        1.5,
        random.randint(0, 2 ** 8 - 1),
    ]

    test_event = fprime_test_api.get_event_pred("TestCmd1Args", TEST_ARGS)

    # Send CMD_1 (no-op with args) command to FSW and wait for expected event
    fprime_test_api.send_and_assert_event(
        f"{fprime_test_api.get_mnemonic('Svc.CommandDispatcher')}.CMD_TEST_CMD_1",
        TEST_ARGS,
        events=[test_event],
        timeout=3,
    )


def test_telemetry_update(fprime_test_api: IntegrationTestAPI):
    """Test that we can receive telemetry updates with expected values"""

    # Enable all telemetry packet groups so CommandsDispatched is emitted
    fprime_test_api.set_tlm_packet_level(3)

    cmd_dispatched_channel = fprime_test_api.get_telemetry_pred("CommandsDispatched")

    # Wait for telemetry update with expected values
    begin_result = fprime_test_api.await_telemetry(cmd_dispatched_channel, timeout=3)
    begin_tlm_val = begin_result.val_obj.val

    # Send no op to increase the count of commands dispatched
    end_result = fprime_test_api.send_and_await_telemetry(
        f"{fprime_test_api.get_mnemonic('Svc.CommandDispatcher')}.CMD_NO_OP",
        channels=cmd_dispatched_channel,
        timeout=3,
    )
    # Assert that the telemetry value has increased by 1 after sending the command
    assert end_result.val_obj.val == begin_tlm_val + 1
