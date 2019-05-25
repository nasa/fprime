/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND2IMPL_HPP_
#define TESTCOMMAND2IMPL_HPP_

#include <Autocoders/Python/test/serialize_template/ExampleComponentAc.hpp>

class ExampleComponentImpl: public ExampleComponents::ExampleComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        ExampleComponentImpl(const char* compName);
#else
        ExampleComponentImpl();
#endif
        void init(NATIVE_INT_TYPE queueDepth);
        virtual ~ExampleComponentImpl();
    private:
        void exampleInput_Example_handler(NATIVE_INT_TYPE portNum, I32 arg1, Fw::StructSerializable<SomeUserStruct> arg2);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
