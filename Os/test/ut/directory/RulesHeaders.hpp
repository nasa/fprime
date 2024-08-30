// ======================================================================
// \title Os/test/ut/directory/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include "Os/Directory.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace Os {
namespace Test {
namespace Directory {

struct Tester {
    // static constexpr char* TEST_DIR_NAME = "DirectoryTester";
    //! State representation of a Directory.
    //!
    enum DirectoryState {
        UNINITIALIZED,  //!< Directory is uninitialized
        OPEN,   //!< Directory is open
        CLOSED  //!< Directory is closed
    };

    //! Maximum number of files per test directory
    //! Intentionally low to have a decent probability of having an empty directory
    static const FwIndexType MAX_FILES_PER_DIRECTORY = 4;

    //! Assert in Directory.cpp for searching death text
    static constexpr const char* ASSERT_IN_DIRECTORY_CPP = "Assert: \".*/Os/.*/Directory\\.cpp:[0-9]+\"";

    // Constructors that ensures the directory is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    //! Directory under test
    Os::Directory m_directory;
    DirectoryState m_state = UNINITIALIZED;

    //! Currently opened path
    std::string m_path;
    //! List of filenames created for the tested directory
    std::vector<std::string> m_filenames;
    //! Current seek position of directory
    FwIndexType m_seek_position = 0;

    //! Check if the back-end tester is fully functional (i.e. not a stub)
    //! \return true if functional, false otherwise
    //!
    // virtual bool functional() const = 0;

    //! Directory state, for testing purposes


    //! \brief Check if filename is in the list of test m_filenames created for the tested directory
    bool is_valid_filename(const std::string& filename) {
        return std::find(m_filenames.begin(), m_filenames.end(), filename) != m_filenames.end();
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "DirectoryRules.hpp"
};

}  // namespace Directory
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
