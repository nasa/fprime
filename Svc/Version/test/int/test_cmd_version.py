"""test_cmd_version.py:


Test the command version with basic integration tests.
"""

from enum import Enum
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity


"""
This enum is includes the values of EventSeverity that can be filtered by the ActiveLogger Component
"""
FilterSeverity = Enum(
    "FilterSeverity",
    "WARNING_HI WARNING_LO COMMAND ACTIVITY_HI ACTIVITY_LO DIAGNOSTIC",
)


def test_send_version_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of version commands.
      version.VERSION, [PROJECT,FRAMEWORK,LIBRARY,CUSTOM,ALL]
      version.ENABLE, [DISABLED/ENABLED]
    """

    for count, value in enumerate(
        ["PROJECT", "FRAMEWORK", "LIBRARY", "CUSTOM", "ALL"], 1
    ):

        pred = predicates.greater_than(0)

        ActLo_events = fprime_test_api.get_event_pred(
            severity=EventSeverity.ACTIVITY_LO
        )

        results2 = fprime_test_api.send_and_assert_command(
            fprime_test_api.get_mnemonic("Svc.Version") + "." + "VERSION",
            [
                value,
            ],
            max_delay=5,
            #            events=events,
        )

        event_cnt = fprime_test_api.assert_event_count(pred, ActLo_events)
        # EVR or report MSG_EVR:  2025-07-28T17:36:43.474151: CdhCore.version.ProjectVersion EventSeverity.ACTIVITY_LO : Project Version: [v4.0.0a1-122-g7b6e9a2e1]

        for result in event_cnt:
            msg = "{}".format(result.get_str())
            msg_list = str(msg).split()
            version = msg_list[6].replace("[", "")
            report_ver_value = version.replace("]", "")

        # Channel (Telemetry) History search found the specified item: 2025-07-28T17:36:43.474180: CdhCore.version.ProjectVersion = v4.0.0a1-122-g7b6e9a2e1
        if count == 1:
            evr_ver = fprime_test_api.await_telemetry(
                fprime_test_api.get_mnemonic("Svc.Version") + "." + "ProjectVersion",
                start="NOW",
            )
        elif count == 2:
            evr_ver = fprime_test_api.await_telemetry(
                fprime_test_api.get_mnemonic("Svc.Version") + "." + "FrameworkVersion",
                start="NOW",
            )

        if count == 1 or count == 2:
            evr_ver_list = str(evr_ver).split()
            evr_ver_value = evr_ver_list[3]

            if evr_ver_value == report_ver_value:
                print(
                    "COMPARE EVR vs. TELEMETRY channel and version cmd_option PASS ",
                    evr_ver_value,
                    report_ver_value,
                    value,
                )
            else:
                print(
                    "COMPARE EVR vs. TELEMETRY channel and version cmd_option FAIL ",
                    evr_ver_value,
                    report_ver_value,
                    value,
                )

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
