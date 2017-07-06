#!/bin/csh
# **********************************************************************
# *

setenv LD_LIBRARY_PATH ${PERL_LIB}:/tps/lib:/usr/lib:/lib
setenv PATH /tps/bin:/usr/bin:/bin
${PERL_BIN} ${BUILD_ROOT}/actools/bin/create_build_id $* 

