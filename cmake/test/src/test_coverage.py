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
TARGETS = ["Svc_CmdDispatcher_coverage"] #Force a known `coverage` target
EXPECTED = [
    os.path.join("bin", platform.system(), "Svc_CmdDispatcher_ut_exe"), #UT must be built
    os.path.join("<FPRIME>", "Svc", "CmdDispatcher", "coverage", "CommandDispatcherImpl.cpp.gcov"),
    os.path.join("<FPRIME>", "Svc", "CmdDispatcher", "coverage", "CommandDispatcherComponentAc.cpp.gcov")
]
cmake.register_test(__name__, "fp-coverage")
