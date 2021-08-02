#!/bin/sh

# Exit when any command fails
set -e
set -x
cd "$GITHUB_WORKSPACE"
if [ "$GITHUB_WORKFLOW" == "Autodocs" ]
then
    /autodoc.bash
else
    #echo "$INPUT_TEST1"
    "$GITHUB_WORKSPACE/ci/tests/$INPUT_TEST.bash"
fi
