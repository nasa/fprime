#!/bin/sh
# **********************************************************************
# *
export LD_LIBRARY_PATH="${PERL_LIB}:${LD_LIBRARY_PATH}"
"${PERL_BIN}" "${BUILD_ROOT}/mk/bin/xml_validate.pl" "$@"

