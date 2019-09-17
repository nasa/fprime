#!/bin/sh
# *******************************************************************************
# * Copyright 2013, by the California Institute of Technology.
# * ALL RIGHTS RESERVED. United States Government Sponsorship
# * acknowledged.
# *

DIRNAME="`dirname $0`"

# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
if [ -z ${BUILD_ROOT} ]
then
    export BUILD_ROOT="`cd ${DIRNAME}/../..; pwd`"
fi

# Get binary output path
export OUTPUT_DIR="`make -f ${BUILD_ROOT}/mk/makefiles/build_vars.mk BUILD=RASPIAN print_output_dir`"
echo "OUTPUT_DIR: ${OUTPUT_DIR}"

# Set python path
export PYTHONPATH="${BUILD_ROOT}/Gds/src/fprime_gds/tkgui/tools/:${BUILD_ROOT}/Gds/src/:${BUILD_ROOT}/Fw/Python/src/"

DEPLOY=`cd ${DIRNAME}/..; pwd;`
${BUILD_ROOT}/Gds/bin/run_deployment.sh --deploy "${DEPLOY}" "$@"
