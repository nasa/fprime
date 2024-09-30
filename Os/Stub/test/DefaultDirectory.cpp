// ======================================================================
// \title Os/Stub/DefaultDirectory.cpp
// \brief sets default Os::Directory to Stub implementation via linker
// ======================================================================
// #include "Os/Directory.hpp"
#include "Os/Stub/test/Directory.hpp"
#include "Os/Delegate.hpp"

namespace Os {
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Stub::Directory::Test::TestDirectory>(aligned_new_memory);
}
}
