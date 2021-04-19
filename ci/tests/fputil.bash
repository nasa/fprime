#!/bin/bash
####
# fputil.bash:
#
# Helpers to test via FP util
####
export FPUTIL_TARGETS=("generate" "generate --ut" "build" "build --all" "check --all")
export FPUTIL_DEPLOYS="${FPRIME_DIR} ${FPRIME_DIR}/Ref ${FPRIME_DIR}/RPI"

export INT_DEPLOYS="${FPRIME_DIR}/Ref"
####
# fputil_action:
#
# Runs an action for the FP util. This takes two parameters a target and a deployment. This assumes
# prerequisite actions already exist.
# :param target($1): command to run with FP util
# :param deploy($2): deployment to run on
####
function fputil_action {
    export DEPLOYMENT="${1}"
    export TARGET="${2}"
    export WORKDIR="${DEPLOYMENT}/${3}"
    let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"
    (
        cd "${DEPLOYMENT}"
        PLATFORM=""
        # Setup special platform for check/check-all
        if [[ "${TARGET}" == check* ]] && [[ "${DEPLOYMENT}" == */RPI ]]
        then
                PLATFORM="${CHECK_TARGET_PLATFORM}"
                if [[ "${TEST_TYPE}" == "QUICK" ]]
                then
                    echo "[INFO] Generating build cache before ${DEPLOYMENT//\//_} '${TARGET}' execution"
                    fprime-util generate --ut ${PLATFORM} > "${LOG_DIR}/${DEPLOYMENT//\//_}_pregen.out.log" 2> "${LOG_DIR}/${DEPLOYMENT//\//_}_pregen.err.log" \
                        || fail_and_stop "Failed to generate before ${DEPLOYMENT//\//_} '${TARGET}' execution"
                fi
        fi
  
        # Generate is only needed when it isn't being tested
        if [[ "${TARGET}" != "generate" ]] && [[ "${TEST_TYPE}" != "QUICK" ]]
        then
            echo "[INFO] Generating build cache before ${DEPLOYMENT//\//_} '${TARGET}' execution"
            fprime-util "generate" ${PLATFORM} > "${LOG_DIR}/${DEPLOYMENT//\//_}_pregen.out.log" 2> "${LOG_DIR}/${DEPLOYMENT//\//_}_pregen.err.log" \
                || fail_and_stop "Failed to generate before ${DEPLOYMENT//\//_} '${TARGET}' execution"
        fi
        cd "${WORKDIR}"
        if [[ "${TARGET}" != "generate" ]] && [[ "${TARGET}" != "generate --ut" ]]
        then
	    echo "[INFO] FP Util in ${WORKDIR} running ${TARGET} with ${JOBS} jobs"
            fprime-util ${TARGET} --jobs "${JOBS}" ${PLATFORM} > "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.out.log" 2> "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.err.log" \
                || fail_and_stop "Failed to run '${TARGET}' in ${WORKDIR}"
        else
	    echo "[INFO] FP Util in ${WORKDIR} running ${TARGET}"
            fprime-util ${TARGET} ${PLATFORM} > "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.out.log" 2> "${LOG_DIR}/${WORKDIR//\//_}_${TARGET/ /}.err.log" \
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
    export SLEEP_TIME="10"
    export WORKDIR="${1}"
    export ROOTDIR="${WORKDIR}/build-artifacts"
    if [[ "${TEST_TYPE}" != "QUICK" ]]
    then
        fputil_action "${WORKDIR}" "build" || fail_and_stop "Failed to build before integration test"
    fi
    (
        mkdir -p "${LOG_DIR}/gds-logs"
        # Start the GDS layer and give it time to run
        echo "[INFO] Starting headless GDS layer"
        fprime-gds -n -r "${ROOTDIR}" -g none -l "${LOG_DIR}/gds-logs" 1>${LOG_DIR}/gds-logs/fprime-gds.stdout.log 2>${LOG_DIR}/gds-logs/fprime-gds.stderr.log &
        GDS_PID=$!
        # run the app with valgrind in the background
        valgrind  \
            --tool=memcheck \
            --error-exitcode=1 \
            --verbose \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --log-file=${LOG_DIR}/gds-logs/valgrind.log \
            ${ROOTDIR}/*/bin/Ref -a 127.0.0.1 -p 50000 1>${LOG_DIR}/gds-logs/Ref.stdout.log 2>${LOG_DIR}/gds-logs/Ref.stderr.log &
        VALGRIND_PID=$!

        echo "[INFO] Allowing GDS ${SLEEP_TIME} seconds to start"
        sleep ${SLEEP_TIME}
        # Check the above started successfully
        ps -p ${GDS_PID} 2> /dev/null 1> /dev/null || fail_and_stop "Failed to run GDS layer headlessly"
        ps -p ${VALGRIND_PID} 2> /dev/null 1> /dev/null || fail_and_stop "Failed to start Ref with Valgrind"
        # Run integration tests
        (
            cd "${WORKDIR}/test"
            echo "[INFO] Running ${WORKDIR}/test's pytest integration tests" 
            timeout --kill-after=10s 180s pytest
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

        pkill -KILL Ref
        exit ${RET_PYTEST}
    ) || fail_and_stop "Failed integration tests on ${WORKDIR}"
}
export -f integration_test
