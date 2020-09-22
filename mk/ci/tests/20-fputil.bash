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
for deployment in ${FPUTIL_DEPLOYS}
do
    if [[ "${deployment}" == */RPI ]] && [ ! -d "/opt/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf" ]
    then
        warn_and_cont "RPI tools not installed, refusing to test."
        continue
    fi
    echo -e "${BLUE}Testing ${deployment} against fprime-util targets: ${FPUTIL_TARGETS[@]}${NOCOLOR}"
    export CHECK_TARGET_PLATFORM="native"
    for target in "${FPUTIL_TARGETS[@]}"
    do
        if [[ "${TEST_TYPE}" != "QUICK" ]] || [[ "${target}" == "generate" ]]
        then
            rm -rf "${deployment}/build-fprime-automatic-"*
        fi
        fputil_action "${deployment}" "${target}"
        if [[ "${TEST_TYPE}" != "QUICK" ]]
        then
            "${SCRIPT_DIR}/clean.bash" || fail_and_stop "Cleaning repository"
        fi
    done
done
