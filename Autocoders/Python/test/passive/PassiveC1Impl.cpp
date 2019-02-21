#include <Autocoders/Python/test/passive/PassiveC1Impl.hpp>
#include <Fw/Types/FwBasicTypes.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Passive {

        C1Impl::C1Impl(const char* compName) : C1Base(compName) {

        }

        C1Impl::~C1Impl(void) {

        }

        // Internal call - implemented by hand.
        // downcall for input port Ext1
        I32 C1Impl::Ext1_Void_handler(void) {
			printf("*** %s: Ext1_Void_handler down-call\n", this->m_objName);
			this->Void_Void_out();
            return 0;
        }

        // downcall for input port Ext2
        I32 C1Impl::Ext2_Msg1_handler(U32 arg1, I32 arg2, F32 arg3) {
			printf("*** %s: Ext2_Msg1_handler(%u,%ld,%f) down-call\n", this->m_objName, arg1, arg2, arg3);
			this->Msg1_Msg1_out(arg1,arg2,arg3);
            return 0;
        }

        // downcall for input port Sync
        I32 C1Impl::Sync_Msg2_handler(U32 arg1, I32 arg2, F32 arg3, Fw::EightyCharString str) {
            printf("*** %s: Sync_Msg2_handler(%u,%ld,%f,%s) down-call\n", this->m_objName, arg1,arg2,arg3,(char *)str.toChar());
            return 0;
        }

};


