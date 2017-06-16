#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {

#if FW_OBJECT_NAMES == 1
    ConsoleTextLoggerImpl::ConsoleTextLoggerImpl(const char* compName) :
        PassiveTextLoggerComponentBase(compName),m_displayLine(2),m_pointerLine(0) {
    }
#else
    ConsoleTextLoggerImpl::ConsoleTextLoggerImpl() :
        PassiveTextLoggerComponentBase(),m_displayLine(2),m_pointerLine(0) {
    }
#endif
    ConsoleTextLoggerImpl::~ConsoleTextLoggerImpl(void) {

    }

}
