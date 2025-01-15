#!/bin/bash
####
# generate_docs.sh: A crude wrapper for generating fprime documents for API documents.
# This generates both the doxygen (C++) and CMake API documentation for fprime.
####
SOURCE_DIR=`dirname $BASH_SOURCE`

FPRIME=`cd ${SOURCE_DIR}/../../; pwd`
APIDOCS="${FPRIME}/docs/documentation/reference/api"

DOXY_OUTPUT="${APIDOCS}/cpp"
CMAKE_OUTPUT="${APIDOCS}/cmake"

DOXYGEN="${2:-$(which doxygen)}"
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

function clobber
{
    DIRECTORY="${1}"
    shift
    if [[ "${DIRECTORY}" != ${APIDOCS}* ]]
    then
        echo "[ERROR] Cannot clobber ${DIRECTORY} as it is not a child of ${APIDOCS}"
        exit 233
    fi
    rm -r "${DIRECTORY}"
}

# Doxygen generation
(
    cd "${FPRIME}"
    DOCS_CACHE="${FPRIME}/docs-cache"
    clobber "${DOXY_OUTPUT}"
    echo "[INFO] Building fprime"
    rm -rf "${DOCS_CACHE}"

    fprime-util generate --build-cache ${DOCS_CACHE} 1> /dev/null
    fprime-util build --build-cache ${DOCS_CACHE} --all -j32 1> /dev/null

    if (( $? != 0 ))
    then
        echo "[ERROR] Failed to build fprime please generate build cache"
        exit 2
    fi
    mkdir -p ${DOXY_OUTPUT}

    ${DOXYGEN} "${FPRIME}/docs/doxygen/Doxyfile"
    rm -r "${DOCS_CACHE}"
) || exit 1

# CMake
(
    cd "${FPRIME}"
    clobber "${CMAKE_OUTPUT}"
    mkdir -p "${CMAKE_OUTPUT}"
    "${FPRIME}/cmake/docs/docs.py" "${FPRIME}/cmake/" "${CMAKE_OUTPUT}"
) || exit 1


# Copy images so they're properly referenced
IMG_DIR="${DOXY_OUTPUT}/html/img"
mkdir -p "${IMG_DIR}"
for image in $(find "${FPRIME}/Fw" "${FPRIME}/Svc" "${FPRIME}/Drv" \( -name '*.jpg' -o -name '*.png' -o -name '*.svg' \))
do
    cp "${image}" "${IMG_DIR}"
done


# Aggregate and index SDDs so they are rendered in the website
python3 "${FPRIME}/docs/doxygen/sdd_processing.py"
