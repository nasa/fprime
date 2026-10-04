"""test_insert_dpCatalog.py:

Reusable integration tests for DpCatalog runtime insertion (SVC-DPCAT-009): a data product written by
DpWriter is reported on `addToCat` and, when the catalog is built, inserted and downlinked without a
rebuild.

Producing a data product is deployment specific, so the producer command is read from the deployment
configuration file given with --deployment-config, for example:

    "Svc.DpCatalog.producer.command": "Ref.dpDemo.Dp",
    "Svc.DpCatalog.producer.args": ["IMMEDIATE", 10, "PROC_TYPE_NONE"]

The command must result in exactly one data product file written by `Svc.DpWriter` within a few seconds.
These tests are skipped when no producer is configured.
"""

import pytest

PRODUCER_COMMAND_KEY = "Svc.DpCatalog.producer.command"
PRODUCER_ARGS_KEY = "Svc.DpCatalog.producer.args"

# Seconds allowed for the deployment to write a product and for the catalog to downlink one
WRITE_TIMEOUT = 20
XMIT_TIMEOUT = 60


def dp_catalog(fprime_test_api, name):
    """Qualified mnemonic of a DpCatalog command or event"""
    return fprime_test_api.get_mnemonic("Svc.DpCatalog") + "." + name


def dp_writer(fprime_test_api, name):
    """Qualified mnemonic of a DpWriter event"""
    return fprime_test_api.get_mnemonic("Svc.DpWriter") + "." + name


def producer(fprime_test_api):
    """Return the configured (command, args) that produces one data product, or skip the test"""
    config = fprime_test_api.load_config_file() or {}
    command = config.get(PRODUCER_COMMAND_KEY)
    if not command:
        pytest.skip(f"{PRODUCER_COMMAND_KEY} not set in the deployment configuration")
    return command, config.get(PRODUCER_ARGS_KEY, [])


def write_product(fprime_test_api):
    """Command the producer and return the file name DpWriter reports in FileWritten"""
    command, args = producer(fprime_test_api)
    start = fprime_test_api.get_event_test_history().size()
    fprime_test_api.send_and_assert_command(command, args, max_delay=10)
    written = fprime_test_api.assert_event(
        dp_writer(fprime_test_api, "FileWritten"), start=start, timeout=WRITE_TIMEOUT
    )
    return written.get_args()[1].val


def send_dp_catalog_command(fprime_test_api, name, args=None):
    fprime_test_api.send_and_assert_command(
        dp_catalog(fprime_test_api, name), args, max_delay=10
    )


def transmit_catalog_until_complete(fprime_test_api, remain_active):
    """Start a transmission and wait for the catalog walk to complete"""
    start = fprime_test_api.get_event_test_history().size()
    send_dp_catalog_command(
        fprime_test_api, "START_XMIT_CATALOG", ["NO_WAIT", remain_active]
    )
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "CatalogXmitCompleted"), start=start, timeout=XMIT_TIMEOUT
    )


def test_insert_before_build(fprime_test_api):
    """A product written while the catalog is not built is not recorded and is reported with NotLoaded

    Covers: SVC-DPCAT-009
    """
    send_dp_catalog_command(fprime_test_api, "STOP_XMIT_CATALOG")
    send_dp_catalog_command(fprime_test_api, "CLEAR_CATALOG")

    start = fprime_test_api.get_event_test_history().size()
    file_name = write_product(fprime_test_api)

    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "NotLoaded"), [file_name], start=start, timeout=5
    )
    fprime_test_api.assert_event_count(
        0, dp_catalog(fprime_test_api, "DpFileAdded"), start=start
    )


def test_insert_into_built_catalog(fprime_test_api):
    """A product written after BUILD_CATALOG is inserted and downlinked by the next START_XMIT_CATALOG

    Covers: SVC-DPCAT-009
    """
    send_dp_catalog_command(fprime_test_api, "STOP_XMIT_CATALOG")
    send_dp_catalog_command(fprime_test_api, "CLEAR_CATALOG")
    send_dp_catalog_command(fprime_test_api, "BUILD_CATALOG")

    start = fprime_test_api.get_event_test_history().size()
    file_name = write_product(fprime_test_api)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpFileAdded"), [file_name], start=start, timeout=5
    )

    start = fprime_test_api.get_event_test_history().size()
    transmit_catalog_until_complete(fprime_test_api, remain_active=False)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "ProductComplete"),
        [file_name, None, None],
        start=start,
    )

    # the product is now recorded as transmitted: a rebuild skips it instead of re-adding it
    start = fprime_test_api.get_event_test_history().size()
    send_dp_catalog_command(fprime_test_api, "BUILD_CATALOG")
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpFileSkipped"), [file_name], start=start, timeout=5
    )
    fprime_test_api.assert_event_count(
        0, dp_catalog(fprime_test_api, "DpFileAdded"), start=start
    )


def test_insert_during_active_transmission(fprime_test_api):
    """A product written while the catalog transmission remains active is inserted and sent without a new command

    Covers: SVC-DPCAT-009
    """
    send_dp_catalog_command(fprime_test_api, "STOP_XMIT_CATALOG")
    send_dp_catalog_command(fprime_test_api, "CLEAR_CATALOG")
    send_dp_catalog_command(fprime_test_api, "BUILD_CATALOG")
    transmit_catalog_until_complete(fprime_test_api, remain_active=True)

    start = fprime_test_api.get_event_test_history().size()
    file_name = write_product(fprime_test_api)
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "DpFileAdded"), [file_name], start=start, timeout=5
    )
    # remainActive: the new product is sent and the transmission completes again on its own
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "ProductComplete"),
        [file_name, None, None],
        start=start,
        timeout=XMIT_TIMEOUT,
    )
    fprime_test_api.assert_event(
        dp_catalog(fprime_test_api, "CatalogXmitCompleted"), start=start, timeout=5
    )

    send_dp_catalog_command(fprime_test_api, "STOP_XMIT_CATALOG")
