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
