#!/bin/sh -e

export FPRIME_ROOT=..
export SUBDIRS="
ActiveLogger
ActiveRateGroup
ActiveTextLogger
AssertFatalAdapter
BufferLogger
BufferManager
CmdDispatcher
CmdSequencer
ComLogger
ComSplitter
Cycle
Deframer
Fatal
FatalHandler
FileDownlink
FileManager
FileUplink
Framer
GenericHub
GenericRepeater
GroundInterface
Health
LinuxTimer
PassiveConsoleTextLogger
Ping
PolyDb
PolyIf
PrmDb
RateGroupDriver
Sched
Seq
StaticMemory
Time
TlmChan
WatchDog
"
. $FPRIME_ROOT/defs-root.sh
