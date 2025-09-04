"""test_cmd_systemResources.py:

Test the command dispatcher with basic integration tests.
"""

import time


def test_send_systemResources_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.

    SystemResources.Enable, <Disabled> (read SystemResources telemetry confirm value stale or stop)
    SystemResources.Enable, <Enabled>  (read SystemResources telemetry confirm value changing)

    """

    ## Verify memory usage Mem_total and Mem_used and Non_Volatile_total and Non_Volatile_free greater than a certain value 1KB
    mem_total = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "MEMORY_TOTAL",
        start="NOW",
    )
    mem_used = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "MEMORY_USED",
        start="NOW",
    )
    nv_total = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources")
        + "."
        + "NON_VOLATILE_TOTAL",
        start="NOW",
    )
    nv_free = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "NON_VOLATILE_FREE",
        start="NOW",
    )

    mem_total_list = str(mem_total).split()
    mem_used_list = str(mem_used).split()
    nv_total_list = str(nv_total).split()
    nv_free_list = str(nv_free).split()
    print("Index my_LIST", mem_total_list)

    # verify memory usage
    if (
        int(mem_total_list[3]) >= 1
        and mem_total_list[4] == "KB"
        and int(mem_used_list[3]) >= 1
        and mem_used_list[4] == "KB"
        and int(nv_total_list[3]) >= 1
        and nv_total_list[4] == "KB"
        and int(nv_free_list[3]) >= 1
        and nv_free_list[4] == "KB"
    ):
        print(
            "PASSED: Memory usage MEM_TOTAL, MEM_USED, NV_TOTAL, NV_FREE greater than a certain value 1K"
        )
        print(
            "LIST VALUE:  ",
            mem_total_list[3],
            mem_used_list[3],
            nv_total_list[3],
            nv_free_list[3],
        )
    else:
        print("FAILED to confirm memory usage")

    # Verify a number of CPUs greater than or equal to 1 ((example for 4 CPUs other sometimes is less than 1 percent  )
    cpu = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU", start="NOW"
    )
    cpu_00 = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU_00",
        start="NOW",
    )
    cpu_01 = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU_01",
        start="NOW",
    )
    cpu_02 = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU_02",
        start="NOW",
    )
    cpu_03 = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU_03",
        start="NOW",
    )

    cpu_list = str(cpu).split()
    cpu_00_list = str(cpu_00).split()
    cpu_01_list = str(cpu_01).split()
    cpu_02_list = str(cpu_02).split()
    cpu_03_list = str(cpu_03).split()
    print("CPU LIST", cpu_list)

    if (
        float(cpu_list[3]) >= 1
        and float(cpu_00_list[3]) >= 1
        and float(cpu_01_list[3]) >= 1
        and float(cpu_02_list[3]) >= 1
        and float(cpu_03_list[3]) >= 1
    ):
        print("PASSED : A number of CPUs greater than or equal to 1")
        print(
            "PERCENT VALUE: ",
            cpu_list[3],
            cpu_00_list[3],
            cpu_01_list[3],
            cpu_02_list[3],
            cpu_03_list[3],
        )
    else:
        print("FAILED: cpu")

    # Start command here:
    # Current channels before disable
    CPU_resources1 = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU", start="NOW"
    )
    CPU_percent1 = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU",
        CPU_resources1,
    )
    print("CPU RESOURCES1: ", CPU_resources1)
    print("PERCENT: ", CPU_percent1)

    fprime_test_api.clear_histories()  # will clear all history (can read telemetry channel again with latest value.  otherwise still have old value)
    time.sleep(5)

    # Expect number still changing after clear_history
    CPU_resources1A = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU", start="NOW"
    )
    CPU_percent1A = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU",
        CPU_resources1A,
    )

    ##### Command Disabled SystemResources.ENABLE command (DISABLED)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "ENABLE",
        ["DISABLED"],
    )

    time.sleep(3)
    # Expect number no change (stale or stop) after Disable
    CPU_resources2 = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU", start="NOW"
    )
    CPU_percent2 = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU",
        CPU_resources2,
    )
    time.sleep(5)

    CPU_resources2B = fprime_test_api.await_telemetry(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU", start="NOW"
    )
    CPU_percent2B = fprime_test_api.get_telemetry_pred(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "CPU",
        CPU_resources2B,
    )

    ##### Command Disabled SystemResources.ENABLE command (ENABLED)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.SystemResources") + "." + "ENABLE",
        ["ENABLED"],
    )
