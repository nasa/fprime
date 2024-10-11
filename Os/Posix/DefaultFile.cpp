// ======================================================================
// \title Os/Posix/DefaultFile.cpp
// \brief sets default Os::File to posix implementation via linker
// ======================================================================
#include "Os/File.hpp"
#include "Os/Posix/File.hpp"
#include "Os/FileSystem.hpp"
#include "Os/Posix/FileSystem.hpp"
#include "Os/Directory.hpp"
#include "Os/Posix/Directory.hpp"
#include "Os/Delegate.hpp"

namespace Os {
FileInterface* FileInterface::getDelegate(FileHandleStorage& aligned_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Posix::File::PosixFile>(aligned_new_memory, to_copy);
}
FileSystemInterface* FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Posix::FileSystem::PosixFileSystem>(aligned_new_memory);
}
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Posix::Directory::PosixDirectory>(aligned_new_memory);
}
}
