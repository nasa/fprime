// ======================================================================
// \title Os/Posix/DefaultFileSystem.cpp
// \brief sets default Os::FileSystem to posix implementation via linker
// ======================================================================
#include "Os/FileSystem.hpp"
#include "Os/Posix/FileSystem.hpp"
#include "Os/Delegate.hpp"

namespace Os {
FileSystemInterface* FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Posix::FileSystem::PosixFileSystem>(aligned_new_memory);
}
}
