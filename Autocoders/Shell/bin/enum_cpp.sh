#!/bin/sh

# ----------------------------------------------------------------------
# enum_hpp.sh
# Generate source file for an enumeration type
# ----------------------------------------------------------------------
# Arguments
# ----------------------------------------------------------------------
# $1: input NameEnumAi.txt
# ----------------------------------------------------------------------
# Input file
# ----------------------------------------------------------------------
# NameEnumAi.txt.txt: An ASCII table in which each row is an enumeration
# consisting of a name, a value, and a comment.
# The value and comment are optional. The name of the input file will
# be the name of the enumeration. The first row specifies the namespace.
# ----------------------------------------------------------------------

# Source shell functions
. $SHELL_AUTOCODER_DIR/defs.sh

enum_cpp_gen $@
