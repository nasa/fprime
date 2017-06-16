#!/bin/csh
# **********************************************************************
# *

setenv LD_LIBRARY_PATH ${PYTHON_BASE}/lib:/tps/lib:/usr/lib:/lib
setenv PATH /tps/bin:/usr/bin:/bin
${CKSUM} $1 > $2
${CAT} $2
 
