/*
 * TestComponentImpl.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: tcanham
 */

#include <Autocoders/Python/test/queued1/TestComponentImpl.hpp>
#include <stdio.h>

namespace SvcTest {

#if FW_OBJECT_NAMES == 1
    TestComponentImpl::TestComponentImpl(const char* compName) : TestComponentBase(compName) {
#else
    TestComponentImpl::TestComponentImpl() {
#endif

    }

    TestComponentImpl::~TestComponentImpl() {
    }

    void TestComponentImpl::aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6) {
        printf("aport_handler called with %d, %f, %d,\n",arg4,arg5,arg6);
        // call dispatcher
        MsgDispatchStatus stat = this->doDispatch();
        printf("Dispatch status: %d\n",stat);
    }

    void TestComponentImpl::aport2_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, bool arg6) {
        printf("aport2_handler called with %d, %f, %s.\n",arg4,arg5,arg6?"TRUE":"FALSE");
    }

} /* namespace SvcTest */
