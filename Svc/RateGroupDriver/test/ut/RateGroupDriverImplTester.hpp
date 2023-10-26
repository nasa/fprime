/*
 * RateGroupDriverImplTester.hpp
 *
 *  Created on: June 19, 2015
 *      Author: tcanham
 */

#ifndef RATEGROUPDRIVER_TEST_UT_RATEGROUPDRIVERIMPLTESTER_HPP_
#define RATEGROUPDRIVER_TEST_UT_RATEGROUPDRIVERIMPLTESTER_HPP_

#include <RateGroupDriverGTestBase.hpp>
#include <Svc/RateGroupDriver/RateGroupDriver.hpp>

namespace Svc {

    class RateGroupDriverImplTester: public RateGroupDriverGTestBase {
        public:
            RateGroupDriverImplTester(Svc::RateGroupDriver& inst);
            virtual ~RateGroupDriverImplTester();

            void init(NATIVE_INT_TYPE instance = 0);

            void runSchedNominal(Svc::RateGroupDriver::DividerSet dividersSet, NATIVE_INT_TYPE numDividers);

        private:

            void from_CycleOut_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart);

            Svc::RateGroupDriver& m_impl;

            void clearPortCalls();

            bool m_portCalls[3];

    };

} /* namespace Svc */

#endif /* RATEGROUPDRIVER_TEST_UT_ACTIVELOGGERIMPLTESTER_HPP_ */
