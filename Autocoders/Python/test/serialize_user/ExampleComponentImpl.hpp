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
        ExampleComponentImpl(const char* compName);
        void init(NATIVE_INT_TYPE queueDepth);
        virtual ~ExampleComponentImpl();
    private:
        void exampleInput_handler(FwIndexType portNum, I32 arg1, const ANameSpace::UserSerializer& arg2);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
