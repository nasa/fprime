// ======================================================================
// \title Os/Console.cpp
// \brief common function implementation for Os::Console
// ======================================================================
#include <Os/Console.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
    Console Console::s_singleton;
    Console::Console() : ConsoleInterface(), Fw::Logger(), m_delegate(*ConsoleInterface::getDelegate(m_handle_storage)) {
        // Register myself as a logger at construction time. If used in unison with LogDefault.cpp, this will
        // automatically create this as a default logger.
        Fw::Logger::registerLogger(this);
    }

    Console::~Console() {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage[0]));
        m_delegate.~ConsoleInterface();
    }

    Console::Console(const Console& other) :
          m_handle_storage(),
          m_delegate(*Console::getDelegate(m_handle_storage, &other.m_delegate)) {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<Console*>(&this->m_handle_storage[0]));
    }

    Console& Console::operator=(const Console& other) {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<Console*>(&this->m_handle_storage[0]));
        if (this != &other) {
            this->m_delegate = *ConsoleInterface::getDelegate(m_handle_storage, &other.m_delegate);
        }
        return *this;
    }

    // Instance implementation
    void Console::write(const CHAR *message, const FwSizeType size) {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
        FW_ASSERT(message != nullptr || size == 0);
        this->m_delegate.write(message, size);
    }

    ConsoleHandle* Console::getHandle() {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
        return this->m_delegate.getHandle();
    }

    void Console::writeGlobal(const CHAR* message, const FwSizeType size) {
        FW_ASSERT(message != nullptr || size == 0);
        Console::getSingleton().write(message, size);
    }

    Console& Console::getSingleton() {
        return s_singleton;
    }
}

