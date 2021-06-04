#!/bin/sh

# Exit when any command fails
set -e
set -x
cd "$GITHUB_WORKSPACE"
if [ "$GITHUB_WORKFLOW" != "Autodocs" ]
then
    "$GITHUB_WORKSPACE/ci/master.bash" QUICK
    "$GITHUB_WORKSPACE/ci/master.bash" STATIC
else
    /autodoc.bash
fi
