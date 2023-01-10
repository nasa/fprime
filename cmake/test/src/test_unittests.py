####
# test_basic.py:
#
# Basic CMake tests.
#
####
import platform
import tempfile

import settings

import cmake

_ = cmake.get_build(
    "UT_BUILD",
    settings.REF_APP_PATH,
    cmake_arguments={"BUILD_TESTING": "ON"},
    make_targets=["Ref", "ut_exe"],
    install_directory=tempfile.mkdtemp(),
)
MODULES = settings.FRAMEWORK_MODULES + settings.STANDARD_MODULES

UNIT_TESTS = [
    "CFDP_Checksum_ut_exe",
    "Drv_TcpClient_ut_exe",
    "Drv_TcpServer_ut_exe",
    "Drv_Udp_ut_exe",
    "Fw_Buffer_ut_exe",
    "Fw_FilePacket_ut_exe",
    "Fw_Log_ut_exe",
    "Fw_SerializableFile_ut_exe",
    "Fw_Time_ut_exe",
    "Fw_Tlm_ut_exe",
    "Fw_Types_ut_exe",
    "Os_ut_exe",
    "Ref_SignalGen_ut_exe",
    "Svc_ActiveLogger_ut_exe",
    "Svc_ActiveRateGroup_ut_exe",
    "Svc_ActiveTextLogger_ut_exe",
    "Svc_AssertFatalAdapter_ut_exe",
    "Svc_BufferLogger_ut_exe",
    "Svc_BufferManager_ut_exe",
    "Svc_CmdDispatcher_ut_exe",
    "Svc_CmdSequencer_ut_exe",
    "Svc_ComLogger_ut_exe",
    "Svc_ComSplitter_ut_exe",
    "Svc_FileDownlink_ut_exe",
    "Svc_FileManager_ut_exe",
    "Svc_FileUplink_ut_exe",
    "Svc_Framer_ut_exe",
    "Svc_GenericHub_ut_exe",
    "Svc_GroundInterface_ut_exe",
    "Svc_Health_ut_exe",
    "Svc_LinuxTime_ut_exe",
    "Svc_LinuxTimer_ut_exe",
    "Svc_PolyDb_ut_exe",
    "Svc_PrmDb_ut_exe",
    "Svc_RateGroupDriver_ut_exe",
    "Svc_StaticMemory_ut_exe",
    "Svc_TlmChan_ut_exe",
    "Svc_TlmPacketizer_ut_exe",
    "Types_Circular_Buffer_ut_exe",
    "Utils_ut_exe",
]


def test_unittest_run(UT_BUILD):
    """Basic run test for ref"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)


def test_unittest_targets(UT_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)
    for module in MODULES:
        library_name = f"lib{module}.a"
        output_path = UT_BUILD["build"] / "lib" / platform.system() / library_name
        assert output_path.exists(), f"Failed to locate {library_name} in build output"
    for executable in ["Ref"] + UNIT_TESTS:
        output_path = UT_BUILD["build"] / "bin" / platform.system() / executable
        assert output_path.exists(), f"Failed to locate Ref in build output"


def test_unittest_installation(UT_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)
    for module in MODULES:
        library_name = f"lib{module}.a"
        output_path = (
            UT_BUILD["install"] / platform.system() / "lib" / "static" / library_name
        )
        assert output_path.exists(), f"Failed to locate {library_name} in build output"
    output_path = UT_BUILD["install"] / platform.system() / "bin" / "Ref"
    assert output_path.exists(), f"Failed to locate Ref in build output"


def test_unittest_dictionary(UT_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)
    output_path = (
        UT_BUILD["install"]
        / platform.system()
        / "dict"
        / "RefTopologyAppDictionary.xml"
    )
    assert output_path.exists(), f"Failed to locate Ref in build output"
