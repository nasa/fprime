#!/bin/bash
####
# Ref.bash:
#
# Run the tests on the software through fp-util.
####
export CTEST_OUTPUT_ON_FAILURE=1

export SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/../helpers.bash"
. ${SCRIPT_DIR}/fputil.bash

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(cd ${SCRIPT_DIR}/../..; pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-Ref-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

# Directory to be used for Ref CI test
export FPUTIL_DEPLOYS="${FPRIME_DIR}/Ref"

echo -e "${BLUE}Starting CI test ${FPUTIL_DEPLOYS} Ref${NOCOLOR}"

export CMAKE_EXTRA_SETTINGS=""
# For Ref deployment to disable FRAMEWORK UTS
export CMAKE_EXTRA_SETTINGS="${CMAKE_EXTRA_SETTINGS} -DFPRIME_ENABLE_FRAMEWORK_UTS=OFF"
 
echo -e "${BLUE}Testing ${FPUTIL_DEPLOYS} against fprime-util targets: ${FPUTIL_TARGETS[@]}${NOCOLOR}"

export CHECK_TARGET_PLATFORM="native"
for target in "${FPUTIL_TARGETS[@]}"
do
    if [[ "${target}" == "generate" ]]
    then
        rm -rf "${FPUTIL_DEPLOYS}/build-fprime-automatic-"*
    fi
    fputil_action "${FPUTIL_DEPLOYS}" "${target}"
done

# Test Completed
echo -e "${GREEN}CI test ${FPUTIL_DEPLOYS} Ref SUCCESSFUL${NOCOLOR}"

archive_logs