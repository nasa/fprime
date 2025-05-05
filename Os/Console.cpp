// ======================================================================
// \title Os/Console.cpp
// \brief common function implementation for Os::Console
// ======================================================================
#include <Os/Console.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
    Console::Console() : ConsoleInterface(), Fw::Logger(), m_handle_storage(), m_delegate(*ConsoleInterface::getDelegate(m_handle_storage)) {}

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

    void Console::writeMessage(const CHAR *message, const FwSizeType size) {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
        FW_ASSERT(message != nullptr || size == 0);
        this->m_delegate.writeMessage(message, size);
    }

    void Console::writeMessage(const Fw::StringBase& message) {
        this->writeMessage(message.toChar(), message.length());
    }

    ConsoleHandle* Console::getHandle() {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
        return this->m_delegate.getHandle();
    }

    void Console::write(const CHAR *message, const FwSizeType size) {
        Console::getSingleton().writeMessage(message, size);
    }

    void Console::write(const Fw::StringBase& message) {
        Console::getSingleton().writeMessage(message.toChar(), message.length());
    }

    void Console::init() {
        // Force trigger on the fly singleton setup
        (void) Console::getSingleton();
    }

    Console& Console::getSingleton() {
        static Console s_singleton;
        Fw::Logger::registerLogger(&s_singleton);
        return s_singleton;
    }
}

