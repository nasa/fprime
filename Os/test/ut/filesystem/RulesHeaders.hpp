// ======================================================================
// \title Os/test/ut/filesystem/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include "Os/FileSystem.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace Os {
namespace Test {
namespace FileSystem {

struct Tester {
    //! State representation of a FileSystem.
    //!
    // enum FileSystemState {
    //     UNINITIALIZED,  //!< FileSystem is uninitialized
    //     LOCKED,   //!< FileSystem is locked
    //     UNLOCKED  //!< FileSystem is unlocked
    // };

    //! Assert in FileSystem.cpp for searching death text
    static constexpr const char* ASSERT_IN_FILESYSTEM_CPP = "Assert: \".*/Os/.*/FileSystem\\.cpp:[0-9]+\"";

    // Constructors that ensures the filesystem is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    //! FileSystem under test
    // Os::FileSystem m_filesystem = Os::FileSystem::getSingleton();

    //! Check if the back-end tester is fully functional (i.e. not a stub)
    //! \return true if functional, false otherwise
    //!
    // virtual bool functional() const = 0;

    //! FileSystem state, for testing purposes
    // FileSystemState m_state = UNINITIALIZED;

// Do NOT alter, adds rules to Tester as inner classes
#include "FileSystemRules.hpp"
};

}  // namespace FileSystem
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
