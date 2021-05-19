#!/bin/bash
####
# 40-pylama.bash:
#
# Perform static analysis on all the python code by running pylama.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

echo -e "${BLUE}Running static analysis (pylama) on python code${NOCOLOR}"
pylama -o ${SCRIPT_DIR}/pylama-ci.cfg ${FPRIME_DIR}

RET_PYLAMA=$?

if [[ ${RET_PYLAMA} -ne 0 ]]
then 
    # replace with fail_and_stop when all the current python issues have been resolved
    warn_and_cont "Python code contains Errors and Warnings that must be resolved"
fi

