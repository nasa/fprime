#!/bin/sh

# ----------------------------------------------------------------------
# tester.do
# ----------------------------------------------------------------------

. ./defs.sh

redo-ifchange ../../tester
cp ../../TesterBase.hpp .
cp ../../GTestBase.hpp .
sed 's/Impl//g' < ../../TesterBase.cpp > ./TesterBase.cpp
cp ../../GTestBase.cpp .
touch $3
