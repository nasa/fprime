/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND1IMPL_HPP_
#define TESTCOMMAND1IMPL_HPP_

#include <Autocoders/Python/test/tlm_onchange/TestComponentAc.hpp>

class TestTlmImpl: public Tlm::TestTlmComponentBase {
    public:
        TestTlmImpl(const char* compName);
        void genTlm(U32 val);
        virtual ~TestTlmImpl();
        void init();
    protected:
        void aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
