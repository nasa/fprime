#!/bin/bash
mkdir build-dir && cd build-dir
if (( $? != 0 ))
then
    echo "[ERROR] Failed to make 'build-dir'"
    exit 1
fi
# Run cmake for 'Ref'
cmake ../Ref && make -j32 && make install -j32
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake and install for 'Ref'"
    exit 2
fi
# Run standard UTs via CMake
cd ..
mkdir build-dir-ut && cd build-dir-ut
cmake ../Ref -DPLATFORM=ut/`uname` && make -j32 && make -j32 check
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake and process UTs for 'Ref'"
    exit 2
fi
# Change to the CMake test directory and run CMake UTs
cd ..
cd cmake/test/src
# Note: tool-check cannot be run w/o an vritual env
pytest --ignore test_tool_check.py --ignore test_custom_target.py --ignore test_validation.py --ignore test_ut.py
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake unit tests"
    exit 2
fi
exit 0
