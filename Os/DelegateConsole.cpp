// ======================================================================
// \title Os/DelegateConsole.cpp
// \brief common function implementation for Os::Console
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Console.hpp>
#include <Os/DelegateConsole.hpp>

namespace Os {
DelegateConsole::DelegateConsole()
    : ConsoleInterface(), m_handle_storage(), m_delegate(*ConsoleInterface::getDelegate(m_handle_storage)) {}

DelegateConsole::~DelegateConsole() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage[0]));
    m_delegate.~ConsoleInterface();
}

DelegateConsole::DelegateConsole(const DelegateConsole& other)
    : m_handle_storage(), m_delegate(*ConsoleInterface::getDelegate(m_handle_storage, &other.m_delegate)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage[0]));
}

DelegateConsole& DelegateConsole::operator=(const DelegateConsole& other) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage[0]));
    if (this != &other) {
        this->m_delegate = *ConsoleInterface::getDelegate(m_handle_storage, &other.m_delegate);
    }
    return *this;
}

void DelegateConsole::writeMessage(const CHAR* message, const FwSizeType size) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
    FW_ASSERT(message != nullptr || size == 0);
    this->m_delegate.writeMessage(message, size);
}

ConsoleHandle* DelegateConsole::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
    return this->m_delegate.getHandle();
}

void DelegateConsole::write(const CHAR* message, const FwSizeType size) {
    DelegateConsole::getSingleton().writeMessage(message, size);
}

void DelegateConsole::write(const Fw::ConstStringBase& message) {
    DelegateConsole::getSingleton().writeMessage(message.toChar(), message.length());
}

void DelegateConsole::init() {
    // Force trigger on the fly singleton setup
    (void)DelegateConsole::getSingleton();
}

DelegateConsole& DelegateConsole::getSingleton() {
    static DelegateConsole s_singleton;
    // Registration happens once: re-registering on every access would silently replace a
    // logger the project registered after the console singleton was first used
    static bool s_registered = false;
    if (not s_registered) {
        s_registered = true;
        Fw::Logger::registerLogger(&s_singleton);
    }
    return s_singleton;
}

// ------------------------------------------------------------------
// Common virtual functions built on top of OS-specific functions
// ------------------------------------------------------------------

void ConsoleInterface::writeMessage(const Fw::ConstStringBase& message) {
    this->writeMessage(message.toChar(), message.length());
}
}  // namespace Os
