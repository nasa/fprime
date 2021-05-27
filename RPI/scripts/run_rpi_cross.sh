#!/bin/sh
# *******************************************************************************
# * Copyright 2013, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged.
# *
DIRNAME="$(dirname "$0")"
# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
if [ -z "${BUILD_ROOT}" ]
then
    BUILD_ROOT="$(cd "${DIRNAME}/../.." || exit ; pwd)"
    export BUILD_ROOT
fi
DEPLOY=$(cd "${DIRNAME}/.." || exit; pwd)
"${BUILD_ROOT}/Gds/bin/run_deployment.sh" --deploy "${DEPLOY}" --no-app "$@"

