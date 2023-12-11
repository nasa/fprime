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
    "REF_BUILD",
    settings.REF_APP_PATH,
    cmake_arguments={"BUILD_SHARED_LIBS": "ON"},
    make_targets=["Ref"],
    install_directory=tempfile.mkdtemp(),
)
MODULES = settings.FRAMEWORK_MODULES + settings.STANDARD_MODULES


def test_ref_run(REF_BUILD):
    """Basic run test for ref"""
    cmake.assert_process_success(REF_BUILD)


def test_ref_targets(REF_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(REF_BUILD)
    for module in MODULES:
        library_name = (
            f"lib{module}{'.so' if platform.system() != 'Darwin' else '.dylib'}"
        )
        output_path = REF_BUILD["build"] / "lib" / platform.system() / library_name
        assert output_path.exists(), f"Failed to locate {library_name} in build output"
    output_path = REF_BUILD["build"] / "bin" / platform.system() / "Ref"
    assert output_path.exists(), "Failed to locate Ref in build output"


def test_ref_installation(REF_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(REF_BUILD)
    for module in MODULES:
        library_name = (
            f"lib{module}{'.so' if platform.system() != 'Darwin' else '.dylib'}"
        )
        output_path = (
            REF_BUILD["install"] / platform.system() / "Ref" / "lib" / library_name
        )
        assert output_path.exists(), f"Failed to locate {library_name} in build output"
    output_path = REF_BUILD["install"] / platform.system() / "Ref" / "bin" / "Ref"
    assert output_path.exists(), "Failed to locate Ref in build output"


def test_ref_dictionary(REF_BUILD):
    """Run reference and assert reference targets exit"""
    cmake.assert_process_success(REF_BUILD)
    output_path = (
        REF_BUILD["install"]
        / platform.system()
        / "Ref"
        / "dict"
        / "RefTopologyAppDictionary.xml"
    )
    assert output_path.exists(), "Failed to locate Ref in build output"


def test_ref_module_info(REF_BUILD):
    """Run reference and assert module-info.txt was created"""
    cmake.assert_process_success(REF_BUILD)
    txt_path = REF_BUILD["build"] / "Ref" / "SignalGen" / "module-info.txt"
    assert txt_path.exists(), "Failed to locate module-info.txt under SignalGen"

    with open(txt_path, "r") as file_path:
        lines = file_path.readlines()
    assert len(lines) == 5, "Module info not correct number of lines"
    headers, sources, generated, ac_sources, dependencies = [
        line.strip().split(";") for line in lines
    ]
    assert ["SignalGen.hpp"] == [
        Path(header).name for header in headers
    ], "Did not find expected headers"
    assert ["SignalGen.cpp"] == [
        Path(source).name for source in sources
    ], "Did not find expected sources"
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
    ]
    actual_gen = [Path(source).name for source in generated]
    assert sorted(expected_gen) == sorted(
        actual_gen
    ), "Did not find expected autocoder generated sources"
