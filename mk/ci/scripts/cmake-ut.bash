#!/bin/bash
if (( $# < 2 ))
then
    echo "[ERROR] Failed to supply CMake dir and UT targets" 1>&2
    exit 1
fi
DIRNAME="$(dirname $BASH_SOURCE)"
TARGET="$(cd $(dirname $1); pwd)/$(basename $1)"
shift
UT="${1}"
shift

DIR="${BUILD_DIR:-$(mktemp -d)}"
(
    echo "[INFO] Running UT: ${UT}"
    BUILD_DIR="${DIR}" CARGS="-DCMAKE_BUILD_TYPE=TESTING" "${DIRNAME}"/cmake-build.bash "${TARGET}" "${UT}" > "${DIR}/ut.out" 2> >(tee -a "${DIR}/ut.err" >&2) || exit 3
    cd "$DIR"
    if [[ "${UT}" != "check" ]]
    then
        ARGS="-R ${UT}" make test || exit 3
    fi 
)
RET=$?
if [[ "${BUILD_DIR}" == "" ]]
then
    rm -r "${DIR}"
fi
exit $RET

