#!/bin/bash
####
# wx-wrapper.bash:
#
# Wx doesn't work well on macOS in a virtual environment. This is because the virtual environment
# doesn't provide access to the screen through the virtual environment's python binary. This script
# stores the PYTHONHOME variable from the VIRTUALENV and then unsets the python virtual environment
# and runs the system python with the captures PYTHONHOME.
####
shift # Throw away the "python" argument
# First grab the python exe to replace the "python" argument stripped above
PY_VERSION="$(python --version | sed 's/Python \(.\).*/\1/g')"
if (( ${PY_VERSION} >= 3 ))
then
   PY_EXE="python3"
else
   PY_EXE="python"
fi
# Virtual environment's need to go to the system python.
if [[ "$VIRTUAL_ENV" != "" ]]
then
    # If no home set, set the virtual env
    if [[ "${PYTHONHOME}" == "" ]]
    then
        export PYTHONHOME="${VIRTUAL_ENV}"
    fi
    TMP="${VIRTUAL_ENV}/bin:"
    # Cut out the virtual env from path
    export PATH=${PATH/$TMP/} 
fi
# Run the python with the supplied arguments
${PY_EXE} "$@"
