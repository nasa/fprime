#ifndef EXAMPLE_ENUM_IMPL_HPP
#define EXAMPLE_ENUM_IMPL_HPP

#include <Autocoders/Python/test/pass_by_kind/Component1ComponentAc.hpp>

namespace Example {

    class Component1 : public Component1ComponentBase  {
    public:

        // Only called by derived class
        Component1(const char* compName);
        ~Component1();
        void init(NATIVE_INT_TYPE queueDepth);

    private:
        void AsyncPort_handler(FwIndexType portNum,U32 *arg1, U32 &arg2, U32 arg3,
        ExampleType *arg4, ExampleType &arg5, const ExampleType &arg6,
        Arg7String *arg7, Arg8String &arg8, const Arg9String &arg9);

        void SyncPort_handler(FwIndexType portNum,U32 *arg1, U32 &arg2, U32 arg3,
        ExampleType *arg4, ExampleType &arg5, const ExampleType &arg6,
        Arg7String *arg7, Arg8String &arg8, const Arg9String &arg9);
    };

};

#endif
