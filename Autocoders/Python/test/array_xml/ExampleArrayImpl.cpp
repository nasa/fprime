#include <Autocoders/Python/test/array_xml/ExampleArrayImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/String.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Example {

    ExampleArrayImpl::ExampleArrayImpl(const char* compName) : Component1ComponentBase(compName) {
    }

    ExampleArrayImpl::~ExampleArrayImpl(void) {

    }

    void ExampleArrayImpl::init(NATIVE_INT_TYPE queueDepth) {
        Component1ComponentBase::init(queueDepth);
    }

    void ExampleArrayImpl::ExArrayIn_handler(NATIVE_INT_TYPE portNum, Example::ArrayType array1, Example::ArrSerial serial1) {
        Fw::String s;
        array1.toString(s);

        printf("%s Invoked ExArrayIn_handler();\n%s", this->getObjName(), s.toChar());
        this->ArrayOut_out(0, array1, serial1);
    }
    
    void ExampleArrayImpl::ArrayIn_handler(NATIVE_INT_TYPE portNum, Example::ArrayType array1, Example::ArrSerial serial1) {
        Fw::String s;
        array1.toString(s);
        
        printf("%s Invoked ArrayIn_handler(%d);\n%s", this->getObjName(), portNum, s.toChar());
    }
};
