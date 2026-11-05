####
# test_target_triple.py:
#
# Tests for `fprime_get_target_triple` (cmake/platform/platform.cmake).
#
####
import re
import shutil
import tempfile
from pathlib import Path

from . import cmake
from . import settings

SOURCE_DIRECTORY = settings.DATA_DIR / "TestTargetTriple"
TRIPLE_PATTERN = re.compile(r"\[test\] TARGET_TRIPLE=(\S+)")


def run_triple_case(options=None):
    """Run a CMake generation of the TestTargetTriple project and return (return_code, triple)"""
    build_directory = Path(tempfile.mkdtemp())
    try:
        return_code, stdout, _ = cmake.run_cmake(
            SOURCE_DIRECTORY, build_directory, options
        )
        matches = [TRIPLE_PATTERN.search(line) for line in stdout]
        triples = [match.group(1) for match in matches if match]
        return return_code, triples[0] if triples else None
    finally:
        shutil.rmtree(build_directory, ignore_errors=True)


def test_target_triple_dumpmachine():
    """Native builds detect the triple via the compiler's -dumpmachine"""
    return_code, triple = run_triple_case()
    assert return_code == 0, "CMake generation failed"
    assert triple is not None, "Target triple was not reported"
    assert re.fullmatch(r"[\w.]+(-[\w.]+)+", triple), f"Invalid triple: '{triple}'"


def test_target_triple_override():
    """FPRIME_TARGET_TRIPLE overrides all detection"""
    return_code, triple = run_triple_case(
        {"FPRIME_TARGET_TRIPLE": "x86-64-unknown-none"}
    )
    assert return_code == 0, "CMake generation failed"
    assert triple == "x86-64-unknown-none", f"Override not respected: '{triple}'"


def test_target_triple_compiler_target():
    """CMAKE_<LANG>_COMPILER_TARGET is used when set"""
    return_code, triple = run_triple_case({"TEST_C_COMPILER_TARGET": "arm-none-eabi"})
    assert return_code == 0, "CMake generation failed"
    assert triple == "arm-none-eabi", f"Compiler target not used: '{triple}'"


def test_target_triple_mismatch():
    """Differing C and CXX compiler targets is a fatal error"""
    return_code, triple = run_triple_case(
        {
            "TEST_C_COMPILER_TARGET": "arm-none-eabi",
            "TEST_CXX_COMPILER_TARGET": "x86_64-linux-gnu",
        }
    )
    assert return_code != 0, "Mismatched compiler targets should fatal error"
    assert triple is None, "No triple should be reported on failure"
