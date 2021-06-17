#!/bin/sh -e

export FPRIME_ROOT=..
export SUBDIRS="
ActiveRateGroup
ActiveTextLogger
ComSplitter
Cycle
Deframer
Fatal
FatalHandler
FileDownlink
Framer
LinuxTimer
Ping
PolyDb
PolyIf
Sched
Seq
Time
TlmChan
WatchDog
"
. $FPRIME_ROOT/defs-root.sh
