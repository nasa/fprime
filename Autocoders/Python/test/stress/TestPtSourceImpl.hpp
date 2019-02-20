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
#if FW_OBJECT_NAMES == 1
        TestPtSourceImpl(const char* compName);
#else
        TestPtSourceImpl();
#endif
        virtual ~TestPtSourceImpl();
        void init(void);
        void aport_Test(I32 arg4, F32 arg5, U8 arg6);
        void aport2_Test2(I32 arg4, F32 arg5, Ref::Gnc::Quaternion arg6);
    protected:
    private:
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
