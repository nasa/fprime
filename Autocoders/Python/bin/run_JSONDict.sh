#!/bin/sh
DIRNAME="`dirname $0`"
# Set BUILD_ROOT if undefined and print the result
echo "BUILD_ROOT is: ${BUILD_ROOT=`cd ${DIRNAME}/../..; pwd`}"
export BUILD_ROOT

export PYTHONPATH="${BUILD_ROOT}/Autocoders/Python/src"
echo "PYTHONPATH: ${PYTHONPATH}"
python "${BUILD_ROOT}/Autocoders/Python/bin/JSONDictionaryGen.py" "$@"
