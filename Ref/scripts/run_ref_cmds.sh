#!/bin/sh

DIRNAME="`dirname $0`"
# Set BUILD_ROOT if undefined and print the result
echo "BUILD_ROOT is: ${BUILD_ROOT=`cd ${DIRNAME}/../..; pwd`}"
export BUILD_ROOT

${BUILD_ROOT}/Gse/bin/run_cmds.sh --addr localhost --port 50000 --dictionary ${BUILD_ROOT}/Gse/generated/Ref "$@"
