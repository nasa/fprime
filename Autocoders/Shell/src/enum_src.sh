#!/bin/sh

enum_validate()
{
  export input_file=$1

  if [ "$#" -ne 1 ]; then
    echo "Usage: $0 NameEnumAi.txt"  >&2
    exit 1
  fi

  if [ ! -f "$input_file" ]; then
    echo "Error: $input_file does not exist" >&2
    exit 2
  fi

  if [ -z "$SHELL_AUTOCODER_DIR" ]; then
    echo "Error: SHELL_AUTOCODER_DIR is not set" >&2
    exit 3
  fi

  # Check input file name is well formed
  if [[ ! "$input_file" = *EnumAi.txt ]]; then
    echo "Error: $input_file is not a valid EnumAi.txt file" >&2
    exit 4
  fi

  # Extract names
  export enum_name=`basename "$input_file" EnumAi.txt`
  export namespace=$(head -n 1 $input_file)
}

enum_hpp_gen()
{
  # Validate environment
  enum_validate $@
  output_file="${enum_name}EnumAc.hpp"
  sh $SHELL_AUTOCODER_DIR/common/Cpp/Types/Enum/hpp.sh $namespace $enum_name > $output_file
}

enum_h_gen()
{
  # Validate environment
  enum_validate $@
  output_file="${enum_name}EnumAc.h"
  sh $SHELL_AUTOCODER_DIR/common/Cpp/Types/Enum/h.sh $namespace $enum_name > $output_file
}

enum_cpp_gen()
{
  # Validate environment
  enum_validate $@
  output_file="${enum_name}EnumAc.cpp"
  sh $SHELL_AUTOCODER_DIR/common/Cpp/Types/Enum/cpp.sh $namespace $enum_name > $output_file
}

enum_py_gen()
{
  # Validate environment
  enum_validate $@
  output_file="${enum_name}.py"
  sh $SHELL_AUTOCODER_DIR/common/Python/Types/enum.sh $enum_name > $output_file
}

enum_dict_gen()
{
  python_file=$1

  for deployment in $DEPLOYMENTS
  do
    # Make dictionary directory:
    dict_dir=$BUILD_ROOT/Gse/generated/$deployment/serializable/
    mkdir -p $dict_dir
    touch $dict_dir/__init__.py

    # Copy python file
    cp $python_file $dict_dir
  done
}
