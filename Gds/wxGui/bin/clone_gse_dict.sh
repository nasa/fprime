#!/bin/bash
if [[ "${BUILD_ROOT}" == "" ]]
then
    echo "[ERROR] Required to set BUILD_ROOT"
    exit
elif [[ "${1}" == "" ]] || [ ! -d "${BUILD_ROOT}/Gse/generated/${1}" ]
then
    echo "[ERROR] Required to supply valid deployment i.e. HBS NAV"
    exit
fi
#Cleanup first
if [ -e "${BUILD_ROOT}/Gds/generated/${1}" ]
then
    rm -r "${BUILD_ROOT}/Gds/generated/${1}"
fi
mkdir -p "${BUILD_ROOT}/Gds/generated"
cp -r "${BUILD_ROOT}/Gse/generated/${1}" "${BUILD_ROOT}/Gds/generated/${1}"
find "${BUILD_ROOT}/Gds/generated/${1}" -name "*.pyc" -exec rm {} \;
find "${BUILD_ROOT}/Gds/generated/${1}" -name "*.py" | xargs sed -i 's/fprime\.gse\./fprime\.gds\./g'
