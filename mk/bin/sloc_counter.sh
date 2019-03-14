#!/bin/sh
# **********************************************************************
# *
export PYTHONPATH="${BUILD_ROOT}/actools"
python "${BUILD_ROOT}/mk/bin/sloc.py" "$@"

