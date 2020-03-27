# ----------------------------------------------------------------------
# mod.mk 
# ----------------------------------------------------------------------

TEST_SRC= \
  AMPCS.cpp \
  CommandBuffers.cpp \
  Handcode/GTestBase.cpp \
  Health.cpp \
  ImmediateBase.cpp \
  Immediate.cpp \
  ImmediateEOS.cpp \
  Interceptors.cpp \
  InvalidFiles.cpp \
  Main.cpp \
  Mixed.cpp \
  MixedRelativeBase.cpp \
  NoFiles.cpp \
  Relative.cpp \
  SequenceFiles/AMPCS/CRCs.cpp \
  SequenceFiles/AMPCS/Headers.cpp \
  SequenceFiles/AMPCS/Records.cpp \
  SequenceFiles/BadCRCFile.cpp \
  SequenceFiles/BadDescriptorFile.cpp \
  SequenceFiles/BadTimeBaseFile.cpp \
  SequenceFiles/BadTimeContextFile.cpp \
  SequenceFiles/Buffers.cpp \
  SequenceFiles/DataAfterRecordsFile.cpp \
  SequenceFiles/EmptyFile.cpp \
  SequenceFiles/File.cpp \
  SequenceFiles/FPrime/CRCs.cpp \
  SequenceFiles/FPrime/Headers.cpp \
  SequenceFiles/FPrime/Records.cpp \
  SequenceFiles/ImmediateEOSFile.cpp \
  SequenceFiles/ImmediateFile.cpp \
  SequenceFiles/MissingCRCFile.cpp \
  SequenceFiles/MissingFile.cpp \
  SequenceFiles/MixedFile.cpp \
  SequenceFiles/RelativeFile.cpp \
  SequenceFiles/SizeFieldTooLargeFile.cpp \
  SequenceFiles/SizeFieldTooSmallFile.cpp \
  SequenceFiles/TooLargeFile.cpp \
  SequenceFiles/USecFieldTooShortFile.cpp \
  Handcode/TesterBase.cpp \
  Tester.cpp

TEST_MODS= \
  Svc/CmdSequencer \
  Svc/Sched \
  Svc/Ping \
  Svc/Seq \
  Fw/Cmd \
  Fw/Comp \
  Fw/Port \
  Fw/Prm \
  Fw/Time \
  Fw/Tlm \
  Fw/Types \
  Fw/Log \
  Fw/Obj \
  Fw/Com \
  Os/Stubs \
  Os \
  Utils/Hash \
  gtest
  



COMPARGS = -I$(CURDIR)/test/ut/Handcode
