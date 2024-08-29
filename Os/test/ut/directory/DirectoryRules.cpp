// ======================================================================
// \title Os/test/ut/directory/DirectoryRules.cpp
// \brief rule implementations for common testing of directory
// ======================================================================

#include "RulesHeaders.hpp"
#include "DirectoryRules.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenDirectory -> Lock a directory successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::Directory::Tester::OpenDirectory::OpenDirectory() :
    STest::Rule<Os::Test::Directory::Tester>("OpenDirectory") {}

bool Os::Test::Directory::Tester::OpenDirectory::precondition(const Os::Test::Directory::Tester &state) {
    return true;
}

void Os::Test::Directory::Tester::OpenDirectory::action(Os::Test::Directory::Tester &state) {
    state.m_directory.open(state.m_path.c_str());
}
