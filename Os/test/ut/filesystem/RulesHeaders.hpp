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

struct Tester {
    //! State representation of a FileSystem.
    //!
    // enum FileSystemState {
    //     UNINITIALIZED,  //!< FileSystem is uninitialized
    //     LOCKED,   //!< FileSystem is locked
    //     UNLOCKED  //!< FileSystem is unlocked
    // };

    // static constexpr const char* TEST_FILENAME_PREFIX = "test_file_";
    // static constexpr const char* TEST_DIRECTORY_PREFIX = "./filesystem_test_dir_";
    // static const FwIndexType MAX_FILES_PER_DIRECTORY = 4;
    // static const FwIndexType MAX_NUMBER_OF_DIRECTORY = 4;

    //! Assert in FileSystem.cpp for searching death text
    static constexpr const char* ASSERT_IN_FILESYSTEM_CPP = "Assert: \".*/Os/.*/FileSystem\\.cpp:[0-9]+\"";

    // Constructors that ensures the filesystem is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    struct MockFile {
        std::string path;
        std::string contents;
        MockFile(std::string path, std::string contents) : path(path), contents(contents) {}

    }; //!< Mock representation of a file
    struct MockDirectory {
        std::string path;
        std::vector<MockFile> files;
        std::vector<MockDirectory> directories;
        MockDirectory(std::string path) : path(path) {};
        void add_file(std::string name, std::string contents) {
            MockFile file = MockFile(path + "/" + name, contents);
            files.push_back(file);
            Tester::m_test_files.push_back(file);
            Os::File f;
            f.open(file.path.c_str(), Os::File::OPEN_CREATE);
            FwSignedSizeType bytesRead = contents.size();
            f.write(reinterpret_cast<const U8*>(contents.c_str()), bytesRead);
            f.close();
        }
        void add_directory(std::string name) {
            MockDirectory dir = MockDirectory(path + "/" + name);
            directories.push_back(dir);
            Tester::m_test_dirs.push_back(dir);
            Os::Directory d;
            d.open(dir.path.c_str(), Os::Directory::OpenMode::CREATE);
            d.close();
        }
    };


    static FwIndexType m_counter;
    // TODO: rename to s_* for static
    static MockDirectory m_testdir_root;
    static std::vector<MockDirectory> m_test_dirs;
    static std::vector<MockFile> m_test_files;

    static std::string get_new_filename() {
        return "test_file_" + std::to_string(m_counter++);
    }
    static std::string get_new_dirname() {
        return "test_dir_" + std::to_string(m_counter++);
    }

    static MockFile get_random_file() {
        return m_test_files[STest::Pick::lowerUpper(0, m_test_files.size() - 1)];
    }
    static MockDirectory get_random_directory() {
        return m_test_dirs[STest::Pick::lowerUpper(0, m_test_dirs.size() - 1)];
    }

    static void touch_file(std::string path) {
        m_test_files.push_back(MockFile(path, ""));
    }
    static void create_directory(std::string path) {
        m_test_dirs.push_back(MockDirectory(path));
    }
    static void move_file(std::string source, std::string dest) {
        for (MockFile& file : m_test_files) {
            if (file.path == source) {
                file.path = dest;
                return;
            }
        }
    }
    static void copy_file(std::string source, std::string dest) {
        for (MockFile& file : m_test_files) {
            if (file.path == source) {
                m_test_files.push_back(MockFile(dest, file.contents));
                return;
            }
        }
    }
    static void append_file(std::string source, std::string dest, bool createMissingDest) {
        for (MockFile& source_file : m_test_files) {
            if (source_file.path == source) {
                for (MockFile& dest_file : m_test_files) {
                    if (dest_file.path == dest) {
                        dest_file.contents += source_file.contents;
                        return;
                    }
                }
                if (createMissingDest) {
                    m_test_files.push_back(MockFile(dest, source_file.contents));
                    return;
                }
            }
        }
        std::cout << "Failed to append file: source=" << source << ", dest=" << dest << std::endl;
    }
    static void remove_file(std::string path) {
        for (auto it = m_test_files.begin(); it != m_test_files.end(); ++it) {
            if (it->path == path) {
                m_test_files.erase(it);
                return;
            }
        }
    }


    // //! \brief Add file to tracking list, to be removed in teardown
    // static void track_file_for_cleanup(std::string filename) {
    //     m_test_files.push_back(filename);
    // }
    // //! \brief Add directory to tracking list, to be removed in teardown
    // static void track_directory_for_cleanup(std::string directory) {
    //     m_test_dirs.push_back(directory);
    // }
    // static void add_file(std::string path) {
    //     m_test_files.push_back(path);
    //     Os::File file;
    //     file.open(path.c_str(), Os::File::OPEN_CREATE);
    //     file.close();
    // }
    // static void add_directory(std::string path) {
    //     m_test_dirs.push_back(path);
    //     Os::Directory dir;
    //     dir.open(path.c_str(), Os::Directory::OpenMode::CREATE);
    //     dir.close();
    // }

// Do NOT alter, adds rules to Tester as inner classes
#include "FileSystemRules.hpp"
};

}  // namespace FileSystem
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
