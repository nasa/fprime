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

# if [[ "${TEST}" == "" ]] || [[ "${TEST}" == "Ref" ]] || [[ "${TEST}" == "RPI" ]]
# then
#     TEST_RUN="${SCRIPT_DIR}/tests/20-fputil.bash"
#     export TEST_TYPE="20-fputil"
# elif [[ "${TEST}" == "30-ints" ]]
# then
#     TEST_RUN="${SCRIPT_DIR}/tests/30-ints.bash"
#     export TEST_TYPE="30-ints"
# else
#     echo "===================NOTHING WAS DONE++++++++++++++++++++++++++++++"
# fi  

# TESTS="${@}"
# export TEST_TYPE="FULL"
# if (( $# ==  0 )) || [[ "${TESTS}" == "FULL" ]]
# then
#     TESTS="$(ls "${SCRIPT_DIR}/tests"/*)"
# elif [[ "${TESTS}" == "QUICK" ]]
# then
#     TESTS="${SCRIPT_DIR}/tests/20-fputil.bash ${SCRIPT_DIR}/tests/30-ints.bash"
#     export TEST_TYPE="QUICK"    
# elif [[ "${TESTS}" == "STATIC" ]]
# then
#     TESTS="${SCRIPT_DIR}/tests/40-pylama.bash"
#     export TEST_TYPE="STATIC"    
# fi

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(cd ${SCRIPT_DIR}/..; pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-${TEST}-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

####
# RP - Remove after refactor. Tools should be installed in Docker image.
####
# . "${SCRIPT_DIR}/bootstrap.bash" 

# Run the test script
echo -e "${BLUE}Starting CI test ${TEST_RUN} ${TEST}${NOCOLOR}"
/usr/bin/time "${TEST_RUN}" || fail_and_stop "${TEST} failed"
echo -e "${GREEN}CI test ${TEST_RUN} ${TEST} SUCCESSFUL${NOCOLOR}"


# Loop through all scripts in  tests directory and run them
# for test_script in ${TESTS}
# do
#     # "${SCRIPT_DIR}/clean.bash" || fail_and_stop "Cleaning directory"
#     echo -e "${BLUE}Starting CI test ${test_script}${NOCOLOR}"
#     /usr/bin/time "${test_script}" || fail_and_stop "${test_script} failed"
#     echo -e "${GREEN}CI test ${test_script} SUCCESSFUL${NOCOLOR}"
# done
archive_logs
