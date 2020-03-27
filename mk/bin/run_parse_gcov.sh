#!/bin/sh
# **********************************************************************
# *
export LD_LIBRARY_PATH="${PERL_LIB}:${LD_LIBRARY_PATH}"
"${PERL_BIN}" "${BUILD_ROOT}/mk/bin/parse_gcov.pl" "$@"

