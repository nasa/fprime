#!/bin/bash
####
# RPI.bash:
#
# Run the tests on the software through fp-util.
####
# Directory to be used for RPI CI test
export FPUTIL_DEPLOYS="${FPRIME_DIR}/RPI"

export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"
. ${SCRIPT_DIR}/tests/fputil.bash

export CMAKE_EXTRA_SETTINGS=""
if [ ! -d "/opt/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf" ]
then
    warn_and_cont "RPI tools not installed, refusing to test."
    continue
fi
# For RPI deployment to disable FRAMEWORK UTS
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