#!/bin/bash
####
# 30-ints.bash:
#
# Integration tests for CI
####
# Directory to be used for Integration CI test
export FPUTIL_DEPLOYS="${FPRIME_DIR}/Ref"

export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

. ${SCRIPT_DIR}/tests/fputil.bash

# Run integration tests
echo -e "${BLUE}Testing ${INT_DEPLOYS} against integration tests${NOCOLOR}"
integration_test "${INT_DEPLOYS}"
