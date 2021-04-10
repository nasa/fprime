#!/bin/bash
####
# master.bash:
#
# Master script to run the CI process. This will setup and run the tests for the CI system.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/helpers.bash"

TESTS="${@}"
export TEST_TYPE="FULL"
if (( $# ==  0 )) || [[ "${TESTS}" == "FULL" ]]
then
    TESTS="$(ls "${SCRIPT_DIR}/tests"/*)"
elif [[ "${TESTS}" == "QUICK" ]]
then
    TESTS="${SCRIPT_DIR}/tests/20-fputil.bash ${SCRIPT_DIR}/tests/30-ints.bash"
    export TEST_TYPE="QUICK"    
elif [[ "${TESTS}" == "STATIC" ]]
then
    TESTS="${SCRIPT_DIR}/tests/40-pylama.bash"
    export TEST_TYPE="STATIC"    
fi

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(cd ${SCRIPT_DIR}/..; pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

. "${SCRIPT_DIR}/bootstrap.bash" 

# Loop through all scripts in  tests directory and run them
for test_script in ${TESTS}
do
    "${SCRIPT_DIR}/clean.bash" || fail_and_stop "Cleaning directory"
    echo -e "${BLUE}Starting CI test ${test_script}${NOCOLOR}"
    /usr/bin/time "${test_script}" || fail_and_stop "${test_script} failed"
    echo -e "${GREEN}CI test ${test_script} SUCCESSFUL${NOCOLOR}"
done
