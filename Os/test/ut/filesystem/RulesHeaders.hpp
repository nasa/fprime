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
    FileTracker(std::string path, std::string contents) : path(path), contents(contents) {};
}; //!< Representation of a file for tracking state of the filesystem during testing

struct DirectoryTracker {
    std::string path;
    DirectoryTracker(std::string path) : path(path) {};
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
    void remove_file(std::string path) {
        for (auto it = this->m_test_files.begin(); it != this->m_test_files.end(); ++it) {
            if (it->path == path) {
                this->m_test_files.erase(it);
                return;
            }
        }
    }
    // ------------- By reference instead ----------------
    void move_file2(FileTracker& source, std::string dest_path) {
        source.path = dest_path;
    }
    void copy_file2(FileTracker& source, std::string dest_path) {
        FileTracker new_file(dest_path, source.contents);
        this->m_test_files.push_back(new_file);
    }
    void append_file2(FileTracker& source_file, FileTracker& dest_file, bool createMissingDest) {
        if (createMissingDest) {
            touch_file(dest_file.path);
        }
        dest_file.contents += source_file.contents;

    }

    // ----------------------------------------------------------------
    // Helper functions for testing
    // ----------------------------------------------------------------
    std::string get_new_filename() {
        return "test_file_" + std::to_string(m_counter++);
    }
    std::string get_new_dirname() {
        return "test_dir_" + std::to_string(m_counter++);
    }
    FileTracker& get_random_file() {
        return this->m_test_files[STest::Pick::lowerUpper(0, this->m_test_files.size() - 1)];
    }
    DirectoryTracker& get_random_directory() {
        return this->m_test_dirs[STest::Pick::lowerUpper(0, this->m_test_dirs.size() - 1)];
    }
    std::string get_random_new_filepath() {
        return get_random_directory().path + "/" + get_new_filename();
    }

    bool validate_contents_on_disk(FileTracker& file) {
        Os::File disk_file;
        disk_file.open(file.path.c_str(), Os::File::OPEN_READ);
        FwSignedSizeType size;
        disk_file.size(size);
        if (size == 0) {
            disk_file.close();
            return file.contents.empty();
        }
        U8 buffer[size];
        disk_file.read(buffer, size);
        disk_file.close();
        std::string disk_contents(reinterpret_cast<char*>(buffer), size);
        return disk_contents == file.contents;
    }

    // ----------------------------------------------------------------
    // Helpers to write and remove test state from disk
    // ----------------------------------------------------------------
    void intialize_test_state_on_disk() {
        Os::File file;
        Os::Directory dir;
        // Create and write directories
        for (DirectoryTracker& dir_track : this->m_test_dirs) {
            dir.open(dir_track.path.c_str(), Os::Directory::OpenMode::CREATE);
            dir.close();
            this->m_counter++;
        }
        // Create and write files
        for (FileTracker& file_track : this->m_test_files) {
            file.open(file_track.path.c_str(), Os::File::OPEN_CREATE);
            FwSignedSizeType bytesRead = file_track.contents.size();
            file.write(reinterpret_cast<const U8*>(file_track.contents.c_str()), bytesRead);
            file.close();
            this->m_counter++;
        }
    }

    void purge_test_state_from_disk() {
        // TODO: delete all files and directories without relying on Os::FileSystem?
        // Is it strictly needed though? Since we'll rely on user-written functions anyways
        for (auto filename : this->m_test_files) {
            Os::FileSystem::removeFile(filename.path.c_str());
        }
        for (auto it = this->m_test_dirs.rbegin(); it != this->m_test_dirs.rend(); ++it) {
            Os::FileSystem::removeDirectory(it->path.c_str());
        }
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "FileSystemRules.hpp"
};

}  // namespace FileSystem
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
