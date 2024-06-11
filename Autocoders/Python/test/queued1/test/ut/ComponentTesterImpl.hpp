/*
 * ComponentTesterImpl.hpp
 *
 *  Created on: Jul 1, 2015
 *      Author: tcanham
 */

#ifndef TEST_QUEUED1_TEST_UT_COMPONENTTESTERIMPL_HPP_
#define TEST_QUEUED1_TEST_UT_COMPONENTTESTERIMPL_HPP_

#include <Autocoders/Python/test/queued1/TestComponentTestAc.hpp>
#include <Autocoders/Python/test/queued1/TestComponentImpl.hpp>

namespace SvcTest {

    class ComponentTesterImpl: public AQueuedTest::TestTesterComponentBase {
        public:
            ComponentTesterImpl(const char* compName);
            virtual ~ComponentTesterImpl();
            void init();
            void runTest();
        private:
    };

} /* namespace SvcTest */

#endif /* TEST_QUEUED1_TEST_UT_COMPONENTTESTERIMPL_HPP_ */
