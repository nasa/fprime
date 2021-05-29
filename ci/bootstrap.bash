#!/bin/bash
####
# bootstrap.bash:
#
# ** Assumes that the user is in the F prime root directory for F prime CI **
#
# Sets up the python environment for the CI system. This will read the latest Python and ensure
# that we are ready to run all tools.
####
export USABLE_VENV="${FPRIME_DIR}/ci-venv"
echo -e "${BLUE}Preparing VENV at: ${USABLE_VENV}${NOCOLOR}"
deactivate
rm -rf "${USABLE_VENV}"
python3 -m venv "${USABLE_VENV}" || fail_and_stop "Failed to create VENV"
. "${USABLE_VENV}/bin/activate" || fail_and_stop "Failed to source VENV"
echo -e "Installing PIP Packages"
# install dependencies based on the TEST_TYPE
if [[ "${TEST_TYPE}" == "STATIC" ]]
then
    # Only pylama and pylint are needed for STATIC
    pip install -U pylama pylama_pylint radon
else
    # These are required for all other tests
    pip install fprime-tools fprime-gds || fail_and_stop "Failed to install fprime PIP module from ./Fw/Python"
fi
