// ======================================================================
// \title Os/Posix/Console.cpp
// \brief posix implementation for Os::Console
// ======================================================================
#include <Os/Posix/Console.hpp>
#include <Fw/Types/Assert.hpp>
#include <limits>
#include <cstdio>

static_assert(std::numeric_limits<FwSizeType>::max() <= std::numeric_limits<size_t>::max(), "FwSizeType is too large to fit in size_t limit");
namespace Os {
namespace Posix {
namespace Console {


void PosixConsole::writeMessage(const CHAR *message, const FwSizeType size) {
    (void) ::fwrite(message, sizeof(CHAR), size, this->m_handle.m_file_descriptor);
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