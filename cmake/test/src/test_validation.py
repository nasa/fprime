####
# test_validation.py:
#
# Tests that validation steps can run.  Test that a validation test can fail the build is handled by
# test_tool_check.py.
#
####
import os
import platform
import cmake

# Test a normal build, with the ref executable and static libraries
BUILD_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "custom-validation")
EXPECTED = []
TARGETS = ["validate-nova"]
cmake.register_test(__name__, "custom-validation")
