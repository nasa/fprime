#include <Autocoders/Python/test/app1/DuckImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Duck {

		DuckImpl::DuckImpl(const char* compName) : DuckComponentBase(compName) {

		}

		DuckImpl::~DuckImpl(void) {

		}

		void DuckImpl::init(NATIVE_INT_TYPE queueDepth) {
			DuckComponentBase::init(queueDepth);
		}

		// Internal call - implemented by hand.
		// downcall for input port ExtCmdIn
		void DuckImpl:: ExtCmdIn_handler(NATIVE_INT_TYPE portNum, U32 cmd, Fw::EightyCharString str) {
			printf("*** %d %s: ExtCmdIn_Msg1_handler down-call\n", portNum, this->m_objName);
			this->CmdOut_out(portNum, cmd, str);
		}

		// downcall for input port CmdIn
		void DuckImpl::CmdIn_handler(NATIVE_INT_TYPE portNum, U32 cmd, Fw::EightyCharString str) {
			printf("*** %d %s: CmdIn_Msg1_handler down-call\n", portNum, this->m_objName);
			printf("*** %s: cmd = %d str = %s\n", this->m_objName, cmd, str.toChar());
		}
};
