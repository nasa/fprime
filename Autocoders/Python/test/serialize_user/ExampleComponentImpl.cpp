/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/serialize_user/ExampleComponentImpl.hpp>
#include <Fw/Types/String.hpp>
#include <cstdio>

ExampleComponentImpl::ExampleComponentImpl(const char* name) :  ExampleComponents::ExampleComponentComponentBase(name)
{
}

void ExampleComponentImpl::init(NATIVE_INT_TYPE queueDepth) {
    ExampleComponents::ExampleComponentComponentBase::init(queueDepth);
}

ExampleComponentImpl::~ExampleComponentImpl() {
}

void ExampleComponentImpl::exampleInput_handler(FwIndexType portNum, I32 arg1, const ANameSpace::UserSerializer& arg2) {

    Fw::String str;
    arg2.toString(str);
    printf("ARG: %s\n",str.toChar());
}
