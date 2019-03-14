#!/bin/sh
# **********************************************************************
# *
export PYTHONPATH="${BUILD_ROOT}/actools"
python "${BUILD_ROOT}/mk/bin/run_file_crc.py" "$@"

