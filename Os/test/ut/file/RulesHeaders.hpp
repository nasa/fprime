

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

namespace Os {
namespace Test {
namespace File {

struct Tester {
    struct FileData {
        std::vector<U8> data;
        FwSizeType pointer;
    };

    //! Assert in File.cpp for searching death text
    static constexpr const char* ASSERT_IN_FILE_CPP = "Assert: \".*/Os/File\\.cpp:[0-9]+\"";

    // Constructors that ensures the file is always valid
    Tester() : file(), mode(Os::File::Mode::OPEN_NO_MODE) {}

    // Destructor must be virtual
    virtual ~Tester() = default;

    /**
     * Check if the test file exists.
     * @return true if it exists, false otherwise.
     */
    virtual bool exists(const std::string& filename) const = 0;

    /**
     * Current offset of the open file
     * @return: offset into file of read/write/seek position
     */
    virtual FwSizeType position() const = 0;

    /**
     * Get a filename, randomly if random is true, otherwise use a basic filename.
     * @param random: true if filename should be random, false if predictable
     * @return: filename to use for testing as a shared pointer
     */
    virtual std::shared_ptr<const std::string> get_filename(bool random) const = 0;

    /**
     * Reports that a file may be opens.
     * @return: true if a file may yet be created, false otherwise
     */
    virtual bool can_create_file() const = 0;

    /**
     * Get size of given file.
     * @return: size of file
     */
    virtual FwSizeType size(const std::string& filename) const = 0;

    /**
     * Assert file and shadow state are in-sync
     */
    void assert_file_consistent();

    /**
     * Assert that the supplied status is appropriate for the current mode.
     * @param status: status to check
     */
    void assert_valid_mode_status(Os::File::Status& status) const;

    /**
     * Assert that the file is opened
     */
    void assert_file_opened(const std::string& path, Os::File::Mode newly_opened_mode = Os::File::Mode::OPEN_NO_MODE, bool overwrite = false);

    /**
     * Assert that the file is closed
     */
    void assert_file_closed();

    /**
     * Assert a file read
     */
    void assert_file_read(const unsigned char* read_data, FwSizeType size_read, FwSizeType size_desired);

    /**
     * Assert a file write
     */
    void assert_file_write(const unsigned char* write_data, FwSizeType size_written, FwSizeType size_desired);

    /**
     * Assert a file seek
     */
    void assert_file_seek(FwSizeType seek_desired, bool absolute);

    //! File under test
    Os::File file;

    //! Shadow file mode
    Os::File::Mode mode;

    //! Shadow file state: file system
    std::map<std::string, std::unique_ptr<FileData>> filesystem;

    //! Currently opened path
    std::string current_path;

// Do NOT alter, adds rules to Tester as inner classes
#include "MyRules.hpp"
};

/**
 * Get the tester implementation for the given backend.
 * @return pointer to tester subclass implementation
 */
std::unique_ptr<Os::Test::File::Tester> get_tester_implementation();
}  // namespace File
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
