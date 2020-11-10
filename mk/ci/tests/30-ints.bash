#!/bin/bash
####
# 30-ints.bash:
#
# Integration tests for CI
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

. ${SCRIPT_DIR}/tests/fputil.bash
# Loop over deployments and targets
for deployment in ${INT_DEPLOYS}
do
    echo -e "${BLUE}Testing ${deployment} against integration tests${NOCOLOR}"
    integration_test "${deployment}"
done
