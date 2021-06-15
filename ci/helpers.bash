#!/bin/bash
####
# helpers.bash:
#
# Helpers used to make the CI system a bit nicer. It defines some methods, like pretty colors for
# usage elsewhere.
####
export BLUE='\033[0;34m'
export GREEN='\033[0;32m'
export RED='\033[0;31m'
export PURPLE='\033[0;95m'
export NOCOLOR='\033[0m' # No Color

####
# archive_logs: create a log archive
####
function archive_logs()
{
    tar -czf "${FPRIME_DIR}/ci-logs.tar.gz" "${LOG_DIR}"/*
}

####
# fail_and_stop:
#
# This function, when called outside a subshell, should print an error and stop the test. This will
# allow fairly easy test failures.
# :param message ($1): message to print to the error log
####
function fail_and_stop()
{
    echo -e "${RED}---------------- ERROR ----------------" 1>&2
    echo    "${1}" 1>&2
    echo -e "---------------------------------------${NOCOLOR}" 1>&2
    LASTLOG=$(ls -td $(find "${LOG_DIR}" -name "*err.log" -type f) | head -1)
    if [ -f "${LASTLOG}" ]
    then
    
        echo -e "---------------- STDERR ---------------" 1>&2
        tail -30 "${LASTLOG}" 1>&2
        echo -e "---------------------------------------" 1>&2
    fi
    archive_logs
    exit 1
}
####
# warn_and_cont:
#
# This function produces a warning block of test but exits with a success error code such that subsequent tests will run.
####
function warn_and_cont()
{
    echo -e "${PURPLE}--------------- WARNING --------------" 1>&2
    echo    "${1}" 1>&2
    echo -e "---------------------------------------${NOCOLOR}" 1>&2
}

export -f fail_and_stop
export -f warn_and_cont
export -f archive_logs
