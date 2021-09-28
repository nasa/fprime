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
#include "Svc/CmdSequencer/test/ut/Tester.hpp"
#include "Svc/CmdSequencer/test/ut/Mixed.hpp"
#include "Svc/CmdSequencer/test/ut/UnitTest.hpp"

TEST(AMPCS, MissingCRC) {
  Svc::AMPCS::Tester tester;
  tester.MissingCRC();
}

TEST(AMPCS, MissingFile) {
  Svc::AMPCS::Tester tester;
  tester.MissingFile();
}

TEST(Health, Ping) {
  TEST_CASE(103.1.9,"Nominal ping test");
  Svc::Health::Tester tester;
  tester.Ping();
}

TEST(Immediate, AutoByCommand) {
  Svc::Immediate::Tester tester;
  tester.AutoByCommand();
}

TEST(Immediate, AutoByCommandAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByCommand();
}

TEST(Immediate, AutoByPort) {
  Svc::Immediate::Tester tester;
  tester.AutoByPort();
}

TEST(Immediate, AutoByPortAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByPort();
}

TEST(Immediate, Cancel) {
  Svc::Immediate::Tester tester;
  tester.Cancel();
}

TEST(Immediate, CancelAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Cancel();
}

TEST(Immediate, FailedCommands) {
  TEST_CASE(103.2.6,"Off-Nominal failed command");
  Svc::Immediate::Tester tester;
  tester.FailedCommands();
}

TEST(Immediate, FailedCommandsAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.FailedCommands();
}

TEST(Immediate, FileErrors) {
  Svc::Immediate::Tester tester;
  tester.FileErrors();
}

TEST(Immediate, FileErrorsAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.FileErrors();
}

TEST(Immediate, InvalidManualCommands) {
  Svc::Immediate::Tester tester;
  tester.InvalidManualCommands();
}

TEST(Immediate, InvalidManualCommandsAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.InvalidManualCommands();
}

TEST(Immediate, LoadOnInit) {
  Svc::Immediate::Tester tester;
  tester.LoadOnInit();
}

TEST(Immediate, LoadRunRun) {
  Svc::Immediate::Tester tester;
  tester.LoadRunRun();
}

TEST(Immediate, LoadOnInitAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.LoadOnInit();
}

TEST(Immediate, Manual) {
  TEST_CASE(103.1.8,"Nominal Manual Sequence Stepping - No end of sequence marker");
  Svc::Immediate::Tester tester;
  tester.Manual();
}

TEST(Immediate, ManualAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Manual();
}

TEST(Immediate, NeverLoaded) {
  Svc::Immediate::Tester tester;
  tester.NeverLoaded();
}

TEST(Immediate, NewSequence) {
  Svc::Immediate::Tester tester;
  tester.NewSequence();
}

TEST(Immediate, NewSequenceAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.NewSequence();
}

TEST(Immediate, SequenceTimeout) {
  Svc::Immediate::Tester tester;
  tester.SequenceTimeout();
}

TEST(Immediate, SequenceTimeoutAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.SequenceTimeout();
}

TEST(Immediate, UnexpectedCommandResponse) {
  Svc::Immediate::Tester tester;
  tester.UnexpectedCommandResponse();
}

TEST(Immediate, UnexpectedCommandResponseAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.UnexpectedCommandResponse();
}

TEST(Immediate, Validate) {
  Svc::Immediate::Tester tester;
  tester.Validate();
}

TEST(Immediate, ValidateAMPCS) {
  Svc::Immediate::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Validate();
}

TEST(ImmediateEOS, AutoByCommand) {
  TEST_CASE(103.1.2,"Nominal Immediate Commands");
  Svc::ImmediateEOS::Tester tester;
  tester.AutoByCommand();
}

TEST(ImmediateEOS, AutoByPort) {
  TEST_CASE(103.1.5,"Nominal Immediate Port Sequence");
  Svc::ImmediateEOS::Tester tester;
  tester.AutoByPort();
}

TEST(ImmediateEOS, Cancel) {
  TEST_CASE(103.1.6,"Nominal Sequence Cancel");
  Svc::ImmediateEOS::Tester tester;
  tester.Cancel();
}

TEST(ImmediateEOS, FileErrors) {
  TEST_CASE(103.2.14,"File Load errors");
  Svc::ImmediateEOS::Tester tester;
  tester.FileErrors();
}

TEST(ImmediateEOS, InvalidManualCommands) {
  TEST_CASE(103.2.13,"Invalid Manual Commands");
  Svc::ImmediateEOS::Tester tester;
  tester.InvalidManualCommands();
}

TEST(ImmediateEOS, Manual) {
  TEST_CASE(103.1.7,"Nominal Manual Sequence Stepping");
  Svc::ImmediateEOS::Tester tester;
  tester.Manual();
}

TEST(ImmediateEOS, NewSequence) {
  TEST_CASE(103.2.7,"Off-Nominal invalid modes");
  Svc::ImmediateEOS::Tester tester;
  tester.NewSequence();
}

TEST(ImmediateEOS, SequenceTimeout) {
  TEST_CASE(103.2.12,"Sequence timeout");
  Svc::ImmediateEOS::Tester tester;
  tester.SequenceTimeout();
}

