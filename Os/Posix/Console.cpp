// ======================================================================
// \title Os/Posix/Console.cpp
// \brief posix implementation for Os::Console
// ======================================================================
#include <Os/Posix/Console.hpp>
#include <limits>
#include <cstdio>

static_assert(std::numeric_limits<FwSizeType>::max() <= std::numeric_limits<size_t>::max(), "FwSizeType is too large to fit in size_t limit");
namespace Os {
namespace Posix {
namespace Console {


void PosixConsole::write(const CHAR *message, const FwSizeType size) {
    ::fwrite(message, sizeof(CHAR), size, this->m_handle.m_file_descriptor);
}

ConsoleHandle* PosixConsole::getHandle() {
    return &this->m_handle;
}

void PosixConsole ::setOutputStream(Stream stream) {
    this->m_handle.m_file_descriptor = (stream == STANDARD_ERROR) ? stderr : stdout;
}


} // namespace Console
} // namespace Posix
} // namespace Os