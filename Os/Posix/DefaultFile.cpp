// ======================================================================
// \title Os/Posix/DefaultFile.cpp
// \brief sets default Os::File to posix implementation via linker
// ======================================================================
#include "Os/File.hpp"
#include "Os/Posix/File.hpp"
#include "Os/Delegate.hpp"

namespace Os {
FileInterface* FileInterface::getDelegate(HandleStorage& aligned_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Posix::File::PosixFile>(aligned_new_memory, to_copy);
}
}
