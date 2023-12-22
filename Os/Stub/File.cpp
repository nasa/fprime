// ======================================================================
// \title Os/Stub/File.cpp
// \brief stub implementation for Os::File
// ======================================================================
#include <Os/Stub/File.hpp>

namespace Os {
namespace Stub {
namespace { // Prevents external linkage of this namespaced internal variable
    Os::File::Status NEXT_STATUS = Os::File::Status::OP_OK;
} // Anonymous namespace
    void file_set_next_status(Os::File::Status status) {
        Os::Stub::NEXT_STATUS = status;
    }
} // Stub

File::Status File::openInternal(const char* path, File::Mode mode, bool overwrite) {
    return Os::Stub::NEXT_STATUS;
}

} // Os