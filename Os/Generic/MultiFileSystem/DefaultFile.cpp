// ======================================================================
// \title Os/Multi/DefaultFileSystem.cpp
// \brief sets default Os::FileSystem to generic file system implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Generic/MultiFileSystem/Directory.hpp"
#include "Os/Generic/MultiFileSystem/File.hpp"
#include "Os/Generic/MultiFileSystem/FileSystem.hpp"

namespace Os {
FileInterface* FileInterface::getDelegate(FileHandleStorage& aligned_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Generic::MultiFile>(aligned_new_memory, to_copy);
}

FileSystemInterface* FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Generic::MultiFileSystem>(aligned_new_memory);
}

// NOTE: why doesn't DirectoryInterface::getDelegate have a to_copy argument ??
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Generic::MultiDirectory>(aligned_new_memory);
}
}  // namespace Os
