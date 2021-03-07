#!/bin/bash
####
# terminal-wrapper.bash:
#
# Terminal wrapper wraps the call to the macOS Terminal.app to run the application in an external
# window. This ensures that the application can be shutdown after a shutdown call. It must do
# the following:
#
# 1. Replicate the PYTHONPATH and PYTHON environment for running
# 2. Use osascript to start Terminal window
# 3. Run the passed in application
# 4. Track the executable and kill it on signal
# 5. Loop-forever waiting for termination signal
####

# Setup existing python path. Also, if in VIRTUAL_ENV, then source that virtual environment
# first. This will not work running a `tsch` shell on macOS 
EXTRA="PYTHONPATH='${PYTHONPATH}' "
if [[ "$VIRTUAL_ENV" != "" ]]
then
    EXTRA=". $(dirname $(which python) )/activate;${EXTRA}"
fi

# Get the existing PIDs for the given program, and then start the Terminal with that app via OSA
# script. This sets up running in the Terminal by doing running the source of the Virtual env
# and setting the PYTHONPATH before running.
ref_pids=`pgrep -a "$1" | sort -u`
osascript -e 'tell app "Terminal"
    do script "'"${EXTRA}$*"'"
end tell' 2> /dev/null 1>/dev/null
sleep 1
# Get the new PIDs and then loop through them, looking for PIDs that are not in the original set.
# These new PIDs are the PIDs that need to be killed.
ref_pids_new=`pgrep -a "$1" | sort -u`
kill_ls=""
for pid in ${ref_pids_new}
do
    if [[ "${ref_pids}" != *" ${pid} "* ]]
    then
        kill_ls="${kill_ls} ${pid}"
    fi
done
# Setup the kill trap and exit with a SIGINT and a SIGTERM. Then loop forever waiting for these
# termination signals.
trap "kill ${kill_ls}; exit" SIGINT SIGTERM
for (( ; ; ))
do
    sleep 1 
done
