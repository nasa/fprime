// ======================================================================
// \title Os/Stub/Console.cpp
// \brief stub implementation for Os::Console
// ======================================================================
#include <Os/Posix/Console.hpp>

namespace Os {
namespace Posix {
namespace Console {


void PosixConsole::write(const CHAR *message, const FwSizeType size) {
}

ConsoleHandle* PosixConsole::getHandle() {
    return &this->m_handle;
}



} // namespace Console
} // namespace Stub
} // namespace Os