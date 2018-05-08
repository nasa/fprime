#!/bin/csh
# *******************************************************************************
# * Copyright 2013, by the California Institute of Technology.
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

# remove me
 
if !($?BUILD_ROOT) then
	set curdir = "${PWD}"
	setenv BUILD_ROOT `dirname $0`/..
	cd $BUILD_ROOT
	setenv BUILD_ROOT ${PWD}
	cd ${curdir}
endif

echo "BUILD_ROOT is: ${BUILD_ROOT}"

setenv PYTHON_BASE /usr

setenv LD_LIBRARY_PATH ${PYTHON_BASE}/lib:/usr/lib:/lib
setenv PATH ${PATH}:/usr/bin:/bin
setenv PYTHONPATH ${BUILD_ROOT}/ptf
${PYTHON_BASE}/bin/python ${BUILD_ROOT}/ptf/scripts/framework/runtests.py $*
