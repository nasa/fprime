####
# test_basic.py:
#
# Basic CMake tests.
#
####
import platform
import tempfile
import json
import pytest
from pathlib import Path
from . import cmake
from . import settings


if platform.system() == "Darwin":
    pytestmark = pytest.mark.skip(reason="Shared modules are not supported on macOS")

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


def test_ref_dictionary_json(REF_BUILD):
    """Build Ref and assert JSON dictionary exists"""
    cmake.assert_process_success(REF_BUILD)
    output_path = (
        REF_BUILD["install"]
        / platform.system()
        / "Ref"
        / "dict"
        / "RefTopologyDictionary.json"
    )
    assert output_path.exists(), "Failed to locate Ref JSON Dictionary in build output"
    dict_metadata = json.loads(output_path.read_text()).get("metadata")
    assert (
        dict_metadata.get("projectVersion") is not None
    ), "Project version missing in JSON Dictionary"
    # For Ref, versions should match since it's the same Git repo
    assert dict_metadata.get("frameworkVersion") == dict_metadata.get(
        "projectVersion"
    ), "Version mismatch in JSON Dictionary"
