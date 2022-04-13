/*
 * TestPrmImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTPARAMIMPL_HPP_
#define TESTPARAMIMPL_HPP_

#include <Autocoders/Python/test/param_string/TestComponentAc.hpp>
#include <Fw/Prm/PrmString.hpp>

class TestPrmImpl: public Prm::TestPrmComponentBase {
    public:
        TestPrmImpl(const char* compName);
        void genTlm(U32 val);
        virtual ~TestPrmImpl();
        void init();
        void printParam();
    protected:
        void aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6);
};

#endif /* TESTPARAMIMPL_HPP_ */
