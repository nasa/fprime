#ifndef PASSIVEC2IMPL_HPP
#define PASSIVEC2IMPL_HPP

#include <Autocoders/Python/test/passive/PassiveC2ComponentAc.hpp>

namespace Passive {

    class C2Impl : public C2Base  {
    public:

        // Only called by derived class
        C2Impl(const char* compName);
        ~C2Impl(void);

    private:
        // downcall for input ports
        I32 Void_Void_handler(void);
        I32 Msg1_Msg1_handler(U32 arg1, I32 arg2, F32 arg3);
    };
};

#endif



