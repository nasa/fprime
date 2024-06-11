// ======================================================================
// \title Os/test/ut/file/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include <cstdio>
#include <map>
#include "Os/File.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "Os/test/ut/file/SyntheticFileSystem.hpp"

namespace Os {
namespace Test {
namespace File {

struct Tester {
    //! State data for an open OS file.
    //!
    struct FileState {
        FwSignedSizeType size = -1;
        FwSignedSizeType position = -1;
    };

    //! Assert in File.cpp for searching death text
    static constexpr const char* ASSERT_IN_FILE_CPP = "Assert: \".*/Os/File\\.cpp:[0-9]+\"";

    // Constructors that ensures the file is always valid
    Tester();

    // Destructor must be virtual
    virtual ~Tester() = default;

    //! Check if the test file exists.
    //! \return true if it exists, false otherwise.
    //!
    virtual bool exists(const std::string& filename) const = 0;

    //! Check if the back-end tester is fully functional
    //! \return true if functional, false otherwise
    //!
    virtual bool functional() const = 0;

    //! Get a filename, randomly if random is true, otherwise use a basic filename.
    //! \param random: true if filename should be random, false if predictable
    //! \return: filename to use for testing as a shared pointer
    //!
    virtual std::shared_ptr<const std::string> get_filename(bool random) const = 0;

    //! Performs the "open" action on the shadow state.
    //!
    Os::File::Status shadow_open(const std::string &path, Os::File::Mode newly_opened_mode = Os::File::Mode::OPEN_NO_MODE,
                                 bool overwrite = false);

    //! Perform the "close" action on the shadow state.
    //!
    void shadow_close();

    //! Perform the "read" action on the shadow state returning the read data.
    //! \return data read
    //!
    std::vector<U8> shadow_read(FwSignedSizeType size);

    //! Perform the "write" action on the shadow state given the data.
    //!
    void shadow_write(const std::vector<U8>& data);

    //! Perform the "seek" action on the shadow state given.
    //!
    void shadow_seek(const FwSignedSizeType offset, const bool absolute);

    //! Perform the "preallocate" action on the shadow state.
    //!
    void shadow_preallocate(const FwSignedSizeType offset, const FwSignedSizeType length);

    //! Perform the "flush" action on the shadow state.
    //!
    void shadow_flush();

    //! Perform the "full crc" action on the shadow state.
    //! \param crc: output for CRC value
    //!
    void shadow_crc(U32& crc);

    //! Perform the "incremental crc" action on the shadow state.
    //! \param crc: output for CRC value
    //!
    void shadow_partial_crc(FwSignedSizeType& size);

    //! Perform the "crc finalize" action on the shadow state.
    //! \param crc: output for CRC value
    //!
    void shadow_finalize(U32& crc);

    //! Detect current state of the file. Note: for files that are known, but unopened this will open the file as read
    //! detect the state, and then close it again.
    //! \return file state for the test
    //!
    FileState current_file_state();

    //! Assert file and shadow state are in-sync
    //!
    void assert_file_consistent();

    //! Assert that the supplied status is appropriate for the current mode.
    //! \param status: status to check
    //!
    void assert_valid_mode_status(Os::File::Status& status) const;

    //! Assert that the file is opened
    //!
    void assert_file_opened(const std::string& path="", Os::File::Mode newly_opened_mode = Os::File::Mode::OPEN_NO_MODE, bool overwrite = false);

    //! Assert that the file is closed
    //!
    void assert_file_closed();

    //! Assert a file read
    //!
    void assert_file_read(const std::vector<U8>& state_data, const unsigned char* read_data, FwSignedSizeType size_read);

    //! Assert a file write
    //!
    void assert_file_write(const std::vector<U8>& write_data, FwSignedSizeType size_written);

    //! Assert a file seek
    //!
    void assert_file_seek(const FwSignedSizeType original_position, const FwSignedSizeType seek_desired, const bool absolute);

    //! File under test
    Os::File m_file;

    //! Shadow file state: file system
    Os::Test::SyntheticFile m_shadow;

    //! Currently opened path
    std::string m_current_path;

    //! Independent tracking of mode
    Os::File::Mode m_mode = Os::File::Mode::OPEN_NO_MODE;

    U32 m_independent_crc = Os::File::INITIAL_CRC;

// Do NOT alter, adds rules to Tester as inner classes
#include "FileRules.hpp"
};

//! Get the tester implementation for the given backend.
//! \return pointer to tester subclass implementation
//!
std::unique_ptr<Os::Test::File::Tester> get_tester_implementation();
}  // namespace File
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
