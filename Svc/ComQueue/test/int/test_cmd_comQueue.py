"""test_cmd_comQueue.py:

Integration tests for Svc.ComQueue commands.
    comQueue.FLUSH_QUEUE
    comQueue.SET_QUEUE_PRIORITY

The deployment configuration must map "Svc.ComQueue" to the ComQueue instance mnemonic.
Indices assume the default `ComQueueComPorts = 2` / `ComQueueBufferPorts = 1` from AcConstants.fpp.
"""

COM_PORT_COUNT = 2
BUFFER_PORT_COUNT = 1


def _send_and_assert_validation_error(fprime_test_api, command, args):
    """Send a command and assert the dispatcher reports VALIDATION_ERROR for it"""
    cmd_id = fprime_test_api.translate_command_name(command)
    error = fprime_test_api.get_event_pred("cmdDisp.OpCodeError", [cmd_id, "VALIDATION_ERROR"])
    fprime_test_api.send_and_assert_event(command, args, [error], timeout=5)


def test_flush_queue_valid_indices(fprime_test_api):
    """FLUSH_QUEUE succeeds for every in-range COM_QUEUE and BUFFER_QUEUE index"""
    command = fprime_test_api.get_mnemonic("Svc.ComQueue", "FLUSH_QUEUE")
    for index in range(COM_PORT_COUNT):
        fprime_test_api.send_and_assert_command(command, ["COM_QUEUE", index], max_delay=5)
    for index in range(BUFFER_PORT_COUNT):
        fprime_test_api.send_and_assert_command(command, ["BUFFER_QUEUE", index], max_delay=5)


def test_flush_queue_invalid_indices(fprime_test_api):
    """FLUSH_QUEUE rejects indices outside the per-type range instead of folding onto another queue type"""
    command = fprime_test_api.get_mnemonic("Svc.ComQueue", "FLUSH_QUEUE")
    for index in (COM_PORT_COUNT, COM_PORT_COUNT + BUFFER_PORT_COUNT, -1):
        _send_and_assert_validation_error(fprime_test_api, command, ["COM_QUEUE", index])
    for index in (BUFFER_PORT_COUNT, COM_PORT_COUNT + BUFFER_PORT_COUNT, -1):
        _send_and_assert_validation_error(fprime_test_api, command, ["BUFFER_QUEUE", index])


def test_set_queue_priority_invalid_indices(fprime_test_api):
    """SET_QUEUE_PRIORITY rejects indices outside the per-type range and emits no QueuePriorityChanged event"""
    command = fprime_test_api.get_mnemonic("Svc.ComQueue", "SET_QUEUE_PRIORITY")
    changed = fprime_test_api.get_event_pred(
        fprime_test_api.get_mnemonic("Svc.ComQueue", "QueuePriorityChanged")
    )
    start = fprime_test_api.get_event_test_history().size()
    for index in (COM_PORT_COUNT, COM_PORT_COUNT + BUFFER_PORT_COUNT, -1):
        _send_and_assert_validation_error(fprime_test_api, command, ["COM_QUEUE", index, 0])
    for index in (BUFFER_PORT_COUNT, COM_PORT_COUNT + BUFFER_PORT_COUNT, -1):
        _send_and_assert_validation_error(fprime_test_api, command, ["BUFFER_QUEUE", index, 0])
    fprime_test_api.assert_event_count(0, changed, start=start)


def test_set_queue_priority_valid_index(fprime_test_api):
    """SET_QUEUE_PRIORITY applies to an in-range BUFFER_QUEUE index and emits QueuePriorityChanged

    The queue is set to priority 0 and then back to 1, the priority the ComCcsds subtopology assigns to its file queue.
    """
    command = fprime_test_api.get_mnemonic("Svc.ComQueue", "SET_QUEUE_PRIORITY")
    changed = fprime_test_api.get_mnemonic("Svc.ComQueue", "QueuePriorityChanged")
    last_index = BUFFER_PORT_COUNT - 1
    for priority in (0, 1):
        fprime_test_api.send_and_assert_command(
            command,
            ["BUFFER_QUEUE", last_index, priority],
            max_delay=5,
            events=[fprime_test_api.get_event_pred(changed, ["BUFFER_QUEUE", last_index, priority])],
        )
