// ======================================================================
// \title Os/Stub/test/DefaultFile.cpp
// \brief sets default Os::File to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/File.hpp"
#include "Os/Stub/test/Directory.hpp"
#include "Os/Stub/test/FileSystem.hpp"
#include "Os/Delegate.hpp"

namespace Os {

//! \brief get a delegate for FileInterface that intercepts calls for  for stub test file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileInterface *FileInterface::getDelegate(FileHandleStorage& aligned_placement_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Stub::File::Test::TestFile>(
            aligned_placement_new_memory, to_copy
    );
}
//! \brief get a delegate for Directory that intercepts calls for  for stub test file usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Stub::Directory::Test::TestDirectory>(aligned_new_memory);
}

//! \brief get a delegate for FileSystemInterface that intercepts calls for  for stub test file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileSystemInterface *FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Stub::FileSystem::Test::TestFileSystem>(
        aligned_placement_new_memory
    );
}
}
