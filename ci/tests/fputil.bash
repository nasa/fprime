#!/bin/bash
####
# fputil.bash:
#
# Helpers to test via FP util
####
set -e
export FPUTIL_TARGETS=("generate" "generate --ut" "build" "build --all" "check --all")
####
# fputil_action:
#
# Runs an action for the FP util. This takes two parameters a target and a deployment. This assumes
# prerequisite actions already exist.
# :param target($1): command to run with FP util
# :param deploy($2): deployment to run on
####
function fputil_action {
    export WORKDIR="${1}"
    export TARGET="${2}"
    let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"
    (
        PLATFORM=""

        cd "${WORKDIR}"
        if [[ "${TARGET}" != "generate" ]] && [[ "${TARGET}" != "generate --ut" ]]
        then
	        echo "[INFO] FP Util in ${WORKDIR} running ${TARGET} with ${JOBS} jobs"
            fprime-util ${TARGET} --jobs "${JOBS}" ${PLATFORM} > "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.out.log" 2> "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.err.log" \
                || fail_and_stop "Failed to run '${TARGET}' in ${WORKDIR}"
        else
	        echo "[INFO] FP Util in ${WORKDIR} running ${TARGET}"
            fprime-util ${TARGET} ${PLATFORM} ${CMAKE_EXTRA_SETTINGS} > "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.out.log" 2> "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.err.log" \
                || fail_and_stop "Failed to run '${TARGET}' in ${WORKDIR}"
        fi
    ) || exit 1
}
export -f fputil_action
####
# integration_test:
#
# Runs the FPrime GDS and integration test layer for a deployment.
# :param deploy($1): deployment to run on.
####
function integration_test {
    export WORKDIR="${1}"
    let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"

    CMAKE_EXTRA_SETTINGS=""
    PLATFORM=""

    cd "${WORKDIR}"
    fprime-util "generate" > "${LOG_DIR}/${WORKDIR//\//_}_pregen.out.log" 2> "${LOG_DIR}/${WORKDIR//\//_}_pregen.err.log" \
        || fail_and_stop "Failed to generate before ${WORKDIR//\//_} building integration test"
    cd "${WORKDIR}/"
    fprime-util "build" --jobs "${JOBS}" ${PLATFORM} > "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.out.log" 2> "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.err.log" \
        || fail_and_stop "Failed to build before integration test"

    integration_test_run "${WORKDIR}"
}
export -f integration_test

function integration_test_run {
    export SLEEP_TIME="10"
    export WORKDIR="${1}"
    export BINARY=`basename "${WORKDIR}"`
    export ROOTDIR="${WORKDIR}/build-artifacts"
    (
        cd "${WORKDIR}"
        mkdir -p "${LOG_DIR}/gds-logs"
        # Start the GDS layer and give it time to run
        echo "[INFO] Starting headless GDS layer"
        fprime-gds -n --dictionary "${ROOTDIR}/"*"/dict/${BINARY}TopologyAppDictionary.xml" -g none -l "${LOG_DIR}/gds-logs" 1>${LOG_DIR}/gds-logs/fprime-gds.stdout.log 2>${LOG_DIR}/gds-logs/fprime-gds.stderr.log &
        GDS_PID=$!
        # run the app with valgrind in the background
        if command -v valgrind &> /dev/null
        then
            valgrind  \
                --tool=memcheck \
                --error-exitcode=1 \
                --verbose \
                --leak-check=full \
                --show-leak-kinds=all \
                --track-origins=yes \
                --log-file=${LOG_DIR}/gds-logs/valgrind.log \
            ${ROOTDIR}/*/bin/${BINARY} -a 127.0.0.1 -p 50000 1>${LOG_DIR}/gds-logs/${BINARY}.stdout.log 2>${LOG_DIR}/gds-logs/${BINARY}.stderr.log &
        else
            ${ROOTDIR}/*/bin/${BINARY} -a 127.0.0.1 -p 50000 1>${LOG_DIR}/gds-logs/${BINARY}.stdout.log 2>${LOG_DIR}/gds-logs/${BINARY}.stderr.log &
        fi
        VALGRIND_PID=$!

        echo "[INFO] Allowing GDS ${SLEEP_TIME} seconds to start"
        sleep ${SLEEP_TIME}
        # Check the above started successfully
        ps -p ${GDS_PID} 2> /dev/null 1> /dev/null || fail_and_stop "Failed to run GDS layer headlessly"
        ps -p ${VALGRIND_PID} 2> /dev/null 1> /dev/null || fail_and_stop "Failed to start ${BINARY} with Valgrind"
        # Run integration tests
        (
            cd "${WORKDIR}"
            if [[ "${DICTIONARY_PATH}" != "" ]]
            then
                DICTIONARY_ARGS="--dictionary ${WORKDIR}/${DICTIONARY_PATH}"
            fi
            echo "[INFO] Running ${WORKDIR}/test's pytest integration tests"
            TIMEOUT="timeout"
            if ! command -v ${TIMEOUT} &> /dev/null
            then
                TIMEOUT="gtimeout" # macOS homebrew "coreutils"
            fi
            ${TIMEOUT} --kill-after=10s 180s pytest ${DICTIONARY_ARGS}
        )
        RET_PYTEST=$?
        pkill -P $GDS_PID
        kill $GDS_PID
        sleep 2
        # Kill Valgrind and get exit code
        pkill -P $VALGRIND_PID
        kill $VALGRIND_PID
        wait $VALGRIND_PID
        RET_MEMTEST=$?
        # Report memory leaks if they occurred and the pytests were successful
        if [ ${RET_MEMTEST} -ne 0 ] && [ ${RET_PYTEST} -eq 0 ]; then
            cat "${LOG_DIR}/gds-logs/valgrind.log"
            fail_and_stop "Integration tests on ${WORKDIR} contain memory leaks"
        fi

        pkill -KILL bin/${BINARY}
        exit ${RET_PYTEST}
    ) || fail_and_stop "Failed integration tests on ${WORKDIR}"
}
export -f integration_test_run



