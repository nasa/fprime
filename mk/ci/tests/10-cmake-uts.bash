#!/bin/bash
####
# 10-cmake-uts.bash:
#
# Run the CMake UTs.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

pytest cmake/ -v --ignore test_ut.py --ignore test_validation.py || fail_and_stop "Failed to run CMake UTs"
