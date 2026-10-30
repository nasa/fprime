"""test_retransmit_dpCatalog.py:

Reusable integration tests for the DpCatalog RETRANSMIT_DP command, which reschedules one transmitted data
product for downlink. The product identity (container id, time seconds, time subseconds) is taken from the
canonical file name Dp_<id>_<tSec>_<tSub>.fdp (decimal fields) that DpWriter reports in FileWritten.

Producing a data product is deployment specific, so the producer command is read from the deployment
configuration as in test_insert_dpCatalog.py. These tests are skipped when no producer is configured.
"""

import re

from test_insert_dpCatalog import (
    XMIT_TIMEOUT,
    dp_catalog,
    reset_catalog,
    send_dp_catalog_command,
    transmit_catalog_until_complete,
    write_product,
)

DP_FILE_NAME = re.compile(r"Dp_(\d+)_(\d+)_(\d+)\.fdp$")


def identity(file_name):
    """(id, tSec, tSub) of a data product from its canonical file name"""
    match = DP_FILE_NAME.search(file_name)
    assert match, f"{file_name} is not a canonical data product file name"
    return int(match.group(1)), int(match.group(2)), int(match.group(3))


def write_and_transmit(fprime_test_api):
    """Write one product into the built catalog and downlink it; return its file name"""
    start = fprime_test_api.get_event_test_history().size()
    file_name = write_product(fprime_test_api)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpFileAdded"), [file_name], start=start, timeout=5
    )
    start = fprime_test_api.get_event_test_history().size()
    transmit_catalog_until_complete(fprime_test_api, remain_active=False)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "ProductComplete"), [file_name, None, None], start=start
    )
    return file_name


def retransmit(fprime_test_api, file_name):
    """Send RETRANSMIT_DP for the product of file_name and return the history index before it"""
    start = fprime_test_api.get_event_test_history().size()
    send_dp_catalog_command(fprime_test_api, "RETRANSMIT_DP", list(identity(file_name)))
    return start


def retransmit_rejected(fprime_test_api, dp_id, t_sec, t_sub, reason):
    """Send RETRANSMIT_DP and check it fails with EXECUTION_ERROR and DpRetransmitError naming reason"""
    start = fprime_test_api.get_event_test_history().size()
    fprime_test_api.send_command(dp_catalog(fprime_test_api, "RETRANSMIT_DP"), [dp_id, t_sec, t_sub])
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpRetransmitError"),
        [dp_id, t_sec, t_sub, reason],
        start=start,
        timeout=10,
    )
    op_code_error = fprime_test_api.assert_event(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + ".OpCodeError",
        start=start,
        timeout=10,
    )
    assert op_code_error.get_args()[1].val == "EXECUTION_ERROR"


def test_retransmit_transmitted_product(fprime_test_api):
    """A transmitted product is rescheduled at its header priority and downlinked again by the next
    START_XMIT_CATALOG; afterwards a rebuild skips it again

    Covers: SVC-DPCAT-023, SVC-DPCAT-025
    """
    reset_catalog(fprime_test_api)
    file_name = write_and_transmit(fprime_test_api)

    start = retransmit(fprime_test_api, file_name)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpRetransmitScheduled"), [file_name, None], start=start, timeout=5
    )
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpFileAdded"), [file_name], start=start, timeout=5
    )
    # remainActive was not set, so the product waits for the next transmit command
    fprime_test_api.assert_event_count(
        0, dp_catalog(fprime_test_api, "SendingProduct"), start=start
    )

    start = fprime_test_api.get_event_test_history().size()
    transmit_catalog_until_complete(fprime_test_api, remain_active=False)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "ProductComplete"), [file_name, None, None], start=start
    )

    # transmitted again: the rebuild skips it and a transmission has nothing to send
    start = fprime_test_api.get_event_test_history().size()
    send_dp_catalog_command(fprime_test_api, "BUILD_CATALOG")
    fprime_test_api.assert_event_count(
        0, dp_catalog(fprime_test_api, "DpFileAdded"), start=start
    )
    transmit_catalog_until_complete(fprime_test_api, remain_active=False)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "CatalogXmitCompleted"), [0], start=start
    )


def test_retransmit_survives_rebuild(fprime_test_api):
    """The reset is recorded in the state file: a BUILD_CATALOG after RETRANSMIT_DP catalogs the product
    instead of skipping it, and the next transmission sends it

    Covers: SVC-DPCAT-023
    """
    reset_catalog(fprime_test_api)
    file_name = write_and_transmit(fprime_test_api)

    start = retransmit(fprime_test_api, file_name)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpRetransmitScheduled"), [file_name, None], start=start, timeout=5
    )

    start = fprime_test_api.get_event_test_history().size()
    send_dp_catalog_command(fprime_test_api, "BUILD_CATALOG")
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "CatalogBuildComplete"), start=start, timeout=10
    )
    transmit_catalog_until_complete(fprime_test_api, remain_active=False)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "ProductComplete"),
        [file_name, None, None],
        start=start,
        timeout=XMIT_TIMEOUT,
    )


def test_retransmit_rejected(fprime_test_api):
    """A pending product and an unknown product are rejected with DpRetransmitError and EXECUTION_ERROR

    Covers: SVC-DPCAT-024
    """
    reset_catalog(fprime_test_api)

    start = fprime_test_api.get_event_test_history().size()
    file_name = write_product(fprime_test_api)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpFileAdded"), [file_name], start=start, timeout=5
    )
    dp_id, t_sec, t_sub = identity(file_name)

    # pending: the product has not been transmitted
    retransmit_rejected(fprime_test_api, dp_id, t_sec, t_sub, "PENDING")

    # unknown: no such file in a managed directory
    retransmit_rejected(fprime_test_api, dp_id, t_sec + 1, t_sub, "NOT_FOUND")

    # the pending product is untouched and is sent normally
    start = fprime_test_api.get_event_test_history().size()
    transmit_catalog_until_complete(fprime_test_api, remain_active=False)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "ProductComplete"), [file_name, None, None], start=start
    )
