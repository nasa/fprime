#!/bin/bash

# start socket server and GUI
python $BUILD_ROOT/Gse/bin/ThreadedTCPServer.py -p 50000 &
python $BUILD_ROOT/Gse/bin/gse.py -d $BUILD_ROOT/Gse/generated/Ref -p 50000 -c &
sleep 5

# kill GUI
PID=`ps -ef | grep gse.py | head -n 1 | cut -f 2 -d ' '`
kill $PID

sleep 5
# start Ref app
$BUILD_ROOT/Ref/darwin-darwin-x86-debug-llvm-5.1-bin/Ref -p 50000 &
sleep 5

# kill Ref app
PID=`ps -ef | grep Ref | head -n 1 | cut -f 2 -d ' '`
kill -s INT $PID

sleep 5
# bring down socket server
PID=`ps -ef | grep python | head -n 1 | cut -f 2 -d ' '`
kill -s INT $PID
