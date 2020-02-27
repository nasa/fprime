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
    echo -e "${BLUE}Testing ${deployment} against fprime-util targets: ${FPUTIL_TARGET}${NOCOLOR}"
    for target in ${FPUTIL_TARGET}
    do
        fputil_action "${deployment}" "${target}"
    done
done
