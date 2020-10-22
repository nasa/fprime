#!/bin/bash

# A quick file for generating docs
DIRECTORY="../../../docs/UsersGuide/api/python/fprime/html"
rm -r "${DIRECTORY}"
mkdir -p "${DIRECTORY}"

sphinx-build . "${DIRECTORY}"
(
    cd "${DIRECTORY}"
    for reference in _*
    do
        newref="${reference#_}"
        mv "${reference}" "${newref}"
        sed -i.bakobak 's/'${reference}'/'${newref}'/g' `find . -type f -exec grep -Iq . {} \; -print`
    done
    find . -name "*.bakobak" -delete
)
