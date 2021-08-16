#!/bin/bash
####
# master.bash:
#
# Master script to run the CI process. This will setup and run the tests for the CI system.
####

 CTEST_OUTPUT_ON_FAILURE=1
 SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/helpers.bash"

 TEST="${}"
TEST_RUN="${SCRIPT_DIR}/tests/${TEST}.bash"
 TEST_TYPE="${TEST}"

#### NEEDED ENVIRONMENT ####
 FPRIME_DIR="$(cd ${SCRIPT_DIR}/..; pwd)"
 LOG_DIR="${FPRIME_DIR}/ci-${TEST}-logs-$(date +"%Y-%m-%dT%H%M%S")"
    -p "${LOG_DIR}"

# Run the test script
    -e "${BLUE}Starting CI test ${TEST_RUN} ${TEST}${NOCOLOR}"
/usr/bin/time "${TEST_RUN}"  fail_and_stop "${TEST} failed"
    -e "${GREEN}CI    ${TEST_RUN} ${TEST} SUCCESSFUL${NOCOLOR}"

archive_logs
