#!/bin/bash

# Exit when any command fails
set -e
set -x

cd "$GITHUB_WORKSPACE"
REMOTE="${1:-origin}"
git config --local user.email "nasa-fprime[bot]@users.noreply.github.com"
git config --local user.name "nasa-fprime[bot]"
git fetch "${REMOTE}" docs/auto-documentation
git fetch "${REMOTE}" devel
git checkout docs/auto-documentation
GIT_EDITOR=true git merge "${REMOTE}"/devel
${GITHUB_WORKSPACE}/docs/doxygen/generate_docs.bash
git add -Af "${GITHUB_WORKSPACE}/docs"

# Only attempt to push if we are on the devel branch in GitHub Actions
# see https://docs.github.com/en/actions/learn-github-actions/variables
if [ "${GITHUB_REF_NAME}" == "devel" ]; then
    git commit -m "Autodoc on $(date)" || echo "No new commits, pushing merge"
    git push "${REMOTE}"
    exit 0
else
    echo "Not on devel branch, skipping push."
    exit 0
fi
