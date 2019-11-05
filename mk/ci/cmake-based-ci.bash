#!/bin/bash
DIRNAME="$(dirname "${BASH_SOURCE}")"
FPRIME_DIR=`pwd`
# LOG file DIR
export CI_LOG_DIR=`pwd`/ci-logs
rm -rf "${CI_LOG_DIR}"
mkdir -p "${CI_LOG_DIR}"
# Standard builds with `Ref` and `RPI`
for FPRIME_DEP in "${FPRIME_DIR}/Ref" "${FPRIME_DIR}/RPI"
do
    PREFIX="base-build"
    echo "[INFO] Running CMake Builds against ${FPRIME_DEP} with targets: 'generate' 'build' 'check-all' 'install' 'build-all'"
    ${DIRNAME}/scripts/cmake-build.bash "${FPRIME_DEP}" "generate" "build" "check-all" "install" "build-all"
    if (( $? != 0 ))
    then
        echo "[ERROR] Failed to run all cmake builds with: ${FPRIME_DEP}"
        exit 1
    fi
done
#Should have been installed
echo "[INFO] Running CMake Integration Tests against ${FPRIME_DIR}/Ref"
${DIRNAME}/scripts/cmake-int.bash "${FPRIME_DIR}/Ref"
CMAKE_INT_RET="$?"
cp -R "${FPRIME_DIR}/Ref/test/int/logs" "${CI_LOG_DIR}/int-apt-logs"
cp -R "${FPRIME_DIR}/Ref/logs" "${CI_LOG_DIR}/int-gds-logs"
if (( ${CMAKE_INT_RET} != 0 ))
then
    echo "[ERROR] Failed to run 'Ref' I&T tests"
    exit 2
fi
# Prep for standard runs
FPRIME_DIR=`pwd`
FPRIME_DEP="${FPRIME_DIR}/Ref"

let JOBS="${JOBS:-$(( ( RANDOM % 100 )  + 1 ))}"
# Generate build for all UTs
DIR=$(mktemp -d)
(
    cd "${DIR}"
    echo "[INFO] Generating basic CMake dir for running all UTs at: ${DIR}"
    cmake "${FPRIME_DEP}" -DCMAKE_BUILD_TYPE=Testing > "${CI_LOG_DIR}/cmake_all_ut_build.out.log" 2> "${CI_LOG_DIR}/cmake_all_ut_build.err.log"
)
if (( $? != 0 ))
then
   echo "[ERROR] Failed to run CMake for unit tests"
   exit 8
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
