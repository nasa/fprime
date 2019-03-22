#!/bin/sh
# *******************************************************************************
# * Copyright 2019, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged. Any commercial use must be negotiated with the Office
# * of Technology Transfer at the California Institute of Technology.
# *
# * This software may be subject to U.S. export control laws and
# * regulations. By accepting this document, the user agrees to comply
# * with all applicable U.S. export laws and regulations.  User has the
# * responsibility to obtain export licenses,
# * or other export authority as may be required before exporting such
# * information to foreign countries or providing access to foreign
# * persons.
# *
DIRNAME="`dirname $0`"
# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
if [ -z ${BUILD_ROOT} ]
then
    export BUILD_ROOT="`cd ${DIRNAME}/../..; pwd`"
fi
echo "BUILD_ROOT is: ${BUILD_ROOT}"
export PYTHONPATH="${BUILD_ROOT}/Fw/Python/src:${BUILD_ROOT}/Gds/src"
python -m fprime_gds.executables.run_deployment "$@"
