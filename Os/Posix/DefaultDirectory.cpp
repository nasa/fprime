// ======================================================================
// \title Os/Posix/DefaultDirectory.cpp
// \brief sets default Os::Directory to posix implementation via linker
// ======================================================================
#include "Os/Directory.hpp"
#include "Os/Posix/Directory.hpp"
#include "Os/Delegate.hpp"

namespace Os {
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Posix::Directory::PosixDirectory>(aligned_new_memory);
}
}
