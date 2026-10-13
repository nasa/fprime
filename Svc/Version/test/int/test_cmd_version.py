"""test_cmd_version.py:


Test the command version with basic integration tests.
"""

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity


def test_send_version_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of version commands.
      version.VERSION, [PROJECT,FRAMEWORK,LIBRARY,CUSTOM,ALL]
      version.ENABLE, [DISABLED/ENABLED]
    """

    # Enable all telemetry packet groups so ProjectVersion/FrameworkVersion are emitted
    fprime_test_api.set_tlm_packet_level(3)

    version = fprime_test_api.get_mnemonic("Svc.Version")
    version_events = fprime_test_api.get_event_pred(
        event=predicates.is_a_member_of(
            [
                fprime_test_api.translate_event_name(version + "." + name)
                for name in [
                    "ProjectVersion",
                    "FrameworkVersion",
                    "LibraryVersions",
                    "CustomVersions",
                ]
            ]
        ),
        severity=EventSeverity.ACTIVITY_LO,
    )
    # Library/custom counts are deployment specific; ALL must report the sum of the parts
    emitted = {}

    for count, value in enumerate(
        ["PROJECT", "FRAMEWORK", "LIBRARY", "CUSTOM", "ALL"], 1
    ):
        if value in ["PROJECT", "FRAMEWORK"]:
            pred = predicates.equal_to(1)
        elif value == "ALL":
            pred = predicates.equal_to(2 + emitted["LIBRARY"] + emitted["CUSTOM"])
        else:
            pred = predicates.greater_than_or_equal_to(0)

        start_tlm = fprime_test_api.get_telemetry_test_history().size()
        start_evr = fprime_test_api.get_event_test_history().size()

        fprime_test_api.send_and_assert_command(
            version + "." + "VERSION",
            [
                value,
            ],
            max_delay=5,
        )

        event_cnt = fprime_test_api.assert_event_count(
            pred, version_events, start=start_evr, timeout=5
        )
        emitted[value] = len(event_cnt)
        # EVR or report MSG_EVR:  2025-07-28T17:36:43.474151: CdhCore.version.ProjectVersion EventSeverity.ACTIVITY_LO : Project Version: [v4.0.0a1-122-g7b6e9a2e1]

        for result in event_cnt:
            msg = "{}".format(result.get_str())
            report_ver_value = msg.split("[")[-1].split("]")[0]

        # Channel (Telemetry) History search found the specified item: 2025-07-28T17:36:43.474180: CdhCore.version.ProjectVersion = v4.0.0a1-122-g7b6e9a2e1
        if count == 1:
            evr_ver = fprime_test_api.await_telemetry(
                fprime_test_api.get_mnemonic("Svc.Version") + "." + "ProjectVersion",
                start=start_tlm,
            )
        elif count == 2:
            evr_ver = fprime_test_api.await_telemetry(
                fprime_test_api.get_mnemonic("Svc.Version") + "." + "FrameworkVersion",
                start=start_tlm,
            )

        if count == 1 or count == 2:
            evr_ver_value = str(evr_ver).split("=")[-1].strip()

            assert (
                evr_ver_value == report_ver_value
            ), f"EVR version {report_ver_value} != telemetry version {evr_ver_value} for {value}"

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Version") + "." + "ENABLE",
        ["ENABLED"],
        max_delay=1,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.Version") + "." + "ENABLE",
        ["DISABLED"],
        max_delay=1,
    )
