// ======================================================================
// \title Os/Posix/DefaultErrorConsole.cpp
// \brief sets default Os::Console to posix implementation via linker
// ======================================================================
#include "Os/Console.hpp"
#include "Os/Posix/Console.hpp"
#include "Os/Delegate.hpp"

namespace Os {
ConsoleInterface* ConsoleInterface::getDelegate(ConsoleHandleStorage& aligned_new_memory, const ConsoleInterface* to_copy) {
    auto console = Os::Delegate::makeDelegate<ConsoleInterface, Os::Posix::Console::PosixConsole>(aligned_new_memory, to_copy);
    // Override the output file
    reinterpret_cast<Os::Posix::Console::PosixConsoleHandle*>(console->getHandle())->m_file_descriptor = stderr;
    return console;
}
}
