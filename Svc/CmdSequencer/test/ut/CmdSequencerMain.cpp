// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include <Os/FileSystem.hpp>
#include "Svc/CmdSequencer/test/ut/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/Health.hpp"
#include "Svc/CmdSequencer/test/ut/Immediate.hpp"
#include "Svc/CmdSequencer/test/ut/ImmediateEOS.hpp"
#include "Svc/CmdSequencer/test/ut/InvalidFiles.hpp"
#include "Svc/CmdSequencer/test/ut/NoFiles.hpp"
#include "Svc/CmdSequencer/test/ut/Relative.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/SequenceFiles.hpp"
#include "CmdSequencerTester.hpp"
#include "Svc/CmdSequencer/test/ut/Mixed.hpp"
#include "Svc/CmdSequencer/test/ut/UnitTest.hpp"
#include "Svc/CmdSequencer/test/ut/JoinWait.hpp"

TEST(AMPCS, MissingCRC) {
  Svc::AMPCS::CmdSequencerTester tester;
  tester.MissingCRC();
}

TEST(AMPCS, MissingFile) {
  Svc::AMPCS::CmdSequencerTester tester;
  tester.MissingFile();
}

TEST(Health, Ping) {
  TEST_CASE(103.1.9,"Nominal ping test");
  Svc::Health::CmdSequencerTester tester;
  tester.Ping();
}

TEST(Immediate, AutoByCommand) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.AutoByCommand();
}

TEST(Immediate, AutoByCommandAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByCommand();
}

TEST(Immediate, AutoByPort) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.AutoByPort();
}

TEST(Immediate, AutoByPortAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByPort();
}

TEST(Immediate, Cancel) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.Cancel();
}

TEST(Immediate, CancelAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Cancel();
}

TEST(Immediate, FailedCommands) {
  TEST_CASE(103.2.6,"Off-Nominal failed command");
  Svc::Immediate::CmdSequencerTester tester;
  tester.FailedCommands();
}

TEST(Immediate, FailedCommandsAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.FailedCommands();
}

TEST(Immediate, FileErrors) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.FileErrors();
}

TEST(Immediate, FileErrorsAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.FileErrors();
}

TEST(Immediate, InvalidManualCommands) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.InvalidManualCommands();
}

TEST(Immediate, InvalidManualCommandsAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.InvalidManualCommands();
}

TEST(Immediate, LoadOnInit) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.LoadOnInit();
}

TEST(Immediate, LoadRunRun) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.LoadRunRun();
}

TEST(Immediate, LoadOnInitAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.LoadOnInit();
}

TEST(Immediate, Manual) {
  TEST_CASE(103.1.8,"Nominal Manual Sequence Stepping - No end of sequence marker");
  Svc::Immediate::CmdSequencerTester tester;
  tester.Manual();
}

TEST(Immediate, ManualAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Manual();
}

TEST(Immediate, NeverLoaded) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.NeverLoaded();
}

TEST(Immediate, NewSequence) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.NewSequence();
}

TEST(Immediate, NewSequenceAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.NewSequence();
}

TEST(Immediate, SequenceTimeout) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.SequenceTimeout();
}

TEST(Immediate, SequenceTimeoutAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.SequenceTimeout();
}

TEST(Immediate, UnexpectedCommandResponse) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.UnexpectedCommandResponse();
}

TEST(Immediate, UnexpectedCommandResponseAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.UnexpectedCommandResponse();
}

TEST(Immediate, Validate) {
  Svc::Immediate::CmdSequencerTester tester;
  tester.Validate();
}

TEST(Immediate, ValidateAMPCS) {
  Svc::Immediate::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Validate();
}

TEST(ImmediateEOS, AutoByCommand) {
  TEST_CASE(103.1.2,"Nominal Immediate Commands");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.AutoByCommand();
}

TEST(ImmediateEOS, AutoByPort) {
  TEST_CASE(103.1.5,"Nominal Immediate Port Sequence");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.AutoByPort();
}

TEST(ImmediateEOS, Cancel) {
  TEST_CASE(103.1.6,"Nominal Sequence Cancel");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.Cancel();
}

TEST(ImmediateEOS, FileErrors) {
  TEST_CASE(103.2.14,"File Load errors");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.FileErrors();
}

TEST(ImmediateEOS, InvalidManualCommands) {
  TEST_CASE(103.2.13,"Invalid Manual Commands");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.InvalidManualCommands();
}

TEST(ImmediateEOS, Manual) {
  TEST_CASE(103.1.7,"Nominal Manual Sequence Stepping");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.Manual();
}

TEST(ImmediateEOS, NewSequence) {
  TEST_CASE(103.2.7,"Off-Nominal invalid modes");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.NewSequence();
}

TEST(ImmediateEOS, SequenceTimeout) {
  TEST_CASE(103.2.12,"Sequence timeout");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.SequenceTimeout();
}

