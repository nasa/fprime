#!/bin/sh

# ----------------------------------------------------------------------
# h.sh
# Generate an h file for an enumeration type
# ----------------------------------------------------------------------
# Arguments
# ----------------------------------------------------------------------
# $1: The enclosing namespace
# $2: The type name
# ----------------------------------------------------------------------
# Input files
# ----------------------------------------------------------------------
# Enum.txt: An ASCII table in which each row is an enumeration
# consisting of a name, a value, and a comment.
# The value and comment are optional.
# ----------------------------------------------------------------------



namespace=$1
name=$2

echo '`$SHELL_AUTOCODER_DIR/common/Cpp/banner '$name'.h Auto-generated "h file for '$name'"

#ifndef '$namespace'_'$name'_H
#define '$namespace'_'$name'_H

#include "Fw/Types/BasicTypes.hpp"
`sh $SHELL_AUTOCODER_DIR/common/Cpp/Include/h.sh '$name'

`sh $SHELL_AUTOCODER_DIR/common/Cpp/Types/Enum/Body/h.sh '$namespace' '$name'

#endif' | awk -f $SHELL_AUTOCODER_DIR/common/Cpp/expand.awk
