/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTPTSOURCEIMPL_HPP_
#define TESTPTSOURCEIMPL_HPP_

#include <Autocoders/Python/test/stress/TestPtComponentAc.hpp>

class TestPtSourceImpl: public StressTest::TestPortComponentBase {
    public:
        TestPtSourceImpl(const char* compName);
        virtual ~TestPtSourceImpl();
        void init();
        void aport_Test(I32 arg4, F32 arg5, U8 arg6);
        void aport2_Test2(I32 arg4, F32 arg5, const Ref::Gnc::Quaternion& arg6);
    protected:
    private:
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
