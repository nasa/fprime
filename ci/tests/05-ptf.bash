#!/bin/bash
####
# 05-ptf.bash:
#
# Heritage PTF UTs tests
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"
(
    cd ptf
    ./run_all_unit_tests.sh
) || fail_and_stop "F prime UTs failed via PTF"
(
    cd ptf
    ./run_ac_tests.sh
) || fail_and_stop "F prime UTs failed via PTF"
