#!/bin/csh

if !($?BUILD_ROOT) then
    set curdir = "${PWD}"
    setenv BUILD_ROOT `dirname $0`/../../..
    cd $BUILD_ROOT
    setenv BUILD_ROOT ${PWD}
    cd ${curdir}
endif

echo "BUILD_ROOT is: ${BUILD_ROOT}"

# Borrow some variables from build
setenv PYTHON_BASE `make -f ${BUILD_ROOT}/mk/makefiles/build_vars.mk print_python_base`
echo "PYTHON_BASE: ${PYTHON_BASE}"

setenv LD_LIBRARY_PATH ${PYTHON_BASE}/lib
setenv PYTHONPATH ${BUILD_ROOT}/Autocoders/Python/src/fprime_ac
echo "PYTHONPATH: ${PYTHONPATH}"
${PYTHON_BASE}/bin/python ${BUILD_ROOT}/Autocoders/Python/bin/cosmosgen.py  $*

echo "Build vars set, running cosmosgen.py"
python "${BUILD_ROOT}/Autocoders/Python/bin/JSONDictionaryGen.py" "$@"
