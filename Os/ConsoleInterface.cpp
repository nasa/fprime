// ======================================================================
// \title Os/ConsoleInterface.cpp
// \brief common function implementation for Os::ConsoleInterface
// ======================================================================
#include <Os/ConsoleInterface.hpp>

namespace Os {

void ConsoleInterface::writeMessage(const Fw::ConstStringBase& message) {
    this->writeMessage(message.toChar(), message.length());
}

}  // namespace Os
