#ifndef SVC_TEXT_LOGGER_IMPL_HPP
#define SVC_TEXT_LOGGER_IMPL_HPP

#include <Svc/PassiveConsoleTextLogger/PassiveTextLoggerComponentAc.hpp>

namespace Svc {

	class ConsoleTextLoggerImpl : public PassiveTextLoggerComponentBase  {
		
	public:

		// Only called by derived class 
	    ConsoleTextLoggerImpl(const char* compName);
	    void init(void);
		~ConsoleTextLoggerImpl(void);
		
	private:

		// downcalls for input ports
        void TextLogger_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text);
        
        // Track which line of the display we're on
        NATIVE_INT_TYPE m_displayLine;
        NATIVE_INT_TYPE m_pointerLine;
        
	};
	
}

#endif
