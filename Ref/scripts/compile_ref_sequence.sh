#!/bin/sh
# *******************************************************************************
# * Copyright 2013, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged.
# *
# * regulations. By accepting this document, the user agrees to comply
# * with all applicable U.S. export laws and regulations.  User has the 
# * responsibility to obtain export licenses,
# * or other export authority as may be required before exporting such
# * information to foreign countries or providing access to foreign
# * persons.
# *
DIRNAME="$(dirname "$0")"
# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
if [ -z "${BUILD_ROOT}" ]
then
    BUILD_ROOT="$(cd "${DIRNAME}/../.." || exit ; pwd)"
    export BUILD_ROOT
fi
echo "BUILD_ROOT is: ${BUILD_ROOT}"
export PYTHONPATH="${BUILD_ROOT}/Fw/Python/src:${BUILD_ROOT}/Gds/src"
python "${BUILD_ROOT}/Gds/src/fprime_gds/tkgui/tools/seqgen.py" -g "${BUILD_ROOT}/Ref/py_dict" "$@"
