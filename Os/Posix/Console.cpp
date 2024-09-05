// ======================================================================
// \title Os/Posix/Console.cpp
// \brief posix implementation for Os::Console
// ======================================================================
#include <Os/Posix/Console.hpp>
#include <Fw/Types/Assert.hpp>
#include <limits>
#include <cstdio>

namespace Os {
namespace Posix {
namespace Console {


void PosixConsole::writeMessage(const CHAR *message, const FwSizeType size) {
    // size_t is defined as different sizes on different platforms. Since FwSizeType is likely larger than size_t
    // on these platforms, and the user is unlikely to console-log more than size_t-max data, we cap the total
    // size at the limit of the interface.
    FwSizeType capped_size = (size <= std::numeric_limits<size_t>::max()) ? size : std::numeric_limits<size_t>::max();
    if (message != nullptr) {
        (void)::fwrite(message, sizeof(CHAR), static_cast<size_t>(capped_size), this->m_handle.m_file_descriptor);
        (void)::fflush(this->m_handle.m_file_descriptor);
    }
}

ConsoleHandle* PosixConsole::getHandle() {
    return &this->m_handle;
}

void PosixConsole ::setOutputStream(Stream stream) {
    switch (stream) {
        case STANDARD_OUT:
            this->m_handle.m_file_descriptor = stdout;
            break;
        case STANDARD_ERROR:
            this->m_handle.m_file_descriptor = stderr;
            break;
        default:
            FW_ASSERT(0);
            break;
    }
}


} // namespace Console
} // namespace Posix
} // namespace Os
