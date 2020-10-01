#!/bin/sh

# Exit when any command fails
set -e

cd "$GITHUB_WORKSPACE"
"$GITHUB_WORKSPACE/mk/ci/master.bash" QUICK
"$GITHUB_WORKSPACE/mk/ci/master.bash" STATIC
