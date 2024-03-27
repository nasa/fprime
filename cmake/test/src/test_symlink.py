####
# test_symlink.py:
#
# Test that a build from a directory that contains a symlink works correctly.
#
####
import os
import shutil
import tempfile

import pytest
import settings

import cmake
from pathlib import Path


SYMLINK_PATH = Path(tempfile.mkdtemp()) / "fprime-link"
os.symlink(settings.REF_APP_PATH.parent, SYMLINK_PATH)


_ = cmake.get_build(
    "SYMLINKED_UT_BUILD",
    SYMLINK_PATH / "Ref",
    cmake_arguments={"BUILD_TESTING": "ON"},
    make_targets=["Ref", "ut_exe"],
    install_directory=tempfile.mkdtemp(),
)


@pytest.fixture(scope="session")
def symlink_maker():
    """Fixture for symlinked builds"""
    yield None
    shutil.rmtree(SYMLINK_PATH, ignore_errors=True)


def test_unittest_run(SYMLINKED_UT_BUILD, symlink_maker):
    """Basic run test for ref"""
    cmake.assert_process_success(SYMLINKED_UT_BUILD, errors_ok=True)
