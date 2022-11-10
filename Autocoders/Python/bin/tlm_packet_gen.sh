#!/usr/bin/bash

script_dir=$(dirname $0)

# $0 = this script
# $1 = packet file
# $2 = deployment build cache (ex. Ref/build-fprime-automatic-native)

export PYTHONPATH=$script_dir/../src
export BUILD_ROOT=$script_dir/../../../:$2:$2/F-Prime
echo "BUILD_ROOT=$BUILD_ROOT"

# get python from the path
python3 $script_dir/tlm_packet_gen.py $1