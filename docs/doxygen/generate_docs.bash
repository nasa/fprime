#!/bin/bash
####
# generate_docs.sh: A crude wrapper for generating fprime documents for API documents.
####
SOURCE_DIR=`dirname $BASH_SOURCE`

DOXYGEN="doxygen"
FPRIME=`cd ${SOURCE_DIR}/../../; pwd`

DOXY_OUTPUT="${FPRIME}/docs/UsersGuide/api/c++"
CMAKE_OUTPUT="${FPRIME}/docs/UsersGuide/api/cmake"
PY_OUTPUT="${FPRIME}/docs/UsersGuide/api/python"

FPRIME_UTIL=`which fprime-util`
if [[ "${FPRIME_UTIL}" == "" ]]
then
    echo "[ERROR] Cannot run docs gen without sourcing an fprime virtual environment"
    exit 1
fi

# Doxygen generation
(
    cd "${FPRIME}"
    if [ -e "${DOXY_OUTPUT}.prev" ]
    then
        echo "[ERROR] Backup already exists at ${DOXY_OUTPUT}.prev"
        exit 1
    fi
    fprime-util build -j32
    if (( $? != 0 ))
    then
        echo "[ERROR] Failed to build fprime please generate build cache"
        exit 2 
    fi
    mv "${DOXY_OUTPUT}" "${DOXY_OUTPUT}.prev"
    ${DOXYGEN} "${FPRIME}/docs/doxygen/Doxyfile"
) || exit 1

# CMake
(
    cd "${FPRIME}"
    if [ -e "${CMAKE_OUTPUT}.prev" ]
    then
        echo "[ERROR] Backup already exists at ${CMAKE_OUTPUT}.prev"
        exit 1
    fi
    mv "${CMAKE_OUTPUT}" "${CMAKE_OUTPUT}.prev"
    mkdir -p "${CMAKE_OUTPUT}"
    "${FPRIME}/cmake/docs/docs.py" "${FPRIME}/cmake/" "${FPRIME}/docs/UsersGuide/api/cmake"
) || exit 1

# Python
(
    cd "${FPRIME}/Fw/Python/docs"
    if [ -e "${PY_OUTPUT}.prev" ]
    then
        echo "[ERROR] Backup already exists at ${PY_OUTPUT}.prev"
        exit 1
    fi
    mv "${PY_OUTPUT}" "${PY_OUTPUT}.prev"
    "${FPRIME}/Fw/Python/docs/gendoc.bash"
    cd "${FPRIME}/Gds/docs"
    "${FPRIME}/Gds/docs/gendoc.bash"
) || exit 1

