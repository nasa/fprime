#include <Autocoders/Python/test/passive/PassiveC2Impl.hpp>
#include <Fw/Types/FwBasicTypes.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Passive {

        C2Impl::C2Impl(const char* compName) : C2Base(compName) {

        }

        C2Impl::~C2Impl(void) {

        }

        // Internal call - implemented by hand.
        // downcall for input port Void
        I32 C2Impl::Void_Void_handler(void) {
			printf("*** %s: Void_Void_handler down-call\n", this->m_objName);
            return 0;
        }

        // downcall for input port Msg1
        I32 C2Impl::Msg1_Msg1_handler(U32 arg1, I32 arg2, F32 arg3) {
			printf("*** %s: Msg1_Msg1_handler(%u,%ld,%f) down-call\n", this->m_objName,arg1,arg2,arg3);
			this->Msg2_Msg2_out(arg1,arg2,arg3,"Synchronous Test String");
            return 0;
        }

};


