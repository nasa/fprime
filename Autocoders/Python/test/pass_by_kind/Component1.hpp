#ifndef EXAMPLE_ENUM_IMPL_HPP
#define EXAMPLE_ENUM_IMPL_HPP

#include <Autocoders/Python/test/pass_by_kind/Component1ComponentAc.hpp>

namespace Example {

    class Component1 : public Component1ComponentBase  {
    public:

        // Only called by derived class
        Component1(const char* compName);
        ~Component1(void);
        void init(NATIVE_INT_TYPE queueDepth);

    private:
        void AsyncPort_handler(NATIVE_INT_TYPE portNum,U32 *arg1, U32 &arg2, U32 arg3, U32 arg4,
        ExampleType *arg5, ExampleType &arg6, ExampleType arg7, const ExampleType &arg8,
        Arg9String *arg9, Arg10String &arg10, Arg11String arg11, const Arg12String &arg12);

        void SyncPort_handler(NATIVE_INT_TYPE portNum,U32 *arg1, U32 &arg2, U32 arg3, U32 arg4,
        ExampleType *arg5, ExampleType &arg6, ExampleType arg7, const ExampleType &arg8,
        Arg9String *arg9, Arg10String &arg10, Arg11String arg11, const Arg12String &arg12);
    };

};

#endif
