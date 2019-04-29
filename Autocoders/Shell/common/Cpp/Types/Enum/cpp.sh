#!/bin/sh

# ----------------------------------------------------------------------
# cpp.sh
# ----------------------------------------------------------------------



namespace=$1
name=$2

sh $SHELL_AUTOCODER_DIR/common/Cpp/banner $name Auto-generated 'cpp file for '$name
echo '
#include <string.h>

#include "'$name'EnumAc.hpp"

namespace '$namespace' {

  `sh '$SHELL_AUTOCODER_DIR/'common/Cpp/Types/Enum/Body/cpp.sh '$name'

}' | awk -f $SHELL_AUTOCODER_DIR/common/Cpp/expand.awk
