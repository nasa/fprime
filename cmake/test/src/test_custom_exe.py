####
# test_custom_exe.py:
#
# CMake executable additions
#
####
import os
import platform
import cmake

# Test a normal build, with the ref executable and static libraries
BUILD_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "custom-executables" )
EXPECTED = [
    os.path.join("bin", platform.system(), "UT-Custom-Execs"),
]
cmake.register_test(__name__, "custom-exe")
