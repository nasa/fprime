#!/bin/bash
####
# 00-fw-uts.bash:
#
# FW Unit Tests for the python code. Performed upfront as it will inform the stability of the rest
# of the build.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

pytest Fw/Python || fail_and_stop "Failed to run F prime framework UTs"
#pytest Gds --ignore=Gds/test/fprime_gds/common/gds_cli/parsing_test.py || fail_and_stop "Failed to run F prime Gds UTs"
