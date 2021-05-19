#!/bin/bash
####
# 01-base-cmake.bash
#
# 
####
export SCRIPT_DIR="$(dirname ${BASH_SOURCE})/.."
. "${SCRIPT_DIR}/helpers.bash"

# Set arguments if CTEST_ARGS
CTEST_ARGS=""
which valgrind > /dev/null
if (( $? == 0 ))
then
    CTEST_ARGS="-T memcheck"
fi
# Build just the framework in this test to prove that it works. Can add deployments in the following
# list when needed.
for build in "" 
do
    BUILD_DIR="${FPRIME_DIR:-${SCRIPT_DIR}/../../}/${build}/build-cmake-test"
    (
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_BUILD_TYPE=Testing .. || fail_and_stop "Failed to do basic CMake cache generation"
        cmake --build . --target all -- -j31 || fail_and_stop "Failed to do basic CMake build"
        ctest ${CTEST_ARGS} || fail_and_stop "Failed to run unit-tests"
    )
    rm -rf "${BUILD_DIR}"
done


