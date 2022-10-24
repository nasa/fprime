#include <Autocoders/Python/test/partition/DuckDuckImpl.hpp>
#include <FpConfig.hpp>
#include <iostream>
#include <cstdio>

using namespace std;

namespace Duck {

        DuckImpl::DuckImpl(const char* compName) : DuckBase(compName) {

        }

        DuckImpl::~DuckImpl() {

        }

        // Internal call - implemented by hand.
        // downcall for input port externInputPort1
        I32 DuckImpl::externInputPort1_Msg1_handler(U32 cmd, Fw::String str) {
            // User code is written here.
			printf("\n\t*** %s: externInputPort1_Msg1_handler down-call\n", this->m_objName);
			this->outputPort1_Msg1_out(cmd, str);
            return 0;
        }

        // downcall for input port externInputPort3
        I32 DuckImpl::externInputPort3_Msg3_handler(U32 cmd) {
            // User code is written here.
			printf("\n\t*** %s: externInputPort3_Msg3_handler down-call\n", this->m_objName);
        	outputPort2_Msg3_out(cmd);
            return 0;
        }

        // downcall for input port inputPort1
        I32 DuckImpl::inputPort1_Msg1_handler(U32 cmd, Fw::String str) {
            // User code is written here.
            return 0;
        }

        // downcall for input port inputPort2
        I32 DuckImpl::inputPort2_Msg1_handler(U32 cmd, Fw::String str) {
            // User code is written here.
			printf("\n\t*** %s: inputPort2_Msg1_handler(%d, %s) down-call\n", this->m_objName, cmd, str.toChar());
            return 0;
        }

        // downcall for input port inputPort3
        I32 DuckImpl::inputPort3_Msg3_handler(U32 cmd) {
            // User code is written here.
			printf("\n\t*** %s: inputPort3_Msg3_handler(%d) down-call\n", this->m_objName, cmd);
            return 0;
        }

};


