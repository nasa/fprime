#!/bin/sh

# Exit when any command fails
set -e
set -x
cd "$GITHUB_WORKSPACE"
if [ "$GITHUB_WORKFLOW" != "Autodocs" ]
then
    "$GITHUB_WORKSPACE/ci/master.bash" QUICK
else
    /autodoc.bash
fi
