####
# test_rust_toolchain.py:
#
# Generation without a usable Rust toolchain (CARGO=OFF): the framework skips Svc_WasmSequencer,
# and the Ref deployment, which instantiates it, fails before FPP analysis with the reason.
#
####
import shutil
import tempfile
from pathlib import Path

import pytest

from . import cmake
from . import settings

NO_CARGO = {"CARGO": "OFF", "FPRIME_SUB_BUILD_JOBS": f"{cmake.CPUS}"}


def run_generate(source_directory, options):
    """Run a CMake generation of source_directory and return (return_code, combined output)"""
    build_directory = Path(tempfile.mkdtemp())
    try:
        return_code, stdout, stderr = cmake.run_cmake(
            source_directory, build_directory, options
        )
        return return_code, "".join(stdout + stderr)
    finally:
        shutil.rmtree(build_directory, ignore_errors=True)


def test_framework_without_rust():
    """Framework-only generation skips Svc_WasmSequencer and succeeds"""
    return_code, output = run_generate(settings.FRAMEWORK_PATH, NO_CARGO)
    assert return_code == 0, "CMake generation failed"
    assert "[rust] Svc_WasmSequencer disabled: cargo not found" in output


@pytest.mark.parametrize("json_model", ["OFF", "ON"])
def test_ref_requires_rust(json_model):
    """Ref generation fails early, naming the requirement, the reason, and the install guide"""
    return_code, output = run_generate(
        settings.REF_APP_PATH,
        {**NO_CARGO, "FPRIME_ENABLE_JSON_MODEL_GENERATION": json_model},
    )
    assert return_code != 0, "Ref generation should fail without Rust"
    assert "Ref requires the Rust toolchain for Svc_WasmSequencer" in output
    assert "which is disabled: cargo not found" in output
    assert "https://www.rust-lang.org/tools/install" in output
    assert "symbol WasmSequencer is not defined" not in output
