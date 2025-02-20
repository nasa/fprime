/*
 * TestComponentImpl.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: tcanham
 */

#include <Autocoders/Python/test/queued1/TestComponentImpl.hpp>
#include <cstdio>

namespace SvcTest {

    TestComponentImpl::TestComponentImpl(const char* compName) : TestComponentBase(compName) {

    }

    TestComponentImpl::~TestComponentImpl() {
    }

    void TestComponentImpl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {
        printf("aport_handler called with %d, %f, %d,\n",arg4,arg5,arg6);
        // call dispatcher
        MsgDispatchStatus stat = this->doDispatch();
        printf("Dispatch status: %d\n",stat);
    }

    void TestComponentImpl::aport2_handler(FwIndexType portNum, I32 arg4, F32 arg5, bool arg6) {
        printf("aport2_handler called with %d, %f, %s.\n",arg4,arg5,arg6?"TRUE":"FALSE");
    }

} /* namespace SvcTest */
