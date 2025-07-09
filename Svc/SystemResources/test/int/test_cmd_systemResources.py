""" test_cmd_systemResources.py:

Test the command dispatcher with basic integration tests.
"""
import time

def test_send_systemResources_command(fprime_test_api):
    """Test that commands may be sent

     Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.

     SystemResources.Enable, <Disabled> (read SystemResources telemetry confirm value stale or stop)
     SystemResources.Enable, <Enabled>  (read SystemResources telemetry confirm value changing)

    """
    
    # Current channels before disable
    CPU_resources1 = fprime_test_api.await_telemetry(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", start="NOW")
    CPU_percent1 = fprime_test_api.get_telemetry_pred(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", CPU_resources1)

    fprime_test_api.clear_histories()  # will clear all history (can read telemetry channel again with latest value.  otherwise still have old value)
    time.sleep(5)

    # Expect number still changing after clear_history    
    CPU_resources1A = fprime_test_api.await_telemetry(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", start="NOW")
    CPU_percent1A = fprime_test_api.get_telemetry_pred(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", CPU_resources1A)
    
    ##### Command Disabled SystemResource.ENABLE command (DISABLED)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.SystemResources') + '.' + 'ENABLE',["DISABLED"])

    time.sleep(3)
    # Expect number no change (stale or stop) after Disable 
    CPU_resources2 = fprime_test_api.await_telemetry(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", start="NOW")
    CPU_percent2 = fprime_test_api.get_telemetry_pred(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", CPU_resources2)
    time.sleep(5)
    
    CPU_resources2B = fprime_test_api.await_telemetry(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", start="NOW")
    CPU_percent2B = fprime_test_api.get_telemetry_pred(fprime_test_api.get_mnemonic('Svc.SystemResources') + "." + "CPU", CPU_resources2B)

    ##### Command Disabled SystemResource.ENABLE command (ENABLED)    
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.SystemResources') + '.' + 'ENABLE',["ENABLED"])

    




