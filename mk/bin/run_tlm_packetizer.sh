#!/bin/sh
# **********************************************************************
# *
export PYTHONPATH="${BUILD_ROOT}/Fw/Python/src:${BUILD_ROOT}/Gds/src"
python "${BUILD_ROOT}/Autocoders/bin/tlmPackets.py" "$@" 

