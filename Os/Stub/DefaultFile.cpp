// ======================================================================
// \title Os/Stub/DefaultFile.cpp
// \brief sets default Os::File to no-op stub implementation via linker
// ======================================================================
#include "Os/Stub/File.hpp"
#include "Os/Delegate.hpp"
namespace Os {

//! \brief get a delegate for FileInterface that intercepts calls for parameter database testing
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileInterface *FileInterface::getDelegate(U8 *aligned_placement_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Os::Stub::File::StubFile>(
            aligned_placement_new_memory, to_copy
    );
}
}
