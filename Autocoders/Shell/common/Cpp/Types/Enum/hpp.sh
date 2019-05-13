#!/bin/sh

# ----------------------------------------------------------------------
# hpp.sh
# Generate an hpp file for an enumeration type
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

echo '`'$SHELL_AUTOCODER_DIR/'common/Cpp/banner '$name'EnumAc.hpp Auto-generated "hpp file for '$name'"

#ifndef '$namespace'_'$name'_ENUM_AC_HPP
#define '$namespace'_'$name'_ENUM_AC_HPP

#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Serializable.hpp"
`sh '$SHELL_AUTOCODER_DIR/'common/Cpp/Include/hpp.sh '$name'

namespace '$namespace' {

  `sh '$SHELL_AUTOCODER_DIR/'common/Cpp/Types/Enum/Body/hpp.sh '$name'

}

#endif' | awk -f $SHELL_AUTOCODER_DIR/common/Cpp/expand.awk
