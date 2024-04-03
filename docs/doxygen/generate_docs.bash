#!/bin/bash
####
# generate_docs.sh: A crude wrapper for generating fprime documents for API documents.
####
SOURCE_DIR=`dirname $BASH_SOURCE`

FPRIME=`cd ${SOURCE_DIR}/../../; pwd`
APIDOCS="${FPRIME}/docs/UsersGuide/api"

DOXY_OUTPUT="${FPRIME}/docs/UsersGuide/api/c++"
CMAKE_OUTPUT="${FPRIME}/docs/UsersGuide/api/cmake"

VERSIONED_OUTPUT="${1:-}"

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
    rm -r "${DIRECTORY}"
}

function make_version
{
    VERSION="${1}"
    if [[ "$VERSION" != "" ]]
    then
        mkdir "${FPRIME}/docs/${VERSION}"
        cp "${FPRIME}/docs/latest.md" "${FPRIME}/docs/${VERSION}/index.md"
        cp -r "${FPRIME}/docs/INSTALL.md" "${FPRIME}/docs/Tutorials" "${FPRIME}/docs/UsersGuide" "${FPRIME}/docs/Design" "${FPRIME}/docs/Design/HowTo" "${FPRIME}/docs/${VERSION}"
        REPLACE='| ['$VERSION' Documentation](https:\/\/nasa.github.io\/fprime\/'$VERSION') |\n'
    else
        echo "No version specified, updating local only"
    fi

    sed -i.backup 's/\(| \[Latest Documentation\](.\/latest.md)\)[^|]*|/'"$REPLACE"'\1 As of: '"$(date)"' |/' "${FPRIME}/docs/index.md"

}

# Doxygen generation
(
    cd "${FPRIME}"
    DOCS_CACHE="${FPRIME}/docs-cache"
    clobber "${DOXY_OUTPUT}"
    echo "[INFO] Building fprime"
    rm -rf "${DOCS_CACHE}"

    fprime-util generate --build-cache ${DOCS_CACHE} -DCMAKE_BUILD_TYPE=Release -DFPRIME_SKIP_TOOLS_VERSION_CHECK=ON 1>/dev/null
    fprime-util build --build-cache ${DOCS_CACHE} --all -j32 1> /dev/null

    if (( $? != 0 ))
    then
        echo "[ERROR] Failed to build fprime please generate build cache"
        exit 2
    fi
    mkdir -p ${DOXY_OUTPUT}

    # Replace version number in Doxyfile
    if [[ "${VERSIONED_OUTPUT}" != "" ]]
    then
        sed -i "s/^PROJECT_NUMBER[ ]*=.*$/PROJECT_NUMBER=${VERSIONED_OUTPUT}/g" docs/doxygen/Doxyfile
    fi

    ${DOXYGEN} "${FPRIME}/docs/doxygen/Doxyfile"
    rm -r "${DOCS_CACHE}"
) || exit 1

# CMake
(
    cd "${FPRIME}"
    clobber "${CMAKE_OUTPUT}"
    mkdir -p "${CMAKE_OUTPUT}"
    "${FPRIME}/cmake/docs/docs.py" "${FPRIME}/cmake/" "${CMAKE_OUTPUT}"
    "${FPRIME}/docs/doxygen/index_gen.py" "${CMAKE_OUTPUT}" 'CMake API Index' 'test,googletest-download' > "${CMAKE_OUTPUT}/index.md"
) || exit 1

# Generate full index
(
    cd "${FPRIME}"
    "${FPRIME}/docs/doxygen/index_gen.py" "${FPRIME}/docs" "F´ Documentation Index" "_data,_includes,doxygen,img" > "${FPRIME}/docs/doc-index.md"
) || exit 1

# Fix for github pages
github_page_adjustment "${DOXY_OUTPUT}/html"
make_version "${VERSIONED_OUTPUT}"

# Copy images so they're properly referenced
IMG_DIR="${FPRIME}/docs/UsersGuide/api/c++/html/img"
mkdir -p "${IMG_DIR}"
for image in $(find "${FPRIME}/Fw" "${FPRIME}/Svc" "${FPRIME}/Drv" \( -name '*.jpg' -o -name '*.png' -o -name '*.svg' \))
do
    cp "${image}" "${IMG_DIR}"
done
