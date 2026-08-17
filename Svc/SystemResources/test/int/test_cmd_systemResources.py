"""test_cmd_systemResources.py:

Test the command dispatcher with basic integration tests.
"""

import time


def test_send_systemResources_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.

    SystemResources.Enable, <Disabled> (read SystemResources telemetry confirm value stale or stop)
    SystemResources.Enable, <Enabled>  (read SystemResources telemetry confirm value changing)

    """
    mnemonic = fprime_test_api.get_mnemonic("Svc.SystemResources")

    def await_channel(channel):
        result = fprime_test_api.await_telemetry(
            f"{mnemonic}.{channel}", start="NOW", timeout=10
        )
        assert result is not None, f"Timed out awaiting {channel} telemetry"
        return result.get_val()

    # Verify memory and non-volatile usage report at least 1 KB
    assert await_channel("MEMORY_TOTAL") >= 1
    assert await_channel("MEMORY_USED") >= 1
    assert await_channel("NON_VOLATILE_TOTAL") >= 1
    assert await_channel("NON_VOLATILE_FREE") >= 1

    # Verify CPU utilization channels report valid percentages
    for channel in ["CPU", "CPU_00", "CPU_01", "CPU_02", "CPU_03"]:
        value = await_channel(channel)
        assert 0.0 <= value <= 100.0, f"{channel} reported invalid percentage {value}"

    # Confirm CPU telemetry is flowing before disabling
    await_channel("CPU")

    ##### Command Disabled SystemResources.ENABLE command (DISABLED)
    fprime_test_api.send_and_assert_command(f"{mnemonic}.ENABLE", ["DISABLED"])

    # Allow in-flight telemetry to drain, then verify no new CPU samples arrive
    time.sleep(2)
    stale = fprime_test_api.await_telemetry(f"{mnemonic}.CPU", start="NOW", timeout=5)
    assert stale is None, "CPU telemetry still updating while DISABLED"

    ##### Command Enabled SystemResources.ENABLE command (ENABLED)
    fprime_test_api.send_and_assert_command(f"{mnemonic}.ENABLE", ["ENABLED"])

    # Verify CPU telemetry resumes after re-enable
    await_channel("CPU")
