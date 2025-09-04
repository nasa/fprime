"""test_parameter.py:

Test the command dispatcher with basic integration tests.
"""


def test_send_parameter(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.

    recvBuffComp.PARAMETER1_PRM_SET  (call telemetry confirm value)
    recvBuffComp.PARAMETER1_PRM_SAVE
    recvBuffComp.PARAMETER2_PRM_SET  (call telemetry confirm value)
    recvBuffComp.PARAMETER2_PRM_SAVE

    sendBuffComp.PARAMETER3_PRM_SET  (call telemetry confirm value)
    sendBuffComp.PARAMETER3_PRM_SAVE
    sendBuffComp.PARAMETER4_PRM_SET  (call telemetry confirm value)
    sendBuffComp.PARAMETER4_PRM_SAVE

    Notes: send a parameterX_set,value=current telemetry channel.
       the function assert_telemetry will fail because no_change
    """

    ## setup default-value
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "PARAMETER1_PRM_SET",
        [1],
        max_delay=5,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "PARAMETER2_PRM_SET",
        [2],
        max_delay=5,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "PARAMETER3_PRM_SET",
        [3],
        max_delay=5,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "PARAMETER4_PRM_SET",
        [4],
        max_delay=5,
    )

    # Only work if send command PARAMETER1_PRM_SET then check telemetry. Unsigned integer 0..4294967295)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "PARAMETER1_PRM_SET",
        [10],
        max_delay=5,
    )

    # Check Telem only will not work
    param1_change = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "Parameter1", 10
    )
    fprime_test_api.assert_telemetry(param1_change, timeout=5)

    # Send PARAMETER1_PRM_SAVE
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "PARAMETER1_PRM_SAVE",
        max_delay=1,
    )

    # Send PARAMETER2_PRM_SET (confirm new value / SAVE      ) signed integer -32867 and 32767
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "PARAMETER2_PRM_SET",
        [20],
        max_delay=5,
    )

    param2_change = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "Parameter2", 20
    )
    fprime_test_api.assert_telemetry(param2_change, timeout=5)

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.RecvBuff") + "." + "PARAMETER2_PRM_SAVE",
        max_delay=5,
    )

    # Send PARAMETER3_PRM_SET (confirm new value / SAVE      ) unsigned integer 0..255
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "PARAMETER3_PRM_SET",
        [30],
        max_delay=5,
    )

    param3_change = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "Parameter3", 30
    )
    fprime_test_api.assert_telemetry(param3_change, timeout=5)

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "PARAMETER3_PRM_SAVE",
        max_delay=5,
    )

    # Send PARAMETER4_PRM_SET (confirm new value / SAVE      ) float
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "PARAMETER4_PRM_SET",
        [40],
        max_delay=5,
    )

    param4_change = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "Parameter4", 40
    )
    fprime_test_api.assert_telemetry(param4_change, timeout=5)

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Ref.SendBuff") + "." + "PARAMETER4_PRM_SAVE",
        max_delay=5,
    )

    # Notes: send a xxx_set,value=current will not update telemetry channel.  call assert_telemetry will fail
