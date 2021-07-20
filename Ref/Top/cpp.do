#!/bin/sh

. ./defs.sh

export FPP_TO_CPP_OPTS='-g RefTop'
cpp_do "$@"
