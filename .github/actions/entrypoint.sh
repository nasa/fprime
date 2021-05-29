#!/bin/sh

# Exit when any command fails
set -e

cd "$GITHUB_WORKSPACE"
"$GITHUB_WORKSPACE/ci/master.bash" QUICK
"$GITHUB_WORKSPACE/ci/master.bash" STATIC
