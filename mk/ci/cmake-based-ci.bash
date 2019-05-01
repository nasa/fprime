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
cmake .. -DPLATFORM=ut/`uname` && make -j32 && make -j32 check
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake and process UTs for 'Ref'"
    exit 2
fi

for target in `make help | grep "exe"`
do
    (
        mkdir ut_${target}
        cd ut_${target}
        cmake ../.. -DPLATFORM=ut/`uname` && make ${target} -j32 && make test ${target}
    )
done

# Change to the CMake test directory and run CMake UTs
cd ..
cd cmake/test/src
# Note: custom target and validation are currently broken due to unimplemented features
pytest --ignore test_custom_target.py --ignore test_validation.py  --ignore test_ut.py --ignore test_tools_validation.py
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake unit tests"
    exit 2
fi
exit 0
