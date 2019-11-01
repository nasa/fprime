#!/bin/bash
if (( $# < 2 ))
then
    echo "[ERROR] Failed to supply CMake dir and make targets" 1>&2
    exit 1
fi
WORKDIR="$1"
shift
JOBS="$2"
shift
TARGETS="$@"
(
    cd ${WORKDIR}
    # Loop through all targets and ensure that the targets run
    for target in ${TARGETS}
    do
        fprime-util --jobs "${JOBS}" "${target}"
        if (( $? != 0 ))
        then
            echo "[ERROR] Failed to run 'fprime-util --jobs ${JOBS} ${target}' in: ${WORKDIR}" 1>&2
            exit 1
        fi
    done
)
RET=$?
(
    cd ${WORKDIR}
    fprime-util --force purge
)    
exit $RET

