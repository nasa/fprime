#!/bin/bash

# A quick file for generating docs
DIRECTORY="../../docs/UsersGuide/api/python/fprime-gds/html"
mkdir -p "${DIRECTORY}"

sphinx-build . "${DIRECTORY}"
