#include <Autocoders/Python/test/pass_by_kind/Component1.hpp>
#include <Autocoders/Python/test/pass_by_kind/ExampleTypeSerializableAc.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <cstdio>
#include <iostream>

using namespace std;

namespace Example {

    Component1::Component1(const char* compName) : Component1ComponentBase(compName) {
    }

    Component1::~Component1() {

    }

    void Component1::init(NATIVE_INT_TYPE queueDepth) {
        Component1ComponentBase::init(queueDepth);
    }

    void Component1::AsyncPort_handler(NATIVE_INT_TYPE portNum,U32 *arg1, U32 &arg2, U32 arg3,
        ExampleType *arg4, ExampleType &arg5, const ExampleType &arg6,
        Arg7String *arg7, Arg8String &arg8, const Arg9String &arg9) {
        *arg1 = 1;
        arg2 = 1;
        // arg3 is a stack variable -- assigning to it has no effect
        arg4->set(1);
        arg5.set(1);
        // arg6 is a const ref -- compiler won't let us modify through it
        *arg7 = "one";
        arg8 = "one";
        // arg9 is a const ref -- compiler won't let us modify through it
    }

    void Component1::SyncPort_handler(NATIVE_INT_TYPE portNum,U32 *arg1, U32 &arg2, U32 arg3,
        ExampleType *arg4, ExampleType &arg5, const ExampleType &arg6,
        Arg7String *arg7, Arg8String &arg8, const Arg9String &arg9) {
        *arg1 = 1;
        arg2 = 1;
        // arg3 is a stack variable -- assigning to it has no effect
        arg4->set(1);
        arg5.set(1);
        // arg6 is a const ref -- compiler won't let us modify through it
        *arg7 = "one";
        arg8 = "one";
        // arg9 is a const ref -- compiler won't let us modify through it
    }
};
