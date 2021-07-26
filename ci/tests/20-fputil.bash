#!/bin/bash
####
# 20-fputil.bash:
#
# Run the tests on the software through fp-util.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

. ${SCRIPT_DIR}/tests/fputil.bash

export CMAKE_EXTRA_SETTINGS=""
if [[ "${FPUTIL_DEPLOYS}" == */RPI ]] && [ ! -d "/opt/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf" ]
then
    warn_and_cont "RPI tools not installed, refusing to test."
    continue
fi
# Check if RPI or Ref deployment to disable FRAMEWORK UTS
# RP - These are the only 2 deployments used by the 20-fputil CI
if [[ "${FPUTIL_DEPLOYS}" == */RPI ]] || [[ "${FPUTIL_DEPLOYS}" == */Ref ]]
then
    export CMAKE_EXTRA_SETTINGS="${CMAKE_EXTRA_SETTINGS} -DFPRIME_ENABLE_FRAMEWORK_UTS=OFF"
fi 
echo -e "${BLUE}Testing ${FPUTIL_DEPLOYS} against fprime-util targets: ${FPUTIL_TARGETS[@]}${NOCOLOR}"
export CHECK_TARGET_PLATFORM="native"
for target in "${FPUTIL_TARGETS[@]}"
do
    START="$(date +%s)"
    # RP - This if stmt is only used for the 20-fputil CI
    if [[ "${target}" == "generate" ]]
    then
        rm -rf "${FPUTIL_DEPLOYS}/build-fprime-automatic-"*
    fi
    fputil_action "${FPUTIL_DEPLOYS}" "${target}"
    END="$(date +%s)"
    echo "$[${END} - ${START}]"
done

# Loop over deployments and targets
# for deployment in ${FPUTIL_DEPLOYS}
# do
#     export CMAKE_EXTRA_SETTINGS=""
#     if [[ "${deployment}" == */RPI ]] && [ ! -d "/opt/rpi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf" ]
#     then
#         warn_and_cont "RPI tools not installed, refusing to test."
#         continue
#     fi
#     # Check if RPI or Ref deployment to disable FRAMEWORK UTS
#     # RP - These are the only 2 deployments used by the 20-fputil CI
#     if [[ "${deployment}" == */RPI ]] || [[ "${deployment}" == */Ref ]]
#     then
#         export CMAKE_EXTRA_SETTINGS="${CMAKE_EXTRA_SETTINGS} -DFPRIME_ENABLE_FRAMEWORK_UTS=OFF"
#     fi 
#     echo -e "${BLUE}Testing ${deployment} against fprime-util targets: ${FPUTIL_TARGETS[@]}${NOCOLOR}"
#     export CHECK_TARGET_PLATFORM="native"
#     for target in "${FPUTIL_TARGETS[@]}"
#     do
#         # RP - This if stmt is only used for the 20-fputil CI
#         if [[ "${TEST_TYPE}" != "20-fputil" ]] || [[ "${target}" == "generate" ]]
#         then
#             rm -rf "${deployment}/build-fprime-automatic-"*
#         fi
#         fputil_action "${deployment}" "${target}"
#         # RP - This if stmt is only used for the 20-fputil CI
#         if [[ "${TEST_TYPE}" != "20-fputil" ]]
#         then
#             "${SCRIPT_DIR}/clean.bash" || fail_and_stop "Cleaning repository"
#         fi
#     done
# done
