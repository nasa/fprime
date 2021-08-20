####
# test_basic.py:
#
# Basic CMake tests.
#
####
import os
import platform
import cmake

# Test a normal build, with the ref executable and static libraries
BUILD_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "custom-make-targets")
EXPECTED = [
    os.path.join("awesome", "everything_is_awesome"),
    os.path.join("awesome", "Drv_BlockDriver_is_awesome"),
    os.path.join("awesome", "Svc_FileUplink_is_awesome"),
    os.path.join("awesome", "Svc_CmdSequencer_is_awesome"),
    os.path.join("awesome", "Fw_Time_is_awesome"),
    os.path.join("awesome", "Svc_FileManager_is_awesome"),
    os.path.join("awesome", "Svc_PolyDb_is_awesome"),
    os.path.join("awesome", "Fw_FilePacket_is_awesome"),
    os.path.join("awesome", "Svc_Ping_is_awesome"),
    os.path.join("awesome", "Fw_Types_is_awesome"),
    os.path.join("awesome", "Utils_Hash_is_awesome"),
    os.path.join("awesome", "Fw_Port_is_awesome"),
    os.path.join("awesome", "Svc_RateGroupDriver_is_awesome"),
    os.path.join("awesome", "Svc_FatalHandler_is_awesome"),
    os.path.join("awesome", "Svc_TlmChan_is_awesome"),
    os.path.join("awesome", "Fw_Obj_is_awesome"),
    os.path.join("awesome", "Fw_Prm_is_awesome"),
    os.path.join("awesome", "Svc_ActiveRateGroup_is_awesome"),
    os.path.join("awesome", "Svc_Cycle_is_awesome"),
    os.path.join("awesome", "Svc_Health_is_awesome"),
    os.path.join("awesome", "Svc_LinuxTime_is_awesome"),
    os.path.join("awesome", "Fw_Com_is_awesome"),
    os.path.join("awesome", "Drv_DataTypes_is_awesome"),
    os.path.join("awesome", "Svc_ComLogger_is_awesome"),
    os.path.join("awesome", "Fw_SerializableFile_is_awesome"),
    os.path.join("awesome", "Fw_Buffer_is_awesome"),
    os.path.join("awesome", "Fw_Comp_is_awesome"),
    os.path.join("awesome", "Svc_CmdDispatcher_is_awesome"),
    os.path.join("awesome", "Fw_Log_is_awesome"),
    os.path.join("awesome", "Fw_Tlm_is_awesome"),
    os.path.join("awesome", "Os_is_awesome"),
    os.path.join("awesome", "Svc_Sched_is_awesome"),
    os.path.join("awesome", "Svc_Seq_is_awesome"),
    os.path.join("awesome", "Svc_WatchDog_is_awesome"),
    os.path.join("awesome", "Svc_AssertFatalAdapter_is_awesome"),
    os.path.join("awesome", "Svc_PolyIf_is_awesome"),
    os.path.join("awesome", "Svc_FileDownlink_is_awesome"),
    os.path.join("awesome", "Svc_Time_is_awesome"),
    os.path.join("awesome", "Fw_Cfg_is_awesome"),
    os.path.join("awesome", "Svc_Fatal_is_awesome"),
    os.path.join("awesome", "Svc_ActiveLogger_is_awesome"),
    os.path.join("awesome", "Svc_BufferManager_is_awesome"),
    os.path.join("awesome", "Fw_Cmd_is_awesome"),
    os.path.join("awesome", "CFDP_Checksum_is_awesome"),
    os.path.join("awesome", "Svc_PrmDb_is_awesome"),
    os.path.join("awesome", "Svc_PassiveConsoleTextLogger_is_awesome"),
]
TARGETS = ["awesome"]
cmake.register_test(__name__, "custom-target")
