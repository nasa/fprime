// ======================================================================
// \title Os/Stub/Console.cpp
// \brief stub implementation for Os::Console
// ======================================================================
#include <Os/Stub/Console.hpp>

namespace Os {
namespace Stub {
namespace Console {


void StubConsole::write(const CHAR *message, const FwSizeType size) {
}

ConsoleHandle* StubConsole::getHandle() {
    return &this->m_handle;
}



} // namespace Console
} // namespace Stub
} // namespace Os