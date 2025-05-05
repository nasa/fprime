// ======================================================================
// \title Os/Stub/DefaultFile.cpp
// \brief sets default Os::File to no-op stub implementation via linker
// ======================================================================
#include "Os/Stub/File.hpp"
#include "Os/Stub/FileSystem.hpp"
#include "Os/Stub/Directory.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for FileInterface that intercepts calls for stub file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileInterface *FileInterface::getDelegate(FileHandleStorage& aligned_placement_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Stub::File::StubFile>(
            aligned_placement_new_memory, to_copy
    );
}

//! \brief get a delegate for FileSystemInterface that intercepts calls for stub fileSystem usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileSystemInterface *FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Stub::FileSystem::StubFileSystem>(
        aligned_placement_new_memory
    );
}

//! \brief get a delegate for DirectoryInterface that intercepts calls for stub Directory usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
DirectoryInterface *DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Stub::Directory::StubDirectory>(
        aligned_placement_new_memory
    );
}
}
