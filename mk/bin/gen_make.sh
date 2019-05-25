#!/bin/sh
# **********************************************************************
# *
 
export PYTHONPATH="${BUILD_ROOT}/mk/src"
python "${BUILD_ROOT}/mk/src/genmake.py" "${BUILD_ROOT}/mk/makefiles/Makefile"