TEST(ImmediateEOS, UnexpectedCommandResponse) {
  TEST_CASE(103.2.11,"Unexpected completion after completed sequence");
  Svc::ImmediateEOS::Tester tester;
  tester.UnexpectedCommandResponse();
}

TEST(ImmediateEOS, Validate) {
  Svc::ImmediateEOS::Tester tester;
  tester.Validate();
}

TEST(InvalidFiles, BadCRC) {
  TEST_CASE(103.2.2,"Off-Nominal Bad File CRC");
  Svc::InvalidFiles::Tester tester;
  tester.BadCRC();
}

TEST(InvalidFiles, BadCRCAMPCS) {
  Svc::InvalidFiles::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.BadCRC();
}

TEST(InvalidFiles, BadRecordDescriptor) {
  Svc::InvalidFiles::Tester tester;
  tester.BadRecordDescriptor();
}

TEST(InvalidFiles, BadRecordDescriptorAMPCS) {
  Svc::InvalidFiles::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.BadRecordDescriptor();
}

TEST(InvalidFiles, BadTimeBase) {
  Svc::InvalidFiles::Tester tester;
  tester.BadTimeBase();
}

TEST(InvalidFiles, BadTimeContext) {
  TEST_CASE(103.2.10,"Bad time context");
  Svc::InvalidFiles::Tester tester;
  tester.BadTimeContext();
}

TEST(InvalidFiles, DataAfterRecords) {
  TEST_CASE(103.2.15,"Extra Data after records");
  Svc::InvalidFiles::Tester tester;
  tester.DataAfterRecords();
}

TEST(InvalidFiles, EmptyFile) {
  TEST_CASE(103.2.3,"Off-Nominal Empty Sequence File");
  Svc::InvalidFiles::Tester tester;
  tester.EmptyFile();
}

TEST(InvalidFiles, EmptyFileAMPCS) {
  Svc::InvalidFiles::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.EmptyFile();
}

TEST(InvalidFiles, FileTooLarge) {
  TEST_CASE(103.2.5,"Off-Nominal File Too Large");
  Svc::InvalidFiles::Tester tester;
  tester.FileTooLarge();
}

TEST(InvalidFiles, FileTooLargeAMPCS) {
  Svc::InvalidFiles::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.FileTooLarge();
}

TEST(InvalidFiles, MissingCRC) {
  TEST_CASE(103.2.4,"Off-Nominal Missing CRC");
  Svc::InvalidFiles::Tester tester;
  tester.MissingCRC();
}

TEST(InvalidFiles, MissingFile) {
  TEST_CASE(103.2.1,"Off-Nominal Missing File");
  Svc::InvalidFiles::Tester tester;
  tester.MissingFile();
}

TEST(InvalidFiles, SizeFieldTooLarge) {
  TEST_CASE(103.2.9,"Size field too large");
  Svc::InvalidFiles::Tester tester;
  tester.SizeFieldTooLarge();
}

TEST(InvalidFiles, SizeFieldTooSmall) {
  Svc::InvalidFiles::Tester tester;
  tester.SizeFieldTooSmall();
}

TEST(InvalidFiles, USecFieldTooShort) {
  Svc::InvalidFiles::Tester tester;
  tester.USecFieldTooShort();
}

TEST(Mixed, AutoByCommand) {
  TEST_CASE(103.1.4,"Nominal Timed Relative Commands");
  Svc::Mixed::Tester tester;
  tester.AutoByCommand();
}

TEST(Mixed, AutoByCommandAMPCS) {
  Svc::Mixed::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByCommand();
}

TEST(Mixed, Validate) {
  Svc::Mixed::Tester tester;
  tester.Validate();
}


TEST(Mixed, ValidateAMPCS) {
  Svc::Mixed::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Validate();
}
TEST(NoFiles, Init) {
  TEST_CASE(103.1.1,"Nominal Initialization");
  Svc::NoFiles::Tester tester;
  tester.Init();
}

TEST(NoFiles, InitAMPCS) {
  Svc::NoFiles::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Init();
}

TEST(NoFiles, NoSequenceActive) {
  TEST_CASE(103.2.8,"Off-Nominal no active sequence");
  Svc::NoFiles::Tester tester;
  tester.NoSequenceActive();
}

TEST(NoFiles, NoSequenceActiveAMPCS) {
  Svc::NoFiles::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.NoSequenceActive();
}

TEST(Relative, AutoByCommand) {
  TEST_CASE(103.1.3,"Nominal Relative Commands");
  Svc::Relative::Tester tester;
  tester.AutoByCommand();
}

TEST(Relative, AutoByCommandAMPCS) {
  Svc::Relative::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.AutoByCommand();
}

TEST(Relative, Validate) {
  Svc::Relative::Tester tester;
  tester.Validate();
}

TEST(Relative, ValidateAMPCS) {
  Svc::Relative::Tester tester(Svc::SequenceFiles::File::Format::AMPCS);
  tester.Validate();
}

int main(int argc, char **argv) {
  // Create ./bin directory for test files
  Os::FileSystem::createDirectory("./bin");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
