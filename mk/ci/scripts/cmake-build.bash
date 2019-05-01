#!/bin/bash
if (( $# < 2 ))
then
    echo "[ERROR] Failed to supply CMake dir and make targets" 1>&2
    exit 1
fi
TARGET="$(cd $(dirname $1); pwd)/$(basename $1)"
shift
echo "$@"
DIR="${BUILD_DIR:-$(mktemp -d)}"
(
    echo "[INFO] Building in: ${DIR}"
    cd "$DIR"
    cmake ${CARGS} "${TARGET}" > "${DIR}/cmake.out" 2> >(tee -a "${DIR}/cmake.err" >&2)
    if (( $? != 0 ))
    then
        echo "[ERROR] Failed to generate CMake: ${TARGET} with ${ARGS}" 1>&2
        exit 2
    fi
    for MK_TAR in "$@"
    do
        make ${MK_TAR} -j${JOBS:-1} > "${DIR}/mk-${MK_TAR}.out" 2> >(tee -a "${DIR}/mk-${MK_TAR}.err" >&2)
        if (( $? != 0 ))
        then
            echo "[ERROR] Failed to make '${MK_TAR}': ${TARGET} with ${ARGS}" 1>&2
            exit 3
        fi
    done
)
RET=$?
if [[ "${BUILD_DIR}" == "" ]]
then
    rm -r "${DIR}"
fi
exit $RET

