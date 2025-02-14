/*
 * TestComponentImpl.hpp
 *
 *  Created on: Jul 1, 2015
 *      Author: tcanham
 */

#ifndef TEST_QUEUED1_TESTCOMPONENTIMPL_HPP_
#define TEST_QUEUED1_TESTCOMPONENTIMPL_HPP_

#include <Autocoders/Python/test/queued1/TestComponentAc.hpp>

namespace SvcTest {

    class TestComponentImpl: public AQueuedTest::TestComponentBase {
        public:
            TestComponentImpl(const char* compName);
            virtual ~TestComponentImpl();
        private:
            void aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6);
            void aport2_handler(FwIndexType portNum, I32 arg4, F32 arg5, bool arg6);

    };

} /* namespace SvcTest */

#endif /* TEST_QUEUED1_TESTCOMPONENTIMPL_HPP_ */
