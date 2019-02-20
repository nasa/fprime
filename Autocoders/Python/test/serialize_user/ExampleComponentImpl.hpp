/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND2IMPL_HPP_
#define TESTCOMMAND2IMPL_HPP_

#include <Autocoders/Python/test/serialize_user/ExampleComponentAc.hpp>

class ExampleComponentImpl: public ExampleComponents::ExampleComponentComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        ExampleComponentImpl(const char* compName);
#else
        ExampleComponentImpl();
#endif
        void init(NATIVE_INT_TYPE queueDepth);
        virtual ~ExampleComponentImpl();
    private:
        void exampleInput_handler(NATIVE_INT_TYPE portNum, I32 arg1, ANameSpace::UserSerializer arg2);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
