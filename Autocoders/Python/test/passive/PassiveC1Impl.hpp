#ifndef PASSIVEC1IMPL_HPP
#define PASSIVEC1IMPL_HPP

#include <Autocoders/Python/test/passive/PassiveC1ComponentAc.hpp>

namespace Passive {

    class C1Impl : public C1Base  {
    public:

        // Only called by derived class
        C1Impl(const char* compName);
        ~C1Impl(void);

    private:
        // downcall for input ports
        I32 Ext1_Void_handler(void);
        I32 Ext2_Msg1_handler(U32 arg1, I32 arg2, F32 arg3);
        I32 Sync_Msg2_handler(U32 arg1, I32 arg2, F32 arg3, Fw::EightyCharString str);
    };
};

#endif



