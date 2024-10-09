// ======================================================================
// \title Os/Stub/DefaultDirectory.cpp
// \brief sets default Os::Directory to no-op stub implementation via linker
// ======================================================================
#include "Os/Stub/Directory.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for DirectoryInterface that intercepts calls for stub Directory usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
DirectoryInterface *DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Stub::Directory::StubDirectory>(
            aligned_placement_new_memory
    );
}
}
