// ======================================================================
// \title Os/Stub/DefaultFile.cpp
// \brief sets default Os::File to no-op stub implementation via linker
// ======================================================================
#include "Os/Stub/File.hpp"
#include "Fw/Types/Assert.hpp"
#include <new>
namespace Os {
//! \brief get implementation file interface for use as delegate
//!
//! Added via linker to ensure that the no-op stub implementation of Os::File is the default.
//!
//! \param aligned_placement_new_memory: memory to placement-new into
//! \return FileInterface pointer result of placement new
//!
FileInterface *FileInterface::getDelegate(U8 *aligned_placement_new_memory, const FileInterface* to_copy) {
    FW_ASSERT(aligned_placement_new_memory != nullptr);
    const Os::Stub::File::StubFile* copy_me = reinterpret_cast<const Os::Stub::File::StubFile*>(to_copy);
    // Placement-new the file handle into the opaque file-handle storage
    static_assert(sizeof(Os::Stub::File::StubFile) <= sizeof Os::File::m_handle_storage, "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Stub::File::StubFile)) == 0, "Handle alignment invalid");
    Os::Stub::File::StubFile *interface = nullptr;
    if (to_copy == nullptr) {
        interface = new(aligned_placement_new_memory) Os::Stub::File::StubFile;
    } else {
        interface = new(aligned_placement_new_memory) Os::Stub::File::StubFile(*copy_me);
    }
    FW_ASSERT(interface != nullptr);
    return interface;
}
}
