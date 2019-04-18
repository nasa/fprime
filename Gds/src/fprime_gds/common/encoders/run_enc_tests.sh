#!/bin/csh
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
    setenv BUILD_ROOT `dirname $0`/../../..
    cd $BUILD_ROOT
    setenv BUILD_ROOT ${PWD}
    cd ${curdir}
endif

echo "BUILD_ROOT is: ${BUILD_ROOT}"

# Borrow some variables from build
setenv PYTHON_BASE `make -f ${BUILD_ROOT}/mk/makefiles/build_vars.mk print_python_base`
echo "PYTHON_BASE: ${PYTHON_BASE}"

# Get binary output path
setenv NATIVE_BUILD `make -f ${BUILD_ROOT}/mk/makefiles/build_vars.mk print_native_build`
echo "NATIVE_BUILD: ${NATIVE_BUILD}"
setenv OUTPUT_DIR `make -f ${BUILD_ROOT}/mk/makefiles/build_vars.mk BUILD=$NATIVE_BUILD print_output_dir`
echo "OUTPUT_DIR: ${OUTPUT_DIR}"

#setenv PYTHON_BASE /proj/dieb/fsw/tools/python

setenv LD_LIBRARY_PATH ${PYTHON_BASE}/lib
setenv PYTHONPATH ${BUILD_ROOT}/Gds/src:${BUILD_ROOT}/Gse/generated
echo ${PYTHONPATH}


echo ""
echo "Running Channel Encoder Tests..."
${PYTHON_BASE}/bin/python ${BUILD_ROOT}/Gds/src/encoders/ch_encoder.py

echo ""
echo "Running Event Encoder Tests..."
${PYTHON_BASE}/bin/python ${BUILD_ROOT}/Gds/src/encoders/event_encoder.py

echo ""
echo "Running Packet Encoder Tests..."
${PYTHON_BASE}/bin/python ${BUILD_ROOT}/Gds/src/encoders/pkt_encoder.py

