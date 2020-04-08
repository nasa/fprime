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
export NOCOLOR='\033[0m' # No Color

####
# fail_and_stop:
#
# This function, when called ouside a subsehll, should print an error and stop the test. This will
# allow fairly easy test failures.
# :param message ($1): message to print to the error log
####
function fail_and_stop()
{
    echo -e "${RED}---------------- ERROR ----------------" 1>&2
    echo    "${1}" 1>&2
    echo -e "---------------------------------------${NOCOLOR}" 1>&2
    exit 1
}
export -f fail_and_stop
