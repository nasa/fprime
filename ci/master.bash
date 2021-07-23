#!/bin/bash
####
# master.bash:
#
# Master script to run the CI process. This will setup and run the tests for the CI system.
####

export CTEST_OUTPUT_ON_FAILURE=1
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/helpers.bash"

export TEST="${@}"
TEST_RUN="${SCRIPT_DIR}/tests/${TEST}.bash"
export TEST_TYPE="${TEST}"

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(cd ${SCRIPT_DIR}/..; pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-${TEST}-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

# Run the test script
echo -e "${BLUE}Starting CI test ${TEST_RUN} ${TEST}${NOCOLOR}"
/usr/bin/time "${TEST_RUN}" || fail_and_stop "${TEST} failed"
echo -e "${GREEN}CI test ${TEST_RUN} ${TEST} SUCCESSFUL${NOCOLOR}"

archive_logs
