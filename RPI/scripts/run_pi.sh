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

if !($?BUILD_ROOT) then
    set curdir = "${PWD}"
    setenv BUILD_ROOT `dirname $0`/../..
    cd $BUILD_ROOT
    setenv BUILD_ROOT ${PWD}
    cd ${curdir}
endif

echo "BUILD_ROOT is: ${BUILD_ROOT}"

# Get binary output path
export OUTPUT_DIR="`make -f ${BUILD_ROOT}/mk/makefiles/build_vars.mk BUILD=RASPIAN print_output_dir`"
echo "OUTPUT_DIR: ${OUTPUT_DIR}"

export PYTHONPATH="${BUILD_ROOT}/Gse/src:${BUILD_ROOT}/Gse/generated/FlightDemo/serializable"
python "${BUILD_ROOT}/RPI/scripts/run_pi.py" "$@"
