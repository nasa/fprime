#!/bin/bash
####
# Framework.bash:
#
# Run the tests on the software through fp-util.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

. ${SCRIPT_DIR}/tests/fputil.bash

export CMAKE_EXTRA_SETTINGS=""
echo -e "${BLUE}Testing ${FPRIME_DIR} against fprime-util targets: ${FPUTIL_TARGETS[@]}${NOCOLOR}"
export CHECK_TARGET_PLATFORM="native"
for target in "${FPUTIL_TARGETS[@]}"
do
    if [[ "${target}" == "generate" ]]
    then
        rm -rf "${FRIME_DIR}/build-fprime-automatic-"*
    fi
    fputil_action "${FPRIME_DIR}" "${target}"
done