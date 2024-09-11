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
#include "STest/Pick/Pick.hpp"

namespace Os {
namespace Test {
namespace FileSystem {

struct FileTracker {
    std::string path;
    std::string contents;
    FileTracker(std::string path, std::string contents) : path(path), contents(contents) {}
    void write_on_disk() {
        Os::File file;
        file.open(path.c_str(), Os::File::OPEN_CREATE);
        FwSignedSizeType bytesRead = contents.size();
        file.write(reinterpret_cast<const U8*>(contents.c_str()), bytesRead);
        file.close();
    }
}; //!< Representation of a file for tracking state of the filesystem during testing
struct DirectoryTracker {
    std::string path;
    DirectoryTracker(std::string path) : path(path) {};
    void write_on_disk() {
        Os::Directory dir;
        dir.open(path.c_str(), Os::Directory::OpenMode::CREATE);
        dir.close();
    }
}; //!< Representation of a directory for tracking state of the filesystem during testing


struct Tester {

    // ! Assert in FileSystem.cpp for searching death text
    // TODO: should add death asserts to test cases ??
    // static constexpr const char* ASSERT_IN_FILESYSTEM_CPP = "Assert: \".*/Os/.*/FileSystem\\.cpp:[0-9]+\"";

    // Constructors that ensures the filesystem is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    FwIndexType m_counter; //!< Counter for generating unique file/directory names


    //! State representation of a section of FileSystem (i.e. a test directory)
    //! This only tracks relative paths, and not hierarchy. This means that all files
    //! and directories that are nested within the root of the test directory are all
    //! tracked in the same vector. This is for simplicity, because hierarchy is not
    //! needed for the tests.
    std::vector<DirectoryTracker> m_test_dirs;
    std::vector<FileTracker> m_test_files;


    // ---------------------------------------------------------------
    // Functions to alter/track the state of the Tester
    // ---------------------------------------------------------------
    void touch_file(std::string path) {
        this->m_test_files.push_back(FileTracker(path, ""));
    }
    void create_directory(std::string path) {
        this->m_test_dirs.push_back(DirectoryTracker(path));
    }
    void move_file(std::string source, std::string dest) {
        for (FileTracker& file : this->m_test_files) {
            if (file.path == source) {
                file.path = dest;
                return;
            }
        }
    }
    void copy_file(std::string source, std::string dest) {
        for (FileTracker& file : this->m_test_files) {
            if (file.path == source) {
                this->m_test_files.push_back(FileTracker(dest, file.contents));
                return;
            }
        }
    }
    void append_file(std::string source, std::string dest, bool createMissingDest) {
        for (FileTracker& source_file : this->m_test_files) {
            if (source_file.path == source) {
                for (FileTracker& dest_file : this->m_test_files) {
                    if (dest_file.path == dest) {
                        dest_file.contents += source_file.contents;
                        return;
                    }
                }
                if (createMissingDest) {
                    this->m_test_files.push_back(FileTracker(dest, source_file.contents));
                    return;
                }
            }
        }
        std::cout << "Failed to append file: source=" << source << ", dest=" << dest << std::endl;
    }
    void remove_file(std::string path) {
        for (auto it = this->m_test_files.begin(); it != this->m_test_files.end(); ++it) {
            if (it->path == path) {
                this->m_test_files.erase(it);
                return;
            }
        }
    }

    // ----------------------------------------------------------------
    // Helper functions for performing random operations during testing
    // ----------------------------------------------------------------
    std::string get_new_filename() {
        return "test_file_" + std::to_string(m_counter++);
    }
    std::string get_new_dirname() {
        return "test_dir_" + std::to_string(m_counter++);
    }
    FileTracker get_random_file() {
        return this->m_test_files[STest::Pick::lowerUpper(0, this->m_test_files.size() - 1)];
    }
    DirectoryTracker get_random_directory() {
        return this->m_test_dirs[STest::Pick::lowerUpper(0, this->m_test_dirs.size() - 1)];
    }
    std::string get_random_new_filepath() {
        return get_random_directory().path + "/" + get_new_filename();
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "FileSystemRules.hpp"
};

}  // namespace FileSystem
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
