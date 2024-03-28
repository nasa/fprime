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
from pathlib import Path

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
    "Svc_Health_ut_exe",
    "Svc_PosixTime_ut_exe",
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
        assert output_path.exists(), "Failed to locate Ref in build output"


def test_unittest_installation(UT_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)
    for module in MODULES:
        library_name = f"lib{module}.a"
        output_path = (
            UT_BUILD["install"]
            / platform.system()
            / "Ref"
            / "lib"
            / "static"
            / library_name
        )
        assert output_path.exists(), f"Failed to locate {library_name} in build output"
    output_path = UT_BUILD["install"] / platform.system() / "Ref" / "bin" / "Ref"
    assert output_path.exists(), "Failed to locate Ref in build output"


def test_unittest_dictionary(UT_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)
    output_path = (
        UT_BUILD["install"]
        / platform.system()
        / "Ref"
        / "dict"
        / "RefTopologyAppDictionary.xml"
    )
    assert output_path.exists(), "Failed to locate Ref in build output"


def test_unittest_module_ut_info(UT_BUILD):
    """Run reference and assert module-ut-info.txt was created"""
    cmake.assert_process_success(UT_BUILD, errors_ok=True)
    txt_path = UT_BUILD["build"] / "Ref" / "SignalGen" / "module-ut-info.txt"
    assert txt_path.exists(), "Failed to locate module-info.txt under SignalGen"

    with open(txt_path, "r") as file_path:
        lines = file_path.readlines()
    assert len(lines) == 5, "Module info not correct number of lines"
    headers, sources, generated, ac_sources, dependencies = [
        line.strip().split(";") for line in lines
    ]
    assert ["SignalGenTester.hpp"] == [
        Path(header).name for header in headers
    ], "Did not find expected headers"
    assert sorted(["SignalGenTester.cpp", "SignalGenTestMain.cpp"]) == sorted(
        [Path(source).name for source in sources]
    ), "Did not find expected sources"
    expected_ac = ["SignalGen.fpp", "Commands.fppi", "Events.fppi", "Telemetry.fppi"]
    actual_ac = [Path(source).name for source in ac_sources]
    assert sorted(expected_ac) == sorted(
        actual_ac
    ), "Did not find expected autocoder sources"
    expected_gen = [
        "SignalGenComponentAi.xml",
        "SignalInfoSerializableAi.xml",
        "SignalPairSerializableAi.xml",
        "SignalPairSetArrayAi.xml",
        "SignalSetArrayAi.xml",
        "SignalTypeEnumAi.xml",
        "SignalGenComponentAc.cpp",
        "SignalGenComponentAc.hpp",
        "SignalInfoSerializableAc.cpp",
        "SignalInfoSerializableAc.hpp",
        "SignalPairSerializableAc.cpp",
        "SignalPairSerializableAc.hpp",
        "SignalPairSetArrayAc.cpp",
        "SignalPairSetArrayAc.hpp",
        "SignalSetArrayAc.cpp",
        "SignalSetArrayAc.hpp",
        "SignalTypeEnumAc.cpp",
        "SignalTypeEnumAc.hpp",
        "SignalGenGTestBase.cpp",
        "SignalGenGTestBase.hpp",
        "SignalGenTesterBase.cpp",
        "SignalGenTesterBase.hpp",
        "SignalGenTesterHelpers.cpp",
    ]
    actual_gen = [Path(source).name for source in generated]
    assert sorted(expected_gen) == sorted(
        actual_gen
    ), "Did not find expected autocoder generated sources"
    assert dependencies == ["Ref_SignalGen"], "Did not find expected dependencies"
