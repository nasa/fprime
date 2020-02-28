#!/bin/bash
####
# bootstrap.bash:
#
# ** Assumes that the user is in the F prime root directory for F prime CI **
#
# Sets up the python environment for the CI system. This will read the latest Python and ensure
# that we are ready to run all tools.
####
echo -e "${BLUE}Installing PIP Packages${NOCOLOR}"
pip install ./Fw/Python || fail_and_stop "Failed to install fprime PIP module from ./Fw/Python"
pip install ./Gds[test-api-xls] || fail_and_stop "Failed to install fprime PIP module from ./Gds[test-api-xls]"
