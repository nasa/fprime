#!/bin/sh

# Exit when any command fails
set -e
set -x
cd "$GITHUB_WORKSPACE"
if [ "$GITHUB_WORKFLOW" == "Autodocs" ]
then
    /autodoc.bash
else
    echo $1
    #"$GITHUB_WORKSPACE/ci/master.bash" $1   
fi
