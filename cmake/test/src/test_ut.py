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
BUILD_DIR = os.path.join("<FPRIME>")
OPTIONS = {"CMAKE_BUILD_TYPE": "TESTING"}
TARGETS = ["all", "check"]
EXPECTED = [
    os.path.join("bin", platform.system(), "CFDP_Checksum_ut_exe"),
    os.path.join("bin", platform.system(), "Fw_FilePacket_ut_exe"),
    os.path.join("bin", platform.system(), "Fw_Log_ut_exe"),
    os.path.join("bin", platform.system(), "Fw_SerializableFile_ut_exe"),
    os.path.join("bin", platform.system(), "Fw_Time_ut_exe"),
    os.path.join("bin", platform.system(), "Fw_Tlm_ut_exe"),
    os.path.join("bin", platform.system(), "Fw_Types_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_ActiveLogger_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_ActiveRateGroup_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_BufferManager_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_CmdDispatcher_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_CmdSequencer_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_ComLogger_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_FileDownlink_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_FileManager_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_FileUplink_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_LinuxTime_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_PolyDb_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_PrmDb_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_RateGroupDriver_ut_exe"),
    os.path.join("bin", platform.system(), "Svc_TlmChan_ut_exe"),
]
cmake.register_test(__name__, "fp-uts")
