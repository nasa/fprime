#ifndef SIMPLE_ACTIVE_IMPL_HPP
#define SIMPLE_ACTIVE_IMPL_HPP

#include <Autocoders/Python/test/active_tester/Simple_ActiveComponentAc.hpp>

namespace Simple_Active{

    class Simple_ActiveImpl : public Simple_ActiveComponentBase{
    public:
        Simple_ActiveImpl(const char* compName);
        ~Simple_ActiveImpl(void);
        void init(NATIVE_INT_TYPE queueDepth);
    private:
        void U32_Async_handler(NATIVE_INT_TYPE portNum, U32 int_arg);
        void F32_Sync_handler(NATIVE_INT_TYPE portNUm, F32 float_arg);
    };


};

#endif
