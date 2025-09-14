"""test_cmd_EventManager.py:

Test the command EventManager with basic integration tests.
    eventLogger.DUMP_FILTER_STATE
    eventLogger.SET_ID_FILTER
    eventLogger.SET_EVENT_FILTER
"""


def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send_and_assert command events level and state
    """
    # Expected True for (WARNING_HI, WARNING_LO, COMMAND, ACTIVITY_HI, ACTIVITY_LO &  False for DIAGNOSTIC)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "DUMP_FILTER_STATE",
        max_delay=1,
    )

    # EVR Display => Event Id: 0x507, Event Severity: ACTIVITY_HI, Event Description: Received a NO-OP command
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + "." + "CMD_NO_OP",
        max_delay=0.1,
    )

    # enable specific evr Id 0x507 or dec 1287
    # EVR Display => Event Id: 0xb01, Event Severity: ACTIVITY_HI, Event Description: ID 1287 is filtered.
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "SET_ID_FILTER",
        ["0x507", "ENABLED"],
        max_delay=1,
    )

    # send noop command.
    # Confirm SET_ID_FILTER: NO evr specific Event Id 0x507 report due SET_ID_FILTER is enable
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + "." + "CMD_NO_OP",
        max_delay=0.1,
    )

    # dump_filter_state, Expected True for (WARNING_HI, WARNING_LO, COMMAND, ACTIVITY_HI, ACTIVITY_LO) & False for DIAGNOSTIC)
    # Confirm DUMP_FILTER_STATE activity_hi work: include set_id filter enable (activity_hi)
    # EVR Display => Event Id: 0xb01, Event Severity: ACTIVITY_HI, Event Description: ID 1287 is filtered.
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "DUMP_FILTER_STATE",
        max_delay=1,
    )

    ### Disable all ACTIVITY_HI
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "SET_EVENT_FILTER",
        ["ACTIVITY_HI", "DISABLED"],
        max_delay=1,
    )

    # dump_filter_state, Expected True for (WARNING_HI, WARNING_LO, COMMAND, ACTIVITY_LO) & False for DIAGNOSTIC, ACTIVITY_HI)
    # Confirm SET_EVENT_FILTER work: Not include any activity_hi,
    #  even specific Id 0x507 still set_id_filter enable
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "DUMP_FILTER_STATE",
        max_delay=1,
    )

    ## Put back default value
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "SET_EVENT_FILTER",
        ["ACTIVITY_HI", "ENABLED"],
        max_delay=1,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "SET_ID_FILTER",
        ["0x507", "DISABLED"],
        max_delay=1,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.EventManager") + "." + "DUMP_FILTER_STATE",
        max_delay=1,
    )
