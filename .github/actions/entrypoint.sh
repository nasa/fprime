#!/bin/sh
cd "$GITHUB_WORKSPACE"
"$GITHUB_WORKSPACE/mk/ci/master.bash" QUICK
"$GITHUB_WORKSPACE/mk/ci/master.bash" STATIC
