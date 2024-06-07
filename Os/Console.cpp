/**
 * File: Os/LogPrintf.cpp
 * Description: an implementation on the Os::Log abstraction that routes log messages into standard
 * printf calls.
 */
#include <Os/Console.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
    Console::Console() : ConsoleInterface(), Fw::Logger(), m_delegate(*ConsoleInterface::getDelegate(m_handle_storage)) {
        // Register myself as a logger at construction time. If used in unison with LogDefault.cpp, this will
        // automatically create this as a default logger.
        this->registerLogger(this);
    }

    // Instance implementation
    void Console::write(const CHAR *message, const FwSizeType size) {
        FW_ASSERT(&this->m_delegate == reinterpret_cast<ConsoleInterface*>(&this->m_handle_storage));
        this->m_delegate.write(message, size);
    }
}

