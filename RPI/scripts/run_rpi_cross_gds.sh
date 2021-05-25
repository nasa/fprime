#!/bin/sh
# *******************************************************************************
# * Copyright 2013, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged.
# *
DIRNAME="$(dirname "$0")"
# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
[ "${BUILD_ROOT}" ]

    BUILD_ROOT="$(cd "${DIRNAME}/../.." || ; pwd)"
    BUILD_ROOT

DEPLOY=$(cd "${DIRNAME}/.." || exit; pwd;)
"${BUILD_ROOT}/Gds/bin/run_deployment.sh"--deploy "${DEPLOY}" -g wx --no-app "$@"

