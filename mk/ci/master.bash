#!/bin/bash
####
# master.bash:
#
# Master script to run the CI process. This will setup and run the tests for the CI system.
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/helpers.bash"

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

# Loop through all scripts in  tests directory and run them
for test_script in "${SCRIPT_DIR}/tests"/*
do
    "${SCriPT_DIR}/bootstrap.bash" || fail_and_stop "Cleaning and PIP install Failed"
    "${test_script}" || fail_and_stop "${test_script} failed"    
done
