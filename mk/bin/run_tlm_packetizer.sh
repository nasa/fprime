#!/bin/sh
# **********************************************************************
# *
export PYTHONPATH="${BUILD_ROOT}/actools:${BUILD_ROOT}/Gse/src"
python "${BUILD_ROOT}/Autocoders/bin/tlmPackets.py" "$@" 

