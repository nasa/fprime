#include "FileTester.hpp"

namespace Os {

FileInterface* FileInterface::getDelegate(FileHandleStorage& aligned_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Svc::FileTester, FileHandleStorage>(aligned_new_memory, to_copy);
}
FileSystemInterface* FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Posix::FileSystem::PosixFileSystem>(aligned_new_memory);
}
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Posix::Directory::PosixDirectory>(aligned_new_memory);
}

}  // namespace Os

namespace Svc {
Os::FileInterface::Status FileTester::m_statOpen = Os::FileInterface::Status();
Os::FileInterface::Status FileTester::m_statRead = Os::FileInterface::Status();
Os::FileInterface::Status FileTester::m_statWrite = Os::FileInterface::Status();
Os::FileInterface::Status FileTester::m_statSize = Os::FileInterface::Status();
FwSizeType FileTester::m_size = 0;
}  // namespace Svc
