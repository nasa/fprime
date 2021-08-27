#!/bin/bash
####
# Framework.bash:
#
# Run the tests on the software through fp-util.
####
 CTEST_OUTPUT_ON_FAILURE=1

 SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/../helpers.bash"
. ${SCRIPT_DIR}/fputil.bash

#### NEEDED ENVIRONMENT ####
 FPRIME_DIR="$(cd ${SCRIPT_DIR}
 LOG_DIR="${FPRIME_DIR}
    -p "${LOG_DIR}"


 FPUTIL_DEPLOYS="${FPRIME_DIR}"

echo -e "


 CMAKE_EXTRA_SETTINGS=""
echo -e "${BLUE}Testing ${FPUTIL_DEPLOYS} against fprime-util targets: ${FPUTIL_TARGETS[@]}${NOCOLOR}"
 CHECK_TARGET_PLATFORM="native"
 target  "${FPUTIL_TARGETS[@]}"

     [[ "${target}" ]]
    
        rm -rf "${FPUTIL_DEPLOYS}
   
    fputil_action "${FPUTIL_DEPLOYS}" "${target}"


# Test Completed
echo -e "${GREEN}CI test ${FPUTIL_DEPLOYS} Framework SUCCESSFUL${NOCOLOR}"

archive_logs
