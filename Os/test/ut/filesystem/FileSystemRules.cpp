// ======================================================================
// \title Os/test/ut/filesystem/FileSystemRules.cpp
// \brief rule implementations for common testing of filesystem
// ======================================================================

#include "RulesHeaders.hpp"
#include "FileSystemRules.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockFileSystem -> Lock a filesystem successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::FileSystem::Tester::LockFileSystem::LockFileSystem() :
    STest::Rule<Os::Test::FileSystem::Tester>("LockFileSystem") {}

bool Os::Test::FileSystem::Tester::LockFileSystem::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_state == Os::Test::FileSystem::Tester::FileSystemState::UNLOCKED;
}

void Os::Test::FileSystem::Tester::LockFileSystem::action(Os::Test::FileSystem::Tester &state) {
    state.m_state = Os::Test::FileSystem::Tester::FileSystemState::LOCKED;
}
