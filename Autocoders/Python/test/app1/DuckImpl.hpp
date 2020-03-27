#ifndef DUCK_DUCK_IMPL_HPP
#define DUCK_DUCK_IMPL_HPP

#include <Autocoders/Python/test/app1/DuckComponentAc.hpp>

namespace Duck {

	class DuckImpl : public DuckComponentBase  {
    public:

		// Only called by derived class
		DuckImpl(const char* compName);
		~DuckImpl(void);
		void init(NATIVE_INT_TYPE queueDepth);

    private:
		// downcall for input ports
        void ExtCmdIn_handler(NATIVE_INT_TYPE portNum, U32 cmd, Fw::EightyCharString str);
        void CmdIn_handler(NATIVE_INT_TYPE portNum, U32 cmd, Fw::EightyCharString str);
	};

};

#endif
