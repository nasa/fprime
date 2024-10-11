// ======================================================================
// \title Os/Stub/DefaultConsole.cpp
// \brief sets default Os::Console to stub implementation via linker
// ======================================================================
#include "Os/Console.hpp"
#include "Os/Stub/Console.hpp"
#include "Os/Delegate.hpp"

namespace Os {
ConsoleInterface* ConsoleInterface::getDelegate(ConsoleHandleStorage& aligned_new_memory, const ConsoleInterface* to_copy) {
    return Os::Delegate::makeDelegate<ConsoleInterface, Os::Stub::Console::StubConsole>(aligned_new_memory, to_copy);
}
}
