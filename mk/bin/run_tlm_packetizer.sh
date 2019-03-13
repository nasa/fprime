#!/bin/csh
# **********************************************************************
# *

setenv LD_LIBRARY_PATH ${PYTHON_BASE}/lib:/usr/lib:/lib
setenv PATH ${PYTHON_BASE}/bin:/tps/bin:/usr/bin:/bin
setenv PYTHONPATH ${BUILD_ROOT}/actools:${BUILD_ROOT}/Gse/src
${PYTHON_BASE}/bin/python ${BUILD_ROOT}/Autocoders/bin/tlmPackets.py $* 

