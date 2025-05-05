// ======================================================================
// \title Os/Posix/test/ut/PosixFileTests.cpp
// \brief tests for posix implementation for Os::File
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Console.hpp"
#include "Os/Posix/Console.hpp"


TEST(Nominal, SwitchStream) {
    Os::Posix::Console::PosixConsole posix_console;
    ASSERT_EQ(reinterpret_cast<Os::Posix::Console::PosixConsoleHandle*>(posix_console.getHandle())->m_file_descriptor,
              stdout);
    posix_console.setOutputStream(Os::Posix::Console::PosixConsole::Stream::STANDARD_ERROR);
    ASSERT_EQ(reinterpret_cast<Os::Posix::Console::PosixConsoleHandle*>(posix_console.getHandle())->m_file_descriptor,
              stderr);
    posix_console.setOutputStream(Os::Posix::Console::PosixConsole::Stream::STANDARD_OUT);
    ASSERT_EQ(reinterpret_cast<Os::Posix::Console::PosixConsoleHandle*>(posix_console.getHandle())->m_file_descriptor,
              stdout);
}
TEST(OffNominal, SwitchStream) {
    Os::Posix::Console::PosixConsole posix_console;
    ASSERT_DEATH(posix_console.setOutputStream(static_cast<Os::Posix::Console::PosixConsole::Stream>(3)), "Posix/|\\Console.cpp:33");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
