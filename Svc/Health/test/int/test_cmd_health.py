"""test_cmd_health.py:

Test the command dispatcher with basic integration tests.
"""

import time
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity


def test_send_health_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of health commands.

     health.HLTH_ENABLE, [disabled/enabled}
     health.HLTH_PING_ENABLE,["FileManager","DISABLED/ENABLED"]
     health.HLTH_CHNG_PING,["FileManager",1,1]
    """

    cmd_events = fprime_test_api.get_event_pred(severity=EventSeverity.COMMAND)
    actHi_events = fprime_test_api.get_event_pred(severity=EventSeverity.ACTIVITY_HI)
    warnLo_events = fprime_test_api.get_event_pred(severity=EventSeverity.WARNING_LO)

    pred = predicates.greater_than(0)
    zero = predicates.equal_to(0)

    # Expect number still increment after clear_history
    fprime_test_api.clear_histories()  # will clear all history (can read telemetry channel again with latest value.  otherwise still have old value)
    time.sleep(5)

    # Verify disable/enable the Health command
    # Command: Disable the health  HLTH_ENABLE command
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "HLTH_ENABLE", ["DISABLED"]
    )
    assert (
        fprime_test_api.get_command_test_history().size() == 1
    )  # current command count

    fprime_test_api.assert_event_count(pred, cmd_events)
    fprime_test_api.assert_event_count(pred, actHi_events)

    # Command: Enable health Expect number increment HLTH_Enable command
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "HLTH_ENABLE", ["ENABLED"]
    )
    assert (
        fprime_test_api.get_command_test_history().size() == 2
    )  # current command count

    fprime_test_api.assert_event_count(4, cmd_events)  # Verify event command
    fprime_test_api.assert_event_count(2, actHi_events)  # Verify event activity_hi
    time.sleep(5)

    ## Command:  Disabled HLTH_PING_ENABLE command with invalid entry (expected warning_lo)
    # use send_command because No completion (will cause pytest to assert when no completion) use send_command will ignore completion)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "HLTH_PING_ENABLE",
        ["Ref_cmdDispatch", "DISABLED"],
    )
    assert (
        fprime_test_api.get_command_test_history().size() == 3
    )  # current command count

    time.sleep(3)
    cmdErrors = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + "." + "CommandErrors",
        start="NOW",
    )
    # If no constraints are specified on the channels, the predicate will always return true
    param_error = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + "." + "CommandErrors",
        cmdErrors,
    )

    time.sleep(3)
    fprime_test_api.assert_event_count(6, cmd_events)  # Verify event command
    fprime_test_api.assert_event_count(2, actHi_events)  # Verify event actHi
    fprime_test_api.assert_event_count(1, warnLo_events)  # Verify event warning_lo

    ##### Disabled/Enabled HLTH_PING_ENABLE command    (PR_NumPings channel will stop when command disabled) and count increment command is enabled
    # namespace <deployment_name>_health {Svc::Health:PingEntry pingEntries[NUM_PING_ENTRIES ]  (look at <deployment_name>/build-fprime-automatic-native/<deployment_name>/top/<deployment_name>TopologyAc.cpp
    # Get mnemonic of PingReceiver => Ref.pingRcvr (replace . to _ )
    # Notes:
    # After send cmd Ref.pingRcvr.PR_StopPings command the channel Ref.pingRcvr.PR_NumPIngs will stop increment
    #  command health_enable or HLTH_PING_ENABLE (enable option) still not restart this channel ???
    #
    # Try append_file 1MiB.txt multi time trigger
    # Ref.health.HLTH_PING_WARN   WARNING_HI   Ping entry Ref_fileManager late warning

    # Verify can disable/enable ping for a particular set of components
    # threshold warningValue and Threshold fatalValue
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "HLTH_PING_ENABLE",
        [fprime_test_api.get_mnemonic("Svc.FileManager").replace(".", "_"), "DISABLED"],
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "HLTH_CHNG_PING",
        [fprime_test_api.get_mnemonic("Svc.FileManager").replace(".", "_"), 1, 1],
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "HLTH_PING_ENABLE",
        [fprime_test_api.get_mnemonic("Svc.FileManager").replace(".", "_"), "ENABLED"],
    )
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/1MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(50)
    # expected trigger warning hi
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/2MiB.txt", "/tmp/2MiB.txt"],
    )
    time.sleep(120)

    # If no constraints are specified on the channels, the predicate will always return true        # confirm PingLateWarnings
    WarnHi_error = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Svc.Health") + "." + "PingLateWarnings", 1
    )
    fprime_test_api.assert_telemetry(WarnHi_error, timeout=5)

    # Cleanup directory
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveFile",
        ["/tmp/2MiB.txt", True],
    )
