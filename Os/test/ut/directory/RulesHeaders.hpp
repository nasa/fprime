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

#include <algorithm> // for std::find


namespace Os {
namespace Test {
namespace Directory {

struct Tester {
    //! State representation of a Directory.
    //!
    enum DirectoryState {
        UNINITIALIZED,  //!< Directory is uninitialized
        OPEN,   //!< Directory is open
        CLOSED  //!< Directory is closed
    };

    // Constructors that ensures the directory is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    //! Directory under test
    Os::Directory m_directory;

    //! Tracks the directory state, for testing purposes
    DirectoryState m_state = UNINITIALIZED;
    //! Tracks the currently opened path, for testing purposes
    std::string m_path;
    //! Tracks the list of filenames created for the tested directory, for testing purposes
    std::vector<std::string> m_filenames;
    //! Tracks the seek position of directory, for testing purposes
    FwIndexType m_seek_position = 0;


    //! \brief Check if filename is in the list of test m_filenames created for the tested directory
    bool is_valid_filename(const std::string& filename) const {
        return std::find(m_filenames.cbegin(), m_filenames.cend(), filename) != m_filenames.cend();
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "DirectoryRules.hpp"
};

}  // namespace Directory
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
