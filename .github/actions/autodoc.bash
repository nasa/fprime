#!/bin/bash

# Exit when any command fails
set -e
set -x

cd "$GITHUB_WORKSPACE"
REMOTE="${1:-origin}"
git config --local user.email "nasa-fprime[bot]@users.noreply.github.com"
git config --local user.name "nasa-fprime[bot]"
git fetch "${REMOTE}" release/documentation
git fetch "${REMOTE}" devel
git checkout release/documentation
GIT_EDITOR=true git merge "${REMOTE}"/devel
${GITHUB_WORKSPACE}/docs/doxygen/generate_docs.bash
git add -Af "${GITHUB_WORKSPACE}/docs"


git commit -m "Autodoc on $(date)" || echo "No new commits, pushing merge"
git push "${REMOTE}"
