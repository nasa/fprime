#!/bin/sh
# **********************************************************************
# *
export PYTHONPATH="${BUILD_ROOT}/actools"
python "${BUILD_ROOT}/mk/bin/echo_args.py" "$@" 
