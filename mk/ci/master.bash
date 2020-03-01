#!/bin/bash
####
# master.bash:
#
# Master script to run the CI process. This will setup and run the tests for the CI system.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/helpers.bash"

TESTS="${@}"
if (( $# ==  0 ))
then
    TESTS="$(ls "${SCRIPT_DIR}/tests"/*)"
fi

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

. "${SCRIPT_DIR}/bootstrap.bash" 
# Loop through all scripts in  tests directory and run them
for test_script in ${TESTS}
do
    "${SCRIPT_DIR}/clean.bash" || fail_and_stop "Cleaning drectory"
    echo -e "${BLUE}Starting CI test ${test_script}${NOCOLOR}"
    /usr/bin/time "${test_script}" || fail_and_stop "${test_script} failed"
    echo -e "${GREEN}CI test ${test_script} SUCCESSFUL${NOCOLOR}"
done
