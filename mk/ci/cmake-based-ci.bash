#!/bin/bash
DIRNAME="$(dirname "${BASH_SOURCE}")"

FPRIME_DIR=`pwd`
FPRIME_DEP="${FPRIME_DIR}/Ref"

let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"
PREFIX="base-build"
${DIRNAME}/scripts/cmake-build.bash "${FPRIME_DEP}" "" "install"
if (( $? != 0 ))
then
    echo "[ERROR] Failed to build (-j${JOBS}): ${PREFIX}"
    exit 1
fi

let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"
PREFIX="all-uts"
DIR=$(mktemp -d)
BUILD_DIR="${DIR}" ${DIRNAME}/scripts/cmake-ut.bash "${FPRIME_DIR}" "check" 
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake and process UTs (-j${JOBS}): ${PREFIX}"
    rm -r "${DIR}"
    exit 2
fi
# Read the previous build and run each target individually
(
  OWD="$(pwd)"
  let RET=0
  cd ${DIR}
  TARGETS="$(make help | sed -n 's/\.\.\. \(.*_exe\)/\1/p')"
  rm -r "${DIR}"
  cd "${OWD}"
  for target in ${TARGETS}
  do
    let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"
    PREFIX="ut-${target}"
    ${DIRNAME}/scripts/cmake-ut.bash "${FPRIME_DIR}" "${target}"
    if (( $? != 0 ))
    then
        let RET=5
        echo "[ERROR] Failed to run individual UT for (-j${JOBS}): ${target}"
    fi
  done
  exit ${RET}
) || exit $?

${DIRNAME}/scripts/cmake-cmaketest.bash
if (( $? != 0 ))
then
    echo "[ERROR] Failed to run CMake unit tests"
    exit 2
fi
exit 0
