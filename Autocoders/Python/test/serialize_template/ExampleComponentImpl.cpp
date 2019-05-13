/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/serialize_template/ExampleComponentImpl.hpp>
#include <Fw/Types/FwEightyCharString.hpp>
#include <cstdio>

#if FW_OBJECT_NAMES == 1
ExampleComponentImpl::ExampleComponentImpl(const char* name) :  ExampleComponents::ExampleComponentBase(name)
#else
ExampleComponentImpl::ExampleComponentImpl() :  ExampleComponents::ExampleComponentBase()
#endif
{
}

void ExampleComponentImpl::init(NATIVE_INT_TYPE queueDepth) {
    ExampleComponents::ExampleComponentBase::init(queueDepth);
}

ExampleComponentImpl::~ExampleComponentImpl() {
}

void ExampleComponentImpl::exampleInput_Example_handler(NATIVE_INT_TYPE portNum, I32 arg1, Fw::StructSerializable<SomeUserStruct> arg2) {

    SomeUserStruct s = arg2.get();
    printf("ARG: %d %f %d\n",s.mem1,s.mem2,s.mem3);
}
