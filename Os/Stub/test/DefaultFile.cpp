// ======================================================================
// \title Os/Stub/test/DefaultFile.cpp
// \brief sets default Os::File to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/File.hpp"
#include "Fw/Types/Assert.hpp"
#include <new>
namespace Os {
//! \brief get implementation file interface for use as delegate
//!
//! Added via linker to ensure that the tracking test stub implementation of Os::File is the default.
//!
//! \param aligned_placement_new_memory: memory to placement-new into
//! \return FileInterface pointer result of placement new
//!
FileInterface *FileInterface::getDelegate(U8 *aligned_placement_new_memory, const FileInterface* to_copy) {
    FW_ASSERT(aligned_placement_new_memory != nullptr);
    const Os::Stub::File::Test::TestFile* copy_me = reinterpret_cast<const Os::Stub::File::Test::TestFile*>(to_copy);
    // Placement-new the file handle into the opaque file-handle storage
    static_assert(sizeof(Os::Stub::File::Test::TestFile) <= sizeof Os::File::m_handle_storage,
            "Handle size not large enough");
    static_assert((FW_HANDLE_ALIGNMENT % alignof(Os::Stub::File::Test::TestFile)) == 0, "Handle alignment invalid");
    Os::Stub::File::Test::TestFile *interface = nullptr;
    if (to_copy == nullptr) {
        interface = new(aligned_placement_new_memory) Os::Stub::File::Test::TestFile;
    } else {
        interface = new(aligned_placement_new_memory) Os::Stub::File::Test::TestFile(*copy_me);
    }
    FW_ASSERT(interface != nullptr);
    return interface;
}
}
