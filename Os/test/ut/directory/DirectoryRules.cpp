// ======================================================================
// \title Os/test/ut/directory/DirectoryRules.cpp
// \brief rule implementations for common testing of directory
// ======================================================================

#include "RulesHeaders.hpp"
#include "DirectoryRules.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockDirectory -> Lock a directory successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::Directory::Tester::LockDirectory::LockDirectory() :
    STest::Rule<Os::Test::Directory::Tester>("LockDirectory") {}

bool Os::Test::Directory::Tester::LockDirectory::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::UNLOCKED;
}

void Os::Test::Directory::Tester::LockDirectory::action(Os::Test::Directory::Tester &state) {
    state.m_state = Os::Test::Directory::Tester::DirectoryState::LOCKED;
}
