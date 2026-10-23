"""test_fpy.py:

Integration test for Fpy, the F Prime sequencing language, on the Ref deployment.

The test compiles test_fpy.fpy (next to this file) to WebAssembly with fprime-fpyc
against the running deployment's dictionary, uplinks the module into the flight
software's working directory, runs it on Ref.wasmSeq (a Svc.WasmSequencer) and
asserts that it runs to completion. The sequence checks itself with Fpy asserts, so
the language coverage lives in the .fpy file; this file only drives it.

To keep unrelated Ref changes from breaking the test, the sequence depends on as
little of the dictionary as possible: the sequencer instance itself, the command
dispatcher, and framework types that every F Prime dictionary carries.
"""

import subprocess
import tempfile
from pathlib import Path

from fprime_gds.common.testing_fw import predicates

SEQUENCER = "Ref.wasmSeq"
SEQUENCE_SOURCE = Path(__file__).parent / "test_fpy.fpy"
# Relative to the flight software's working directory, which FileUplink is sandboxed to
ONBOARD_NAME = "fpy_int_test.wasm"
# The sequence sleeps and polls telemetry on the sequencer's 0.5 Hz timer, and the
# flight software may be running under valgrind
SEQUENCE_TIMEOUT = 180


def compile_sequence(fprime_test_api, source):
    """Compile an Fpy source file to a wasm module and return the module's path"""
    module = Path(tempfile.mkdtemp(prefix="fpy_int_test_")) / f"{source.stem}.wasm"
    result = subprocess.run(
        [
            "fprime-fpyc",
            str(source),
            "--dictionary",
            str(fprime_test_api.dictionaries.dictionary_path),
            "--emit",
            "wasm",
            "--output",
            str(module),
        ],
        capture_output=True,
        text=True,
    )
    assert (
        result.returncode == 0
    ), f"fprime-fpyc failed on {source.name}:\n{result.stdout}\n{result.stderr}"
    return module


def sequencer_events(fprime_test_api, start):
    """The sequencer's events since the given history index, for failure messages"""
    events = fprime_test_api.get_event_test_history().retrieve(start)
    lines = [
        str(event)
        for event in events
        if event.template.get_full_name().startswith(SEQUENCER)
    ]
    return "\n".join([f"Events from {SEQUENCER}:"] + (lines or ["(none)"]))


def test_fpy_sequence(fprime_test_api):
    """Compile, uplink and run the Fpy test sequence on the Ref wasm sequencer

    The sequence exercises variables, arithmetic, control flow, functions, imports,
    commands, telemetry, parameters, checks, sleeps, time and logging and fails
    itself if anything misbehaves. A blocking RUN carries the sequence's verdict:
    it completes OK only if the sequence did.
    """
    module = compile_sequence(fprime_test_api, SEQUENCE_SOURCE)

    fprime_test_api.clear_histories()
    assert fprime_test_api.uplink_file_and_await_completion(
        str(module), destination=ONBOARD_NAME, timeout=30
    ), f"Uplink of {ONBOARD_NAME} did not complete"

    run_command = f"{SEQUENCER}.RUN"
    run_opcode = fprime_test_api.translate_command_name(run_command)
    succeeded = fprime_test_api.get_event_pred(f"{SEQUENCER}.SequenceSucceeded")
    # The sequence never runs other sequences, so an error for RUN's opcode is the
    # ground command failing, which is how a failed sequence reports itself
    failed = fprime_test_api.get_event_pred(
        "CdhCore.cmdDisp.OpCodeError", [run_opcode, None]
    )
    start = fprime_test_api.get_event_test_history().size()
    finished = False
    try:
        fprime_test_api.send_command(run_command, [ONBOARD_NAME, "BLOCK"])
        # find_history_item takes the combined predicate as is; await_event would
        # treat a non-event predicate as an event id predicate
        outcome = fprime_test_api.find_history_item(
            predicates.satisfies_any([succeeded, failed]),
            fprime_test_api.get_event_test_history(),
            start=start,
            timeout=SEQUENCE_TIMEOUT,
        )
        finished = outcome is not None
        assert (
            outcome is not None
        ), f"Sequence did not finish within {SEQUENCE_TIMEOUT}s\n{sequencer_events(fprime_test_api, start)}"
        assert succeeded(
            outcome
        ), f"Sequence failed: {outcome}\n{sequencer_events(fprime_test_api, start)}"
        fprime_test_api.assert_event(
            "CdhCore.cmdDisp.OpCodeCompleted", [run_opcode], start=start, timeout=5
        )

        # The sequence's log() calls reach the ground as the sequencer's guest events,
        # at the requested severities
        for event, message in [
            ("LogActivityHi", "fpy integration sequence complete"),
            ("LogActivityLo", "activity lo from Fpy"),
            ("LogWarningLo", "warning lo from Fpy"),
            ("LogWarningHi", "warning hi from Fpy"),
        ]:
            fprime_test_api.assert_event(
                f"{SEQUENCER}.{event}", [message], start=start, timeout=5
            )
    finally:
        if not finished:
            fprime_test_api.send_command(f"{SEQUENCER}.CANCEL")
        # The flight software owns its working directory; ask it to remove the upload
        fprime_test_api.send_and_assert_command(
            "FileHandling.fileManager.RemoveFile",
            args=[ONBOARD_NAME, True],
            max_delay=5,
        )