TEST(ImmediateEOS, UnexpectedCommandResponse) {
  TEST_CASE(103.2.11,"Unexpected completion after completed sequence");
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.UnexpectedCommandResponse();
}

TEST(ImmediateEOS, Validate) {
  Svc::ImmediateEOS::CmdSequencerTester tester;
  tester.Validate();
}

TEST(InvalidFiles, BadCRC) {
  TEST_CASE(103.2.2,"Off-Nominal Bad File CRC");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.BadCRC();
}

TEST(InvalidFiles, BadCRCAMPCS) {
  Svc::InvalidFiles::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.BadCRC();
}

TEST(InvalidFiles, BadRecordDescriptor) {
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.BadRecordDescriptor();
}

TEST(InvalidFiles, BadRecordDescriptorAMPCS) {
  Svc::InvalidFiles::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.BadRecordDescriptor();
}

TEST(InvalidFiles, BadTimeBase) {
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.BadTimeBase();
}

TEST(InvalidFiles, BadTimeContext) {
  TEST_CASE(103.2.10,"Bad time context");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.BadTimeContext();
}

TEST(InvalidFiles, DataAfterRecords) {
  TEST_CASE(103.2.15,"Extra Data after records");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.DataAfterRecords();
}

TEST(InvalidFiles, EmptyFile) {
  TEST_CASE(103.2.3,"Off-Nominal Empty Sequence File");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.EmptyFile();
}

TEST(InvalidFiles, EmptyFileAMPCS) {
  Svc::InvalidFiles::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.EmptyFile();
}

TEST(InvalidFiles, FileTooLarge) {
  TEST_CASE(103.2.5,"Off-Nominal File Too Large");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.FileTooLarge();
}

TEST(InvalidFiles, FileTooLargeAMPCS) {
  Svc::InvalidFiles::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.FileTooLarge();
}

TEST(InvalidFiles, MissingCRC) {
  TEST_CASE(103.2.4,"Off-Nominal Missing CRC");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.MissingCRC();
}

TEST(InvalidFiles, MissingFile) {
  TEST_CASE(103.2.1,"Off-Nominal Missing File");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.MissingFile();
}

TEST(InvalidFiles, SizeFieldTooLarge) {
  TEST_CASE(103.2.9,"Size field too large");
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.SizeFieldTooLarge();
}

TEST(InvalidFiles, SizeFieldTooSmall) {
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.SizeFieldTooSmall();
}

TEST(InvalidFiles, USecFieldTooShort) {
  Svc::InvalidFiles::CmdSequencerTester tester;
  tester.USecFieldTooShort();
}

TEST(Mixed, AutoByCommand) {
  TEST_CASE(103.1.4,"Nominal Timed Relative Commands");
  Svc::Mixed::CmdSequencerTester tester;
  tester.AutoByCommand();
}

TEST(Mixed, AutoByCommandAMPCS) {
  Svc::Mixed::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByCommand();
}

TEST(Mixed, Validate) {
  Svc::Mixed::CmdSequencerTester tester;
  tester.Validate();
}


TEST(Mixed, ValidateAMPCS) {
  Svc::Mixed::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Validate();
}
TEST(NoFiles, Init) {
  TEST_CASE(103.1.1,"Nominal Initialization");
  Svc::NoFiles::CmdSequencerTester tester;
  tester.Init();
}

TEST(NoFiles, InitAMPCS) {
  Svc::NoFiles::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Init();
}

TEST(NoFiles, NoSequenceActive) {
  TEST_CASE(103.2.8,"Off-Nominal no active sequence");
  Svc::NoFiles::CmdSequencerTester tester;
  tester.NoSequenceActive();
}

TEST(NoFiles, NoSequenceActiveAMPCS) {
  Svc::NoFiles::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.NoSequenceActive();
}

TEST(Relative, AutoByCommand) {
  TEST_CASE(103.1.3,"Nominal Relative Commands");
  Svc::Relative::CmdSequencerTester tester;
  tester.AutoByCommand();
}

TEST(Relative, AutoByCommandAMPCS) {
  Svc::Relative::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByCommand();
}

TEST(Relative, Validate) {
  Svc::Relative::CmdSequencerTester tester;
  tester.Validate();
}

TEST(Relative, ValidateAMPCS) {
  Svc::Relative::CmdSequencerTester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Validate();
}

TEST(JoinWait, JoinWaitNoActiveSeq) {
    Svc::JoinWait::CmdSequencerTester tester;
    tester.test_join_wait_without_active_seq();
}

TEST(JoinWait, JoinWaitWithActiveSeq) {
    Svc::JoinWait::CmdSequencerTester tester;
    tester.test_join_wait_with_active_seq();
}


int main(int argc, char **argv) {
  // Create ./bin directory for test files
  Os::FileSystem::createDirectory("./bin");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
