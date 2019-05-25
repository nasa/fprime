#!/bin/sh
# *******************************************************************************
# * Copyright 2019, by the California Institute of Technology.
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
DIRNAME="`dirname $0`"
# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
BUILD_ROOT="`cd ${DIRNAME}/../../..; pwd`"
export PYTHONPATH="${DIRNAME}:${BUILD_ROOT}/Fw/Python/src:${DIRNAME}/../src"
# Run command utility
CMD="$1"
shift
if [ ! -f ${DIRNAME}/utils/${CMD}.py ]
then
    UTILS=`cd ${DIRNAME}/utils/; ls *.py | sed 's/\.py//g'`
    echo "[ERROR] '${CMD}' is not an autocoder utility. Valid utilities: ${UTILS}"
    exit 1
fi
BUILD_ROOT="${BUILD_ROOT}" python -m utils.${CMD} "$@"

