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
            m_test_files.push_back(file.path);
            Os::File f;
            f.open(file.path.c_str(), Os::File::OPEN_CREATE);
            FwSignedSizeType bytesRead = contents.size();
            f.write(reinterpret_cast<const U8*>(contents.c_str()), bytesRead);
            f.close();
        }
        void add_directory(std::string name) {
            MockDirectory dir = MockDirectory(path + "/" + name);
            directories.push_back(dir);
            m_test_dirs.push_back(dir.path);
            Os::Directory d;
            d.open(dir.path.c_str(), Os::Directory::OpenMode::CREATE);
            d.close();
        }
    };
    // struct FileSystemTestNode {
    //     std::string path;
    //     bool is_directory;
    //     std::vector<FileSystemTestNode> children;
    //     FileSystemTestNode(std::string path, bool is_directory) : path(path), is_directory(is_directory) {}
    //     void add_child_file(std::string name) {
    //         FileSystemTestNode child = FileSystemTestNode(path + "/" + name, false);
    //         children.push_back(child);
    //         Os::FileSystem::touch((child.path).c_str());
    //     }
    //     void add_child_dir(std::string name) {
    //         FileSystemTestNode child = FileSystemTestNode(path + "/" + name, true);
    //         children.push_back(child);
    //         Os::FileSystem::createDirectory((child.path).c_str());
    //     }
    //     void recurse_remove() {
    //         for (auto child : children) {
    //             child.recurse_remove();
    //         }
    //         if (is_directory) {
    //             Os::FileSystem::removeDirectory(path.c_str());
    //         } else {
    //             Os::FileSystem::removeFile(path.c_str());
    //         }
    //     }
    //     std::string get_random_file_child () {
    //         return children[STest::Pick::lowerUpper(0, children.size() - 1)].path;
    //     } // TODO: we need to figure out how to retrieve either a file or a directory, on demand
    //     // std::string get_random_file_child() {
    //     //     std::vector<FileSystemTestNode> filtered_children;
    //     //     std::copy_if(children.begin(), children.end(), std::back_inserter(filtered_children), [](const FileSystemTestNode& child) {
    //     //         return !child.is_directory;
    //     //     });
    //     //     if (filtered_children.empty()) {
    //     //         throw std::runtime_error("No file children available");
    //     //     }
    //     //     return filtered_children[STest::Pick::lowerUpper(0, filtered_children.size() - 1)].path;
    //     // }
    // }; //!< Mock representation of a filesystem node

    static MockDirectory m_testdir_root;
    // FileSystemTestNode m_testdir_root = FileSystemTestNode("filesystem_test_directory", true);

    // TODO: rename to s_* for static
    static std::vector<std::string> m_test_dirs;
    static std::vector<std::string> m_test_files;


    static std::string get_random_file() {
        return m_test_files[STest::Pick::lowerUpper(0, m_test_files.size() - 1)];
    }
    static std::string get_random_directory() {
        return m_test_dirs[STest::Pick::lowerUpper(0, m_test_dirs.size() - 1)];
    }
    //! \brief Add file to tracking list, to be removed in teardown
    static void track_file_for_cleanup(std::string filename) {
        m_test_files.push_back(filename);
    }
    //! \brief Add directory to tracking list, to be removed in teardown
    static void track_directory_for_cleanup(std::string directory) {
        m_test_dirs.push_back(directory);
    }
    static void add_file(std::string path) {
        m_test_files.push_back(path);
        Os::File file;
        file.open(path.c_str(), Os::File::OPEN_CREATE);
        file.close();
    }
    static void add_directory(std::string path) {
        m_test_dirs.push_back(path);
        Os::Directory dir;
        dir.open(path.c_str(), Os::Directory::OpenMode::CREATE);
        dir.close();
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "FileSystemRules.hpp"
};

}  // namespace FileSystem
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
