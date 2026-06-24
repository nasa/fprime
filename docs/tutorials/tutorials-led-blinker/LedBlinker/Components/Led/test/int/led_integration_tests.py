import time

from fprime_gds.common.testing_fw import predicates


def test_blinking(fprime_test_api):
    """Test that LED component can respond to ground commands"""

    # Send command to enable blinking, then assert expected events are emitted
    blink_start_evr = fprime_test_api.get_event_pred(
        "LedBlinker.led.SetBlinkingState", ["ON"]
    )
    led_on_evr = fprime_test_api.get_event_pred("LedBlinker.led.LedState", ["ON"])
    led_off_evr = fprime_test_api.get_event_pred("LedBlinker.led.LedState", ["OFF"])

    fprime_test_api.send_and_assert_event(
        "LedBlinker.led.BLINKING_ON_OFF",
        args=["ON"],
        events=[blink_start_evr, led_on_evr, led_off_evr, led_on_evr],
        timeout=5,
    )

    # Assert that blink command sets blinking state on
    blink_state_on_tlm = fprime_test_api.get_telemetry_pred(
        "LedBlinker.led.BlinkingState", "ON"
    )
    fprime_test_api.assert_telemetry(blink_state_on_tlm)

    # Assert that the LedTransitions channel increments
    results = fprime_test_api.assert_telemetry_count(
        predicates.greater_than(2), "LedBlinker.led.LedTransitions", timeout=4
    )
    ascending = True
    prev = None
    for res in results:
        if prev is not None:
            if not res.get_val() > prev.get_val():
                ascending = False
                fprime_test_api.log(
                    f"LedBlinker.led.LedTransitions not in ascending order: First ({prev.get_val()}) Second ({res.get_val()})"
                )
        prev = res
    assert fprime_test_api.test_assert(
        ascending, "Expected all LedBlinker.led.LedTransitions updates to ascend.", True
    )

    # Send command to stop blinking, then assert blinking stops
    blink_stop_evr = fprime_test_api.get_event_pred(
        "LedBlinker.led.SetBlinkingState", ["OFF"]
    )
    fprime_test_api.send_and_assert_event(
        "LedBlinker.led.BLINKING_ON_OFF", args=["OFF"], events=[blink_stop_evr]
    )
    time.sleep(1)  # Wait one second to let any in-progress telemetry be sent
    # Save reference to current telemetry history so we can search against future telemetry
    telem_after_blink_off = fprime_test_api.telemetry_history.size()
    time.sleep(2)  # Wait to receive telemetry after stopping blinking
    # Assert that blinking has stopped and that LedTransitions is no longer updating
    fprime_test_api.assert_telemetry_count(
        0, "LedBlinker.led.LedTransitions", start=telem_after_blink_off
    )

    # Assert that blink command sets blinking state off
    blink_state_off_tlm = fprime_test_api.get_telemetry_pred(
        "LedBlinker.led.BlinkingState", "OFF"
    )
    fprime_test_api.assert_telemetry(blink_state_off_tlm, timeout=2)
