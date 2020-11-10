#!/bin/bash
####
# 10-cmake-uts.bash:
#
# Run the CMake UTs.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"
(
    cd cmake/test/
    pytest -v --ignore="src/test_validation.py" --ignore="src/test_ut.py" || fail_and_stop "Failed to run CMake UTs"
) || exit 1
