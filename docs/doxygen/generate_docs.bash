#!/bin/bash
####
# generate_docs.sh: A crude wrapper for generating fprime documents for API documents.
####
SOURCE_DIR=`dirname $BASH_SOURCE`

FPRIME=`cd ${SOURCE_DIR}/../../; pwd`
APIDOCS="${FPRIME}/docs/UsersGuide/api"

DOXY_OUTPUT="${FPRIME}/docs/UsersGuide/api/c++"
CMAKE_OUTPUT="${FPRIME}/docs/UsersGuide/api/cmake"
PY_OUTPUT="${FPRIME}/docs/UsersGuide/api/python"

DOXYGEN="${1:-$(which doxygen)}"
if [ ! -x "${DOXYGEN}" ]
then
    echo "[ERROR] Could not find doxygen, please supply it as first argument"
    exit 221
fi

FPRIME_UTIL=`which fprime-util`
if [[ "${FPRIME_UTIL}" == "" ]]
then
    echo "[ERROR] Cannot run docs gen without sourcing an fprime virtual environment"
    exit 1
fi

function github_page_adjustment
{
    DIRECTORY="${1}"
    shift
    echo "[INFO] Munching '_'s in ${DIRECTORY}" 
    python "${SOURCE_DIR}/gh_pages.py" "${DIRECTORY}"
}
function clobber
{
    DIRECTORY="${1}"
    shift
    if [[ "${DIRECTORY}" != ${APIDOCS}* ]]
    then
        echo "[ERROR] Cannot clobber ${DIRECTORY} as it is not a child of ${APIDOCS}"
        exit 233
    fi 
    read -p "[ENTER] Clobbering ${DIRECTORY}. CTRL-C to abort." tmp
    rm -r "${DIRECTORY}"
}

# Doxygen generation
(
    cd "${FPRIME}"
    clobber "${DOXY_OUTPUT}"
    (
        mkdir -p "${FPRIME}/build-fprime-automatic-docs"
        cd "${FPRIME}/build-fprime-automatic-docs"
        cmake "${FPRIME}" -DCMAKE_BUILD_TYPE=RELEASE
    )
    fprime-util build "docs" -j32
    if (( $? != 0 ))
    then
        echo "[ERROR] Failed to build fprime please generate build cache"
        exit 2 
    fi
    ${DOXYGEN} "${FPRIME}/docs/doxygen/Doxyfile"
    rm -r "${FPRIME}/build-fprime-automatic-docs"
) || exit 1

# CMake
(
    cd "${FPRIME}"
    clobber "${CMAKE_OUTPUT}"
    mkdir -p "${CMAKE_OUTPUT}"
    "${FPRIME}/cmake/docs/docs.py" "${FPRIME}/cmake/" "${FPRIME}/docs/UsersGuide/api/cmake"
) || exit 1

# Python
(
    clobber "${PY_OUTPUT}"
    cd "${FPRIME}/Fw/Python/docs"
    "${FPRIME}/Fw/Python/docs/gendoc.bash"
    cd "${FPRIME}/Gds/docs"
    "${FPRIME}/Gds/docs/gendoc.bash"
) || exit 1

# Fix for github pages
github_page_adjustment "${DOXY_OUTPUT}/html"
github_page_adjustment "${PY_OUTPUT}/fprime/html"
github_page_adjustment "${PY_OUTPUT}/fprime-gds/html"
