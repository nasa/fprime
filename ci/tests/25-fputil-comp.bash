#!/bin/bash
####
# 20-fputil.bash:
#
# Run the tests on the software through fp-util.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

. ${SCRIPT_DIR}/tests/fputil.bash
# Loop over deployments and targets
deployment="${FPRIME_DIR}/Ref"
component="SignalGen"
echo -e "${BLUE}Testing ${deployment} against fprime-util targets: ${FPUTIL_TARGETS}${NOCOLOR}"
export CHECK_TARGET_PLATFORM="native"
for target in "impl" "impl --ut" "build" "build --ut"
do
    if [[ "${TEST_TYPE}" != "QUICK" ]] || [[ "${target}" == "generate" ]]
    then
        rm -rf "${deployment}/build-fprime-automatic-"*
    fi
    fputil_action "${deployment}" "${target}" "${component}"
    if [[ "${TEST_TYPE}" != "QUICK" ]]
    then
        "${SCRIPT_DIR}/clean.bash" || fail_and_stop "Cleaning repository"
    fi
done
