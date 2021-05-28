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
####
# run_tool.sh:
#
# Helper script that runs one of the tools associated with the tkgui
# layer. 
####
DIRNAME="$(dirname "$0")"
# Set BUILD_ROOT if unset or "" set the BUILD_ROOT to be the above dir
if [ -z "${BUILD_ROOT}" ]
then
    BUILD_ROOT="$(cd "${DIRNAME}/../../.." || exit; pwd)"
    export
fi
echo "BUILD_ROOT is: ${BUILD_ROOT}"

# Get binary output path
NATIVE_BUILD="$(make -f "${BUILD_ROOT}/mk/makefiles/build_vars.mk print_native_build")"
export NATIVE_BUILD
echo "NATIVE_BUILD: ${NATIVE_BUILD}"
OUTPUT_DIR="$(make -f "${BUILD_ROOT}/mk/makefiles/build_vars.mk BUILD=${NATIVE_BUILD} print_output_dir")"
export OUTPUT_DIR
echo "OUTPUT_DIR: ${OUTPUT_DIR}"

PYTHONPATH="${BUILD_ROOT}/Fw/Python/src:${BUILD_ROOT}/Gds/src"
export PYTHONPATH
python -m fprime_gds.wxgui.tools.run_cmds "$@"
