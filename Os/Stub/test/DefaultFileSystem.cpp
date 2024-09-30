// ======================================================================
// \title Os/Stub/test/DefaultFileSystem.cpp
// \brief sets default Os::FileSystem to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/FileSystem.hpp"
#include "Os/Delegate.hpp"

namespace Os {

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
