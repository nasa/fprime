#!/bin/bash
let SLEEP_TIME=10
if [[ "$1" == "" ]] || [ ! -d  $1 ]
then
   echo "[ERROR] '$1' does not exist"
   exit 2
fi
# Start the GDS layer and give it time to run
echo "[INFO] Starting headless GDS layer"
fprime-gds -d "${1}" -g none 1>/dev/null 2>/dev/null &
GDS_PID=$!
echo "[INFO] Allowing GDS ${SLEEP_TIME} seconds to start"
sleep ${SLEEP_TIME}
# Check the above started successfully
ps -p ${GDS_PID} 2> /dev/null 1> /dev/null
if (( $? != 0 ))
then
    echo "[ERROR] Failed to start GDS"
    exit 1
fi
# Run integration tests
(
    cd "${1}"
    echo "[INFO] Running $1's pytest integration tests" 
    pytest
)
RET_PYTEST=$?
kill $GDS_PID
sleep 2
pkill -KILL Ref
exit ${RET_PYTEST}
