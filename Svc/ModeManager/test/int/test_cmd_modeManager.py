"""test_cmd_modeManager.py:

Reusable integration tests for Svc::ModeManager. Exercises the component
through the GDS over the real communication stack, against the shipped
default policy (unrestricted): every transition should succeed unless the
target has no entry in the deployment's ModeMachine, in which case it must
be rejected without ever asserting.

These tests are portable across deployments: fprime_test_api.get_mnemonic()
resolves "Svc.ModeManager" via each deployment's own int_config.json rather
than a hardcoded instance name.
"""

from fprime_gds.common.utils.event_severity import EventSeverity


def test_start_command(fprime_test_api):
    """START must leave the initial state and enter IDLE.

    Covers: MM-003, MM-004, MM-008, MM-009, MM-016, MM-017
    """
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "START",
        [],
        max_delay=5,
    )

    fprime_test_api.assert_event(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "ModeTransitioned",
        [None, "IDLE", "GROUND", None],
        severity=EventSeverity.ACTIVITY_HI,
        timeout=5,
    )

    fprime_test_api.assert_telemetry(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "CurrentMode",
        "IDLE",
        timeout=5,
    )


def test_request_mode_command(fprime_test_api):
    """REQUEST_MODE must be permitted by the default policy and stamp
    GROUND provenance.

    Covers: MM-001, MM-005, MM-007, MM-009, MM-015, MM-016, MM-017
    """
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "REQUEST_MODE",
        ["SCIENCE"],
        max_delay=5,
    )

    fprime_test_api.assert_event(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "ModeTransitioned",
        [None, "SCIENCE", "GROUND", None],
        severity=EventSeverity.ACTIVITY_HI,
        timeout=5,
    )

    fprime_test_api.assert_telemetry(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "CurrentMode",
        "SCIENCE",
        timeout=5,
    )


def test_request_unsupported_target_rejected(fprime_test_api):
    """Requesting STARTUP has no entry in ModeMachine and must be rejected
    before any policy is consulted, not asserted on. The command still
    completes: a rejected transition is not a command failure.

    Covers: MM-005
    """
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "REQUEST_MODE",
        ["STARTUP"],
        max_delay=5,
    )

    fprime_test_api.assert_event(
        fprime_test_api.get_mnemonic("Svc.ModeManager") + "." + "TransitionRejected",
        [None, "STARTUP", "GROUND", None, "DEFAULT"],
        severity=EventSeverity.WARNING_LO,
        timeout=5,
    )
