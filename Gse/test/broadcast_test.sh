#!/bin/bash

# start GUI with Ref app
python $BUILD_ROOT/Gse/bin/gse.py -d $ISF_ROOT/Gse/generated/Ref -e $ISF_ROOT/Ref/darwin-darwin-x86-debug-llvm-5.1-bin/Ref -p 50000 &
# start GUI as a standalone
python $BUILD_ROOT/Gse/bin/gse.py -d $ISF_ROOT/Gse/generated/Ref -p 50000 -c &
