#!/bin/bash
####
# bootstrap.bash:
#
# ** Assumes that the user is in the F prime root directory for F prime CI **
#
# Sets up the python environment for the CI system. This will read the latest Python and ensure
# that we are ready to run all tools.
####
# Clean the repository as part of setup
echo -e "${BLUE}Cleaning Repository and Installing PIP Package${NOCOLOR}"
git clean -xdf .
pip install ./Fw/Python
pip install ./Gds[test-api-xls]
